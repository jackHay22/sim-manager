package server

import (
  "jackhay.io/segmentprovider/sim"
  "log"
  "net/http"
  "fmt"
)

/*
 * Handle requests for segments from servers (acting as tower)
 */
func towerHandler(provider *sim.SimInfo) http.HandlerFunc {
	return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
		//TODO
	})
}

func StartServer(port int, provider *sim.SimInfo) {
  //allow servers to download segments
  http.HandleFunc("/tower", towerHandler(provider))
	log.Printf("segment provider can serve segment info at http://0.0.0.0:%d/tower", port)

	//serve the endpoint
	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}
