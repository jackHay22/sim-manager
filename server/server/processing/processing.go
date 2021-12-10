package processing

import (
  "log"
  "errors"
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
 * Send the current simulation state to the
 */
func sendState(mainFn *python3.PyObject,
               coverage *segmentprovider.VehicleCoverage,
               segmentBuffer *peers.SegmentBuffer,
               peerForwarder *peers.Peers,
               segmentProvider *segmentprovider.SegmentProvider) (*implRes, error) {

  //create the args tuple
  args := python3.PyTuple_New(3)
  if args == nil {
    python3.PyErr_Print()
    return nil, errors.New("failed to create python args tuple")
  }

  defer args.DecRef()

  //add the args as strings
  python3.PyTuple_SetItem(args, 0, python3.PyUnicode_FromString(segmentProvider.ToJson()))
  python3.PyTuple_SetItem(args, 1, python3.PyUnicode_FromString(coverage.ToJson()))
  python3.PyTuple_SetItem(args, 2, python3.PyUnicode_FromString(segmentBuffer.ToJson()))

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

  log.Printf("%s", jsonStr)
  //TODO
  return &implRes{}, nil
}

/*
 * Takes forwarded segment buffer, peer list, and provider that
 * manages timestep and vehicle connectivity information
 */
func StartProcessing(segmentBuffer *peers.SegmentBuffer,
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

  //while the server is running, process
  for ;; {
    //get the vehicle coverage for this timestep
    if cov, err := segmentProvider.GetVehicles(currentTs); err == nil {

      //send state to python implementation
      _, implErr := sendState(mainFn, cov, segmentBuffer, peerForwarder, segmentProvider)
      if implErr != nil {
        log.Fatalf("implementation error: %s", implErr)
      }

      // implRes

      //set the next timestep
      currentTs++

      if cov.MaxTs < currentTs {
        //notify segment provider
        segmentProvider.Complete()

        //Note: need fatalf to exit
        log.Fatalf("completed final timestep (%d)", cov.MaxTs)
      }

    } else {
      log.Fatalf("failed to get information from segment provider: %s", err)
    }
  }
}
