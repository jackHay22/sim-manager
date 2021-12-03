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

	topoPtr := flag.String("topo", "", "server topology file")
	idxPtr := flag.Int("idx", -1, "the index of this server in the topology file")
	segmentProviderPtr := flag.String("sprov", "127.0.0.1:8080", "address of the segment provider")

	flag.Parse()

	if len(*topoPtr) == 0 {
		log.Fatalf("topology file must be specified")
	}

	if *idxPtr < 0 {
		log.Fatalf("index of this server in toplogy file must be specified")
	}

	//load the server topology
	topo := peers.NewServerTopology(topoPtr)

	if *idxPtr >= len(topo.Servers) {
		log.Fatalf("server index out of bounds: %d", *idxPtr)
	}

	//populate peers
	peerLookup := peers.NewPeerLookup(topo, *idxPtr)

	//extract information about this server
	procConstraint := topo.Servers[*idxPtr].ProcC
	storeConstraint := topo.Servers[*idxPtr].StoreC
	bandwidthConstraint := topo.Servers[*idxPtr].BandC
	port := topo.Servers[*idxPtr].Port
	id := topo.Servers[*idxPtr].Id

	//set the correct logging prefix
	log.SetPrefix(fmt.Sprintf("vehicleserver %s ", id))

	//create server buffers
	segmentBuffer := peers.NewSegmentBuffer(storeConstraint)

	//create a segment provider (connects to provider, downloads segments this
	//server/tower is responsible for)
	segmentProvider, spErr := segmentprovider.NewSegmentProvider(id, *segmentProviderPtr)
	if spErr != nil {
		log.Fatalf("failed to connect to segment provider: %d", *idxPtr)
	}

	//start the server processing system
	go processing.StartProcessing(procConstraint,
		storeConstraint,
		bandwidthConstraint,
		segmentBuffer,
		peerLookup,
		segmentProvider)

	//start the server
	peers.StartServer(port, id, segmentBuffer)
}
