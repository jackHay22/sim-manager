package processing

import (
  "log"
  "time"
  "jackhay.io/vehicleserver/peers"
  "jackhay.io/vehicleserver/segmentprovider"
)

/*
 * Server enqueues a segment to be processed
 * returns whether or not the buffer had space
 */
func (b *SegmentBuffer) Enqueue() bool {
  b.mutex.Lock()
  defer b.mutex.Unlock()

  if b.size >= (b.cap - 1) {
    log.Printf("unable to enqueue segment to process, buffer full")
    return false;
  }
  //enqueue
  b.size++
  return true
}

/*
 * Server dequeues a segment to process
 * returns whether or not the buffer contained a segment
 */
func (b *SegmentBuffer) Dequeue() bool {
  b.mutex.Lock()
  defer b.mutex.Unlock()

  if b.size > 0 {
    b.size--
    return true
  }

  return false
}

/*
 * Create a new segment buffer
 */
func NewSegmentBuffer(capacity int) (*SegmentBuffer) {
  return &SegmentBuffer{
    cap: capacity,
    size: 0,
  }
}

/*
 * Start the main server process
 * Takes the various constraints for this server
 * Takes the buffer of segments to process, buffer
 * of segments to forward to peers
 */
func StartProcessing(procConstraint int,
                     storeConstraint int,
                     bwConstraint int,
                     segmentBuffer *SegmentBuffer,
                     forwardBuffer *ForwardBuffer,
                     peerLookup *peers.PeerLookup,
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
