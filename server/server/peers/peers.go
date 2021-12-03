package peers

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
)

/*
 * Load server topology from file
 */
func NewServerTopology(path *string) *ServerTopology {
	file, fileErr := os.Open(*path)
	if fileErr != nil {
		log.Fatalf("unable to open topology file %s: %v", *path, fileErr)
	}
	defer file.Close()

	//read the file
	fileBytes, readErr := ioutil.ReadAll(file)
	if readErr != nil {
		log.Fatalf("failed to read from topology file %s: %v", *path, readErr)
	}

	var topo ServerTopology
	jsonErr := json.Unmarshal(fileBytes, &topo)
	if jsonErr != nil {
		log.Fatalf("failed to parse topology file %s: %v", *path, jsonErr)
	}

	return &topo
}

/*
 * Add peers to the new lookup
 */
func NewPeerLookup(topo *ServerTopology, serverIdx int) *Peers {
	p := Peers{
		mapping: make(map[string]string),
	}

	//add mappings that don't correspond to the current server
	for i, a := range topo.Servers {
		if i != serverIdx {
			p.mapping[a.Id] = fmt.Sprintf("%s:%d", a.Addr, a.Port)
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
