package processing

import (
  "log"
  "errors"
  "strings"
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
               coverage *segmentprovider.VehicleCoverage,
               forwardBuffer *peers.ForwardBuffer,
               peerForwarder *peers.Peers,
               segmentProvider *segmentprovider.SegmentProvider) (*implRes, error) {

  //create the args tuple
  args := python3.PyTuple_New(3)
  if args == nil {
    python3.PyErr_Print()
    return nil, errors.New("failed to create python args tuple")
  }

  defer args.DecRef()

  //get the components to transfer and add as args

  //get the segment ids that this server is responsible for
  if segmentsStr, segmentsErr := toJsonStr(segmentProvider.SegmentIds); segmentsErr == nil {
    python3.PyTuple_SetItem(args, 0, python3.PyUnicode_FromString(segmentsStr))
  } else {
    return nil, segmentsErr
  }

  //get the vehicles in range of the tower
  if coverageStr, coverageErr := toJsonStr(coverage.Vehicles); coverageErr == nil {
    python3.PyTuple_SetItem(args, 1, python3.PyUnicode_FromString(coverageStr))
  } else {
    return nil, coverageErr
  }

  //get the forward buffer (also acts as local storage)
  if bufferStr, bufferErr := toJsonStr(forwardBuffer.GetCurrentBuffer()); bufferErr == nil {
    python3.PyTuple_SetItem(args, 2, python3.PyUnicode_FromString(bufferStr))
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

  var result implRes
  if jsonErr := json.NewDecoder(strings.NewReader(jsonStr)).Decode(&result); jsonErr != nil {
    log.Printf("failed to deserialize implementation result as json: %v", jsonErr)
    return nil, jsonErr
  }
  return &result, nil
}

/*
 * Takes forwarded segment buffer, peer list, and provider that
 * manages timestep and vehicle connectivity information
 */
func StartProcessing(forwardBuffer *peers.ForwardBuffer,
                     peerForwarder *peers.Peers,
                     segmentProvider *segmentprovider.SegmentProvider) {

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
      implRes, implErr := sendState(mainFn, cov, forwardBuffer, peerForwarder, segmentProvider)
      if implErr != nil {
        log.Fatalf("implementation error: %s", implErr)
      }

      //forward segments
      for _, s := range implRes.toForward {
        peerForwarder.ForwardSegment(s.towerId, s.data)
      }

      //set updated buffer
      forwardBuffer.SetCurrentBuffer(implRes.buffer)
      bufferSize := 0
      for _, b := range implRes.buffer {
        for _, v := range b.VehicleHistory {
          if v.Downloaded {
            //count segments marked as downloaded in total storage
            bufferSize++
          }
        }
      }
      storageUsage = append(storageUsage, bufferSize)

      //notify segment provdider of downloaded segments
      segmentProvider.SetDownloaded(implRes.downloaded)
      bandwidthUsage = append(bandwidthUsage, len(implRes.downloaded))

      //set the next timestep
      currentTs++

      if cov.MaxTs < currentTs {
        //notify segment provider
        segmentProvider.Complete()

        //generate output
        WriteOutput(bandwidthUsage, storageUsage)

        //Note: need fatalf to exit
        log.Fatalf("completed final timestep (%d)", cov.MaxTs)
      }

    } else {
      log.Fatalf("failed to get information from segment provider: %s", err)
    }
  }
}
