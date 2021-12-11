package peers

import (
	"jackhay.io/vehicleserver/data"
	"sync"
)

/*
 * Mapping containing information
 * about the server's peers
 * (static membership)
 */
type Peers struct {
	//map id to address
	mapping map[string]string
}

/*
 * The current buffer on this server
 * This is the processing storage for a given server
 */
type ForwardBuffer struct {
	//mutex for controlling access to buffer
	mutex sync.Mutex
	//the data available at this timestep
	data []data.VehicleData
	//data arriving for the next timestep
	//these are merged into data for the next ts
	nextData []data.VehicleData
}
