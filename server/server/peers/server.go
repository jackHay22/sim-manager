package peers

import (
	"fmt"
	"log"
	"net/http"
	"encoding/json"
)

/*
 * Server enqueues a segment to be processed
 * returns whether or not the buffer had space
 */
func (b *SegmentBuffer) Enqueue(segment ForwardedSegment) bool {
  b.mutex.Lock()
  defer b.mutex.Unlock()

  if len(b.segments) >= (b.cap - 1) {
    log.Printf("unable to enqueue segment to process, buffer full")
    return false;
  }
  b.segments = append(b.segments, segment)
  return true
}

/*
 * Server dequeues a segment to process
 * returns segment and whether or not the buffer contained a segment
 */
func (b *SegmentBuffer) Dequeue() (*ForwardedSegment, bool) {
  b.mutex.Lock()
  defer b.mutex.Unlock()

  if len(b.segments) > 0 {
    x := b.segments[0]
		b.segments = b.segments[1:]
    return &x, true
  }

  return nil, false
}

/*
 * Create a new segment buffer
 */
func NewSegmentBuffer(capacity int) (*SegmentBuffer) {
  return &SegmentBuffer{
    cap: capacity,
  }
}

/*
 * Handle the arrival of new segments for processing
 */
func segmentHandler(buffer *SegmentBuffer) http.HandlerFunc {
	return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
		var segment ForwardedSegment
		//decode the body of the request
		if jsonErr := json.NewDecoder(request.Body).Decode(segment); jsonErr != nil {
			log.Printf("failed to deserialize forwarded segment json: %v", jsonErr)
			writer.WriteHeader(http.StatusBadRequest)
			return
		}

		//enqueue a segment into the buffer
		buffer.Enqueue(segment)
	})
}

/*
 * Serve requests on a given port
 */
func StartServer(port int, id string, buffer *SegmentBuffer) {
	//Handle requests from other servers forwarding
	http.HandleFunc("/segment", segmentHandler(buffer))
	log.Printf("server %s can receive forwarded segments at http://0.0.0.0:%d/segment", port)

	//serve the endpoint
	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}
