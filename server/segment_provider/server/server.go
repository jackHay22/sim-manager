package server

import (
  "jackhay.io/segmentprovider/sim"
  "github.com/gorilla/mux"
  "log"
  "net/http"
  "fmt"
  "encoding/json"
  "strconv"
)

/*
 * Response object used to
 * generate endpoint logging
 */
type LoggedResponse struct {
	http.ResponseWriter
	Status int
}

/*
 * Write the status of a request to the Response
 */
func (r *LoggedResponse) WriteHeader(stat int) {
	r.Status = stat
	r.ResponseWriter.WriteHeader(stat)
}

/*
 * Log requests
 */
func endpointLogger(handler http.HandlerFunc) http.HandlerFunc {
	return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
		res := LoggedResponse{
			ResponseWriter: writer,
			Status:         200,
		}

		//handle the request
		handler.ServeHTTP(&res, request)

    if res.Status != 200 {
      //log the request
      log.Printf("%s %s %s %d",
        request.Method,
        request.URL.Path,
        request.Proto,
        res.Status)
    }
	})
}

/*
 * Handle requests for segments from servers (acting as tower)
 * - At each given timestep, twoers connect to the segment provider,
 * download segments, and then process/forward segments to peers
 */
func towerHandler(provider *sim.SimInfo) http.HandlerFunc {
	return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
    if request.Method != "GET" {
      //bad request
      writer.WriteHeader(http.StatusBadRequest)
      log.Printf("invalid request for /tower: %s", request.Method)
      return
    }

    //get the variables from the request path
    vars := mux.Vars(request)
    towerid, tidOk := vars["towerid"]
    if !tidOk {
      writer.WriteHeader(http.StatusBadRequest)
      log.Printf("/tower request missing towerid")
      return
    }

    timestep, tsOk := vars["timestep"]
    if !tsOk {
      writer.WriteHeader(http.StatusBadRequest)
      log.Printf("/tower request missing timestep")
      return
    }

    //conver the timestep to an int
    ts, intErr := strconv.Atoi(timestep)
    if intErr != nil {
      writer.WriteHeader(http.StatusBadRequest)
      log.Printf("/tower request timestep not an int: %s", intErr)
      return
    }

    //request the vehicles currently connected for the timestep
    //Note: this will block until the timestep is ready
    if res, err := provider.VehiclesConnected(ts, towerid); err == nil {
    	if jsonResp, jsonErr := json.Marshal(res); jsonErr == nil {
        writer.Write(jsonResp)
        //respond with json
        writer.Header().Set("Content-Type", "application/json")
    	} else {
        log.Printf("failed to generate json response %s", jsonErr)
        writer.WriteHeader(http.StatusInternalServerError)
      }

    } else {
      log.Printf("failed to get vehicles connected to tower %s: %s", towerid, err)
      writer.WriteHeader(http.StatusInternalServerError)
    }
	})
}

/*
 * Handle requests from towers for the segments that they are
 * responsible for
 */
func coverageHandler(provider *sim.SimInfo) http.HandlerFunc {
  return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
    if request.Method != "GET" {
      //bad request
      writer.WriteHeader(http.StatusBadRequest)
      log.Printf("invalid request for /segments: %s", request.Method)
      return
    }

    //get the variables from the request path
    vars := mux.Vars(request)
    towerid, tidOk := vars["towerid"]
    if !tidOk {
      writer.WriteHeader(http.StatusBadRequest)
      log.Printf("/segments request missing towerid")
      return
    }

    if jsonResp, jsonErr := json.Marshal(provider.TowerCoverage(towerid)); jsonErr == nil {
      writer.Write(jsonResp)
      //respond with json
      writer.Header().Set("Content-Type", "application/json")
    } else {
      log.Printf("failed to generate json response %s", jsonErr)
      writer.WriteHeader(http.StatusInternalServerError)
    }
  })
}

/*
 * Accepts requests to mark segments as having been downloaded
 */
func downloadedHandler(provider *sim.SimInfo) http.HandlerFunc {
  return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
    if request.Method != "POST" {
      //bad request
      writer.WriteHeader(http.StatusBadRequest)
      log.Printf("invalid request for /downloaded: %s", request.Method)
      return
    }

    //deserialize and pass to siminfo
    var data []sim.DownloadedSegment
    //decode the body of the request
    if jsonErr := json.NewDecoder(request.Body).Decode(&data); jsonErr != nil {
      log.Printf("failed to deserialize downloaded segment json: %v", jsonErr)
      writer.WriteHeader(http.StatusBadRequest)
      return
    }

    //mark segments as downloaded
    provider.MarkDownloaded(data)
  })
}

/*
 * Towers make this request once they have completed the final timestep
 */
func completeHandler(provider *sim.SimInfo) http.HandlerFunc {
  completed := 0
  return http.HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
    completed++
    provider.TowersComplete(completed)
  })
}

/*
 * Register endpoints, start server
 */
func StartServer(port int, provider *sim.SimInfo) {
  r := mux.NewRouter()
  r.HandleFunc("/tower/{towerid}/{timestep}", endpointLogger(towerHandler(provider)))
	log.Printf("segment provider serving segment info at http://0.0.0.0:%d/tower/{towerid}/{timestep}", port)
  r.HandleFunc("/segments/{towerid}", endpointLogger(coverageHandler(provider)))
  log.Printf("segment provider serving segment info at http://0.0.0.0:%d/segments/{towerid}", port)
  r.HandleFunc("/complete", endpointLogger(completeHandler(provider)))
  log.Printf("segment provider serving completion endpoint at http://0.0.0.0:%d/complete", port)
  r.HandleFunc("/downloaded", endpointLogger(downloadedHandler(provider)))
  log.Printf("segment provider serving segment downloaded endpoint at http://0.0.0.0:%d/downloaded", port)

	//serve the endpoint
	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), r))
}
