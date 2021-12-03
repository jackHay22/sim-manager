package peers

type ForwardedSegment struct {
  //how long ago in timesteps the vehicle was on this segment
  SegmentId string `json:"segment_id"`
  //the segment id
  Data string `json:"id"`
}
