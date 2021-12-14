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
    //whether this segment has been downloaded
    Downloaded bool `json:"downloaded"`
  } `json:"hist"`
}

/*
 * Mark a segment that a vehicle has passed
 * as downloaded
 */
type DownloadedSegment struct {
  VehicleId string `json:"vehicle_id"`
  SegmentId string `json:"segment_id"`
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
