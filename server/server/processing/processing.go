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
  currentTs := "0.0"

  //while the server is running, process
  for ;; {
    //get the vehicle coverage for this timestep
    if cov, err := segmentProvider.GetVehicles(currentTs); err == nil {

      //TODO processing

      log.Printf("finished processing for timestep %s", currentTs)
      //set the next timestep
      currentTs = cov.NextTs

      //TODO make sure the queue is empty

      if currentTs == "" {
        log.Fatalf("completed final timestep in simulation")
        return
      }

    } else {
      log.Fatalf("failed to get information from segment provider: %s", err)
    }
  }
}
