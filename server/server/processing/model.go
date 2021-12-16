package processing

import (
	"jackhay.io/vehicleserver/data"
)

/*
 * Defines the decisions made by the
 * algorithm. This includes:
 * - Segments to download and buffer
 * - Segments to process
 * - Segments to forward to peers
 */
type ImplRes struct {
  //the segments to forward to peers
  ToForward []struct {
    TowerId string `json:"tower_id"`
    Data data.VehicleData `json:"data"`
  } `json:"to_forward"`
  //the updated buffer (forwarded and stored)
  Buffer []data.VehicleData `json:"buffer"`
	//vehicle_id, segment_id pairs that the server downloaded
	Downloaded []data.DownloadedSegment `json:"downloaded"`
}
