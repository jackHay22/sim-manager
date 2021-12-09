package processing

import (
  "log"
  "jackhay.io/vehicleserver/peers"
  "jackhay.io/vehicleserver/segmentprovider"
)

/*
 * Start the main server process
 * Takes the various constraints for this server
 * Takes the buffer of segments to process, buffer
 * of segments to forward to peers
 */
func StartProcessing(procConstraint int,
                     storeConstraint int,
                     bwConstraint int,
                     segmentBuffer *peers.SegmentBuffer,
                     peerForwarder *peers.Peers,
                     segmentProvider *segmentprovider.SegmentProvider) {
  log.Printf("processing thread starting with server constraints: processing: %d, storage: %d, bandwidth: %d",
             procConstraint,
             storeConstraint,
             bwConstraint)

  //the initial timestep to request
  currentTs := 1

  //while the server is running, process
  for ;; {
    //get the vehicle coverage for this timestep
    if cov, err := segmentProvider.GetVehicles(currentTs); err == nil {

      //TODO processing

      //set the next timestep
      currentTs++

      //TODO make sure the queue is empty
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
