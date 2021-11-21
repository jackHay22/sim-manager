package sim

/*
 * Note: these formats are described in
 * docs/output.md
 */

/*
 * Defines communications between towers and
 * vehicles
 */
type towerOutput struct {
	//the ids of vehicles in the simulation
	VehicleIds []string `json:"vehicle_ids"`
	//tower communications
	Towers []struct {
		//the id of this tower
		TowerId string `json:"tower_id"`
		//vehicles that have communicated with this tower
		Vehicles []struct {
			//timestep
			Ts float64 `json:"ts"`
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
			Ts float64 `json:"ts"`
			//Segment index, how long ago in
			//timesteps vehicle passed segment
			V [][]float64 `json:"s"`
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
		//segments covered by this tower
		Segments []int `json:"segments"`
	} `json:"towers"`
}

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
	//all timesteps (converted from floats to strings for associative lookup)
	timesteps []string
	//map from each tower id to all timesteps for that tower
	//to all vehicles connected at that timestep
	towers map[string]map[string][]vehicleDist
}

/*
 * Holds simulation information
 */
type SimInfo struct {
	//tower - vehicle connectivity for each timestep
	towerCoverage towerCoverageLookup
}
