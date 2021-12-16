package peers

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"jackhay.io/vehicleserver/data"
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
			p.mapping[fmt.Sprintf("tower_%d", i)] = fmt.Sprintf("127.0.0.1:%d", portOffset + i)
		}
	}

	return &p
}

/*
 * Forward information about a segment to a different server
 */
func (l *Peers) ForwardSegment(serverId string, data data.VehicleData) {
	//the post body
	body, jsonErr := json.Marshal(data)
	if jsonErr != nil {
		log.Printf("error serializing data to send to peer %s: %v", serverId, jsonErr)
		return
	}
	postBody := bytes.NewBuffer(body)

	//identify the server to send to
	if peerAddr, found := l.mapping[serverId]; found {
		//make the request
		resp, postErr := http.Post(fmt.Sprintf("http://%s/forward", peerAddr), "application/json", postBody)
		if postErr != nil {
			log.Printf("error forwarding segment to server %s: %v", serverId, postErr)
			return
		}
		defer resp.Body.Close()

	} else {
		log.Printf("warning, server id not found in peer lookup: %s", serverId)
		return
	}
	return
}
