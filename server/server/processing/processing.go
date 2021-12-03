package processing

import (
  "log"
  "time"
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

  //while the server is running, process
  for ;; {
    time.Sleep(1 * time.Second)
  }
}
