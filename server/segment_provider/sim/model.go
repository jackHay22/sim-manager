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
 * Holds simulation information
 */
type SimInfo struct {
	towerData   towerOutput
	vehicleData vehicleOutput
	segmentData segmentOutput
}
