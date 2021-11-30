package segmentprovider

/*
 * Vehicles connected to a tower at a given timestep
 */
type VehicleCoverage struct {
  //Vehicles in range and their distance
  Vehicles []struct {
    //the id of the vehicle
    VehicleId string `json:"id"`
    //the distance to the tower
    Distance float64 `json:"dist"`
    //the history of the vehicle
    VehicleHistory []struct {
      //how long ago in timesteps the vehicle was on this segment
      Elapsed int `json:"elapsed"`
      //the segment id
      SegmentId string `json:"id"`
    } `json:"hist"`
  } `json:"vehicles"`
  //the next timestep to request
  NextTs string `json:"next_ts"`
}
