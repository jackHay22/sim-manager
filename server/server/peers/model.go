package peers

import "sync"

/*
 * Server topology format
 */
type ServerTopology struct {
	Servers []struct {
		Id     string `json:"id"`
		Addr   string `json:"addr"`
		Port   int    `json:"port"`
		ProcC  int    `json:"processing_constraint"`
		StoreC int    `json:"storage_constraint"`
		BandC  int    `json:"bandwidth_constraint"`
	} `json:"servers"`
}

/*
 * Mapping containing information
 * about the server's peers
 * (static membership)
 */
type Peers struct {
	//map id to address
	mapping map[string]string
}

/*
 * The current buffer on this server
 * This is the processing storage for a given server
 */
type SegmentBuffer struct {
	//mutex for controlling access to buffer
	mutex sync.Mutex
	//the enqueued forwarded segments
	segments []ForwardedSegment
	//the capacity of the buffer
	cap int
}
