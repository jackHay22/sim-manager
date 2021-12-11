package data

/*
 * Data about a vehicle
 */
type VehicleData struct {
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
}
