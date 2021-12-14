package segmentprovider

/*
 * Defines tower state from segment provider
 */
type SegmentProvider struct {
  //the address of the segment provider
  segmentProviderAddr string
  //the id of this tower
  towerId string
  //mappings from tower to segments the tower is responsible for
  Segments TowerSegmentAssignments
}
