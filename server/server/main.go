package main

import (
	"flag"
	"jackhay.io/vehicleserver/peers"
	"jackhay.io/vehicleserver/processing"
	"jackhay.io/vehicleserver/server"
	"log"
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
	log.SetPrefix("vehicleserver ")

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

	//create server buffers
	segmentBuffer := processing.NewSegmentBuffer(storeConstraint)
	forwardBuffer := processing.ForwardBuffer{}

	//start the server processing system
	go processing.StartProcessing(procConstraint,
		storeConstraint,
		bandwidthConstraint,
		segmentBuffer,
		&forwardBuffer,
		peerLookup)

	//start the server
	server.StartServer(port, id, segmentBuffer)
}
