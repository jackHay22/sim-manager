package main

import (
	"flag"
	"jackhay.io/vehicleserver/peers"
	"jackhay.io/vehicleserver/processing"
	"jackhay.io/vehicleserver/segmentprovider"
	"log"
	"fmt"
)

//default server constraints
const procDefault = 1
const storeDefault = 1
const bwDefault = 1

/*
 * Entrypoint
 * -topo  the server topology (list of peers Note: includes self)
 * -proc  the processing constraint for this server
 * -store the storage constraint for this server
 * -bw    the bandwidth constraint for this server
 */
func main() {
	log.SetPrefix("vehicleserver - ")

	idxPtr := flag.Int("idx", -1, "the index of this server")
	peerCountPtr := flag.Int("peer-count", -1, "the number of peer servers")
	portOffset := flag.Int("port-range-start", 9000, "the start of the port range")
	segmentProviderPtr := flag.String("sprov", "127.0.0.1:8080", "address of the segment provider")

	flag.Parse()

	if *idxPtr < 0 {
		log.Fatalf("server index must be specified")
	}

	if *peerCountPtr < 0 {
		log.Fatalf("number of peers must be specified")
	}

	towerId := fmt.Sprintf("tower_%d", *idxPtr)

	//populate peers
	peerLookup := peers.NewPeerLookup(*idxPtr, *peerCountPtr, *portOffset)

	//set the correct logging prefix
	log.SetPrefix(fmt.Sprintf("vehicleserver %s ", towerId))

	//create server buffers
	segmentBuffer := peers.NewSegmentBuffer()

	//create a segment provider (connects to provider, downloads segments this
	//server/tower is responsible for)
	segmentProvider, spErr := segmentprovider.NewSegmentProvider(towerId, *segmentProviderPtr)
	if spErr != nil {
		log.Fatalf("failed to connect to segment provider: %s", spErr)
	}

	//start the server processing system
	go processing.StartProcessing(segmentBuffer,
		peerLookup,
		segmentProvider)

	//start the server
	peers.StartServer(*portOffset + *idxPtr, towerId, segmentBuffer)
}
