package peers

import (
  "jackhay.io/vehicleserver/data"
)

/*
 * Information forwarded between peers
 */
type ForwardedSegment struct {
  //the timestamp at which the segment was forwarded
  ForwardTs int `json:"forward_ts"`
  //information about the vehicle
  Vehicle data.VehicleData `json:"vehicle"`
}
