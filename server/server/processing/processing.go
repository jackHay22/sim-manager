package processing

import (
  "log"
  "errors"
  "fmt"
  "encoding/json"
  "jackhay.io/vehicleserver/peers"
  "jackhay.io/vehicleserver/segmentprovider"
  "github.com/DataDog/go-python3"
)

/*
 * Initialize python, find main function
 */
func initPythonImpl() *python3.PyObject {
  sysPath := python3.PySys_GetObject("path")
  python3.PyList_Append(sysPath, python3.PyUnicode_FromString("./server/python"))

  //load the module
  coreModule := python3.PyImport_ImportModule("impl")
  if coreModule == nil {
    python3.PyErr_Print()
    return nil
  }

  if mainFn := coreModule.GetAttrString("main"); mainFn == nil {
    python3.PyErr_Print()
    return nil
  } else {
    //return the main function
    return mainFn
  }
}

/*
 * Convert data to a json string representation
 */
func toJsonStr(val interface{}) (string, error) {
  if b, err := json.Marshal(val); err == nil {
    return string(b), nil
  } else {
    return "", err
  }
}

/*
 * Send the current simulation state to the
 */
func sendState(mainFn *python3.PyObject,
               towerid string,
               coverage *segmentprovider.VehicleCoverage,
               forwardBuffer *peers.ForwardBuffer,
               peerForwarder *peers.Peers,
               segmentProvider *segmentprovider.SegmentProvider) (*ImplRes, error) {

  //create the args tuple
  args := python3.PyTuple_New(4)
  if args == nil {
    python3.PyErr_Print()
    return nil, errors.New("failed to create python args tuple")
  }

  defer args.DecRef()

  //get the components to transfer and add as args

  //tower id as string
  python3.PyTuple_SetItem(args, 0, python3.PyUnicode_FromString(towerid))

  //get the segment ids that this server is responsible for
  if segmentsStr, segmentsErr := toJsonStr(segmentProvider.Segments); segmentsErr == nil {
    python3.PyTuple_SetItem(args, 1, python3.PyUnicode_FromString(segmentsStr))
  } else {
    return nil, segmentsErr
  }

  //get the vehicles in range of the tower
  if coverageStr, coverageErr := toJsonStr(coverage.Vehicles); coverageErr == nil {
    python3.PyTuple_SetItem(args, 2, python3.PyUnicode_FromString(coverageStr))
  } else {
    return nil, coverageErr
  }

  //get the forward buffer (also acts as local storage)
  if bufferStr, bufferErr := toJsonStr(forwardBuffer.GetCurrentBuffer()); bufferErr == nil {
    python3.PyTuple_SetItem(args, 3, python3.PyUnicode_FromString(bufferStr))
  } else {
    return nil, bufferErr
  }

  //call function
	callRes := mainFn.Call(args, python3.PyDict_New())

	if (callRes == nil) || (python3.PyErr_Occurred() != nil) {
    python3.PyErr_Print()
		return nil, errors.New("failed to call main function")
	}
	defer callRes.DecRef()

	//extract output
	jsonStr := python3.PyUnicode_AsUTF8(callRes)
	if python3.PyErr_Occurred() != nil {
		return nil, errors.New("failed to parse string from python response")
	}

  var result ImplRes
  if jsonErr := json.Unmarshal([]byte(jsonStr), &result); jsonErr != nil {
    log.Printf("failed to deserialize implementation result as json: %v", jsonErr)
    return nil, jsonErr
  }
  return &result, nil
}

/*
 * Takes forwarded segment buffer, peer list, and provider that
 * manages timestep and vehicle connectivity information
 */
func StartProcessing(towerid string,
                     forwardBuffer *peers.ForwardBuffer,
                     peerForwarder *peers.Peers,
                     segmentProvider *segmentprovider.SegmentProvider,
                     outputDir string) {

  //initialize python3
  python3.Py_Initialize()
  defer python3.Py_Finalize()

  mainFn := initPythonImpl()
  if mainFn == nil {
    log.Fatalf("failed to initialize")
  }

  //the initial timestep to request
  currentTs := 1

  //for each timestep, record the number of segments downloaded
  var bandwidthUsage []int
  //for each timestep, record the total memory usage
  var storageUsage []int

  //while the server is running, process
  for ;; {
    //get the vehicle coverage for this timestep
    if cov, err := segmentProvider.GetVehicles(currentTs); err == nil {

      //send state to python implementation
      implRes, implErr := sendState(mainFn, towerid, cov, forwardBuffer, peerForwarder, segmentProvider)
      if implErr != nil {
        log.Fatalf("implementation error: %s", implErr)
      }

      //forward segments
      for _, s := range implRes.ToForward {
        peerForwarder.ForwardSegment(s.TowerId, s.Data)
      }

      //set updated buffer
      forwardBuffer.SetCurrentBuffer(implRes.Buffer)
      bufferSize := 0
      for _, b := range implRes.Buffer {
        for _, v := range b.VehicleHistory {
          if v.Downloaded {
            //count segments marked as downloaded in total storage
            bufferSize++
          }
        }
      }
      storageUsage = append(storageUsage, bufferSize)

      //notify segment provdider of downloaded segments
      segmentProvider.SetDownloaded(implRes.Downloaded)
      bandwidthUsage = append(bandwidthUsage, len(implRes.Downloaded))

      //set the next timestep
      currentTs++

      if cov.MaxTs < currentTs {
        //notify segment provider
        segmentProvider.Complete()

        outPath := fmt.Sprintf("%s/server_stats_tower_%s.csv", outputDir, towerid)

        //generate output
        WriteOutput(bandwidthUsage, storageUsage, outPath)

        //Note: need fatalf to exit
        log.Fatalf("completed final timestep (%d), wrote to %s", cov.MaxTs, outPath)
      }

    } else {
      log.Fatalf("failed to get information from segment provider: %s", err)
    }
  }
}
