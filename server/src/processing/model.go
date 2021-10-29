package processing

import (
  "sync"
)

/*
 * The current buffer on this server
 * This is the processing storage for a given server
 */
type SegmentBuffer struct {
  //mutex for controlling access to buffer
  mutex sync.Mutex
  //the capacity of the buffer
  cap int
  //the current size of the buffer
  size int
}

/*
 * Buffer of segment information to be forwarded to
 * appropriate server
 */
type ForwardBuffer struct {

}
