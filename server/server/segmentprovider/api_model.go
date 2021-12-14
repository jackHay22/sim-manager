package segmentprovider

import (
  "jackhay.io/vehicleserver/data"
)

/*
 * Vehicles connected to a tower at a given timestep
 */
type VehicleCoverage struct {
  //Vehicles in range and their distance
  Vehicles []data.VehicleData `json:"vehicles"`
  //the max timestep (inclusive)
  MaxTs int `json:"max_ts"`
}

/*
 * Tower/segment assignments sent to towers
 * before starting the simulation
 */
type TowerSegmentAssignments struct {
  Towers []struct {
    //the id of this tower
    TowerId string `json:"tower_id"`
    //segments this tower is responsible for
    Segments []string `json:"segments"`
  } `json:"towers"`
}
