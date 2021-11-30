package segmentprovider

/*
 * Defines tower state from segment provider
 */
type SegmentProvider struct {
  //the address of the segment provider
  segmentProviderAddr string
  //the id of this tower
  towerId string
  //the segments that this tower is responsible for
  SegmentIds []string
}
