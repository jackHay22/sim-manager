package peers

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

/*
 * Add peers to the new lookup
 */
func NewPeerLookup(thisIdx int, peerCount int, portOffset int) *Peers {
	p := Peers{
		mapping: make(map[string]string),
	}

	//add mappings that don't correspond to the current server
	for i := 0; i<(peerCount + 1); i++ {
		if i != thisIdx {
			//add the mapping
			p.mapping[fmt.Sprintf("tower_%s", i)] = fmt.Sprintf("127.0.0.1:%d", portOffset + i)
		}
	}

	log.Printf("added %d peers to lookup", len(p.mapping))

	return &p
}

/*
 * Forward information about a segment to a different server
 */
func (l *Peers) ForwardSegment(serverId string, segmentId string, data string) bool {
	//the post body
	body, _ := json.Marshal(ForwardedSegment{
		SegmentId: segmentId,
		Data: data,
	})
	postBody := bytes.NewBuffer(body)

	//identify the server to send to
	if peerAddr, found := l.mapping[serverId]; found {
		//make the request
		resp, postErr := http.Post(fmt.Sprintf("http://%s/segment", peerAddr), "application/json", postBody)
		if postErr != nil {
			log.Printf("error forwarding segment to server %s: %v", serverId, postErr)
		}
		defer resp.Body.Close()
	} else {
		log.Printf("warning, server id not found in peer lookup: %s", serverId)
		return false
	}
	return true
}
