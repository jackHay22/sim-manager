package main

import (
	"log"
  "flag"
  "jackhay.io/vehicleserver/server"
  "jackhay.io/vehicleserver/processing"
)

//default server constraints
const procDefault = 1
const storeDefault = 1
const bwDefault = 1

/*
 * Entrypoint
 * -port  the port to serve endpoints on
 * -id    the id of this server
 * -proc  the processing constraint for this server
 * -store the storage constraint for this server
 * -bw    the bandwidth constraint for this server
 */
func main() {
	log.SetPrefix("sim-server ")

  portPtr := flag.Int("port", 8080, "port to serve requests on")
  idPtr := flag.String("id", "", "id of this server")
  procPtr := flag.Int("proc", procDefault, "the processing constraint for this server")
  storePtr := flag.Int("store", storeDefault, "the storage constraint for this server")
  bwPtr := flag.Int("bw", bwDefault, "the bandwidth constraint for this server")

  flag.Parse()

  if len(*idPtr) == 0 {
    log.Fatalf("server id must be specified")
  }

  //create server buffers
  segmentBuffer := processing.NewSegmentBuffer(*storePtr)
  forwardBuffer := processing.ForwardBuffer{

  }

  //start the server processing system
  go processing.StartProcessing(*procPtr, *storePtr, *bwPtr, segmentBuffer, &forwardBuffer)

	//start the server
	server.StartServer(*portPtr, *idPtr, segmentBuffer)
}
