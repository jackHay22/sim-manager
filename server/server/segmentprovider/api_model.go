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
