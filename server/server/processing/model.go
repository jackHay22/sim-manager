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
type implRes struct {
  //the segments to forward to peers
  toForward []struct {
    towerId string `json:"tower_id"`
    data data.VehicleData `json:"data"`
  } `json:"to_forward"`
  //the updated buffer (forwarded and stored)
  buffer []data.VehicleData `json:"buffer"`
	//vehicle_id, segment_id pairs that the server downloaded
	downloaded []data.DownloadedSegment `json:"downloaded"`
}
