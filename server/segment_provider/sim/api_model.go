package sim

type vehicleHist struct {
  //how long ago in timesteps the vehicle was on this segment
  Elapsed int `json:"elapsed"`
  //the segment id
  SegmentId string `json:"id"`
}

type vehicle struct {
  //the id of the vehicle
  VehicleId string `json:"id"`
  //the distance to the tower
  Distance float64 `json:"dist"`
  //the history of the vehicle
  VehicleHistory []vehicleHist `json:"hist"`
}

/*
 * Vehicles connected to a tower at a given timestep
 */
type vehicleCoverage struct {
  //Vehicles in range and their distance
  Vehicles []vehicle `json:"vehicles"`
  //the max timestep (inclusive)
  MaxTs int `json:"max_ts"`
}
