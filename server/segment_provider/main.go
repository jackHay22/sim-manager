package main

import (
	"flag"
	"jackhay.io/segmentprovider/sim"
	"jackhay.io/segmentprovider/server"
	"log"
)

func main() {
	log.SetPrefix("segmentprovider ")

	portPtr := flag.Int("port", 8080, "port to serve requests on")

	//simulation output files
	towerPtr := flag.String("tower-output", "", "simulation tower comm. output file")
	vehiclePtr := flag.String("vehicle-output", "", "simulation vehicle history output file")
	segmentPtr := flag.String("segment-output", "", "simulation tower segment coverage output file")

	flag.Parse()

	if len(*towerPtr) == 0 {
		log.Fatalf("tower-output file must be specified")
	}

	if len(*vehiclePtr) == 0 {
		log.Fatalf("vehicle-output file must be specified")
	}

	if len(*segmentPtr) == 0 {
		log.Fatalf("segment-output file must be specified")
	}

	//load simulation data
	simInfo := sim.LoadSimInfo(towerPtr, vehiclePtr, segmentPtr)

	//start the server to distribute simulation info
	server.StartServer(*portPtr, simInfo)
}
