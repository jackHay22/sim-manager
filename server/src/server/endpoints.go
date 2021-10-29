package server

import (
  "log"
  "net/http"
  "fmt"
  "jackhay.io/vehicleserver/processing"
)

/*
 * Handle the arrival of new segments for processing
 */
func segmentHandler(buffer *processing.SegmentBuffer) http.HandlerFunc {
   return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
     //enqueue a segment into the buffer
     buffer.Enqueue()
   })
}

/*
 * Serve requests on a given port
 */
func StartServer(port int, id string, buffer *processing.SegmentBuffer) {
  //Handle requests from other servers forwarding
  http.HandleFunc("/segment", segmentHandler(buffer))

	log.Printf("server %s serving requests at 0.0.0.0:%d", id, port)
	//serve the endpoint
	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}
