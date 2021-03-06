package sim

import (
	"sync"
)

/*
 * Note: the next three struct json formats are described in
 * docs/output.md
 */

/*
 * Defines communications between towers and
 * vehicles
 */
type towerOutput struct {
	//the ids of vehicles in the simulation
	VehicleIds []string `json:"vehicles"`
	//tower communications
	Towers []struct {
		//the id of this tower
		TowerId string `json:"tower_id"`
		//vehicles that have communicated with this tower
		Vehicles []struct {
			//timestep
			Ts int `json:"ts"`
			//Vehicle index, distance from tower
			V [][]float64 `json:"v"`
		} `json:"vehicles"`
	} `json:"towers"`
}

/*
 * Defines the route history of a vehicle
 */
type vehicleOutput struct {
	//the segments in the simulation
	Segments []string `json:"segments"`
	//vehicle positions in network at each ts
	Vehicles []struct {
		//the id of this vehicle
		VehicleId string `json:"vehicle_id"`
		//timestep and segments passed by vehicle
		Segments []struct {
			//timestep
			Ts int `json:"ts"`
			//Segment index, how long ago in
			//timesteps vehicle passed segment
			S [][]float64 `json:"s"`
		} `json:"segments"`
	} `json:"vehicles"`
}

/*
 * Defines tower coverage of segments
 */
type segmentOutput struct {
	//the segments in the simulation
	Segments []string `json:"segments"`
	//segments covered by each tower
	Towers []struct {
		//the id of this tower
		TowerId string `json:"tower_id"`
		//for each segment, distance to tower
		Segments []float64 `json:"segments"`
	} `json:"towers"`
}

/*
 * The remaining structs define runtime structures
 */

/*
 * Pair of vehicle id, distance to tower
 */
type vehicleDist struct {
	//id of the vehicle
	vehicleId string
	//distance to the tower
	dist float64
}

/*
 * For each tower, vehicles in range
 * at each timestep
 */
type towerCoverageLookup struct {
	//map from each tower id to all timesteps for that tower
	//to all vehicles connected at that timestep
	towers map[string]map[int][]vehicleDist
}

type segmentPos struct {
	//the id of the segment
	segmentId string
	//how long ago in timesteps
	tsAgo int
}

/*
 * History of segments that each vehicle in the sim passed
 */
type vehicleHistLookup struct {
	//map from vehicle id to timesteps to lists of segments visited
	//(includes timesteps since this position)
	vehicles map[string]map[int][]segmentPos
}

/*
 * Holds simulation information
 */
type SimInfo struct {
	//tower - vehicle connectivity for each timestep
	towerCoverage towerCoverageLookup
	//vehicle history lookup
	vehicleHist vehicleHistLookup
	//map from tower id to the segments it is responsible for
	towerAssignments TowerSegmentAssignments
	//mutex for downloaded segments lookup
	downloadedSegmentsLock sync.Mutex
	//segments that have already been downloaded
	downloadedSegments map[string]map[string]bool
	//the current timestep
	currentTs int
	//the max timestep
	maxTs int
	//the total number of towers
	towers int
	//towers waiting for the next timestep
	towersWaiting int
	//mutex for waiting for next timestep
	cMutex sync.Mutex
	//condition variable for towers waiting for the next timestep
	cond *sync.Cond
}
