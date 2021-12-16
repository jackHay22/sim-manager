package peers

import (
	"fmt"
	"log"
	"net/http"
	"encoding/json"
	"jackhay.io/vehicleserver/data"
)

/*
 * Server enqueues data to be processed (sent by peer)
 */
func (b *ForwardBuffer) Enqueue(data data.VehicleData) {
  b.mutex.Lock()
  defer b.mutex.Unlock()
  b.data = append(b.data, data)
}

/*
 * Get the current buffer for use
 */
func (b *ForwardBuffer) GetCurrentBuffer() []data.VehicleData {
	b.mutex.Lock()
  defer b.mutex.Unlock()

	//get a copy of the current buffer
	cpy := make([]data.VehicleData, len(b.data))
	copy(cpy, b.data)
	return cpy
}

/*
 * Update the data buffer (completed processing step)
 * Also, merge data for next timestep into buffer
 */
func (b *ForwardBuffer) SetCurrentBuffer(buffer []data.VehicleData)  {
	b.mutex.Lock()
  defer b.mutex.Unlock()
	b.data = buffer
	b.data = append(b.data, b.nextData...)
	b.nextData = nil
}

/*
 * Create a new segment buffer
 */
func NewForwardBuffer() (*ForwardBuffer) {
  return &ForwardBuffer{}
}

/*
 * Handle the arrival of new segments for processing
 */
func forwardHandler(buffer *ForwardBuffer) http.HandlerFunc {
	return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
		var data data.VehicleData
		//decode the body of the request
		if jsonErr := json.NewDecoder(request.Body).Decode(&data); jsonErr != nil {
			log.Printf("failed to deserialize forwarded json: %v", jsonErr)
			writer.WriteHeader(http.StatusBadRequest)
			return
		}

		//enqueue a segment into the buffer
		buffer.Enqueue(data)
	})
}

/*
 * Serve requests on a given port
 */
func StartServer(port int, id string, buffer *ForwardBuffer) {
	//Handle requests from other servers forwarding
	http.HandleFunc("/forward", forwardHandler(buffer))
	//serve the endpoint
	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}
