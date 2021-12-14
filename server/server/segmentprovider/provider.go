package segmentprovider

import (
  "net/http"
  "log"
  "fmt"
  "encoding/json"
  "bytes"
  "jackhay.io/vehicleserver/data"
)

/*
 * Load the segment provider
 */
func NewSegmentProvider(towerId string, segmentProviderAddr string) (*SegmentProvider, error) {
  prov := SegmentProvider{
      segmentProviderAddr: segmentProviderAddr,
      towerId: towerId,
  }

  getPath := fmt.Sprintf("http://%s/segments/%s", segmentProviderAddr, towerId)

  //request segment ids from the server
  if resp, err := http.Get(getPath); err == nil {
    defer resp.Body.Close()

    if jsonErr := json.NewDecoder(resp.Body).Decode(&prov.SegmentIds); jsonErr != nil {
      log.Printf("failed to deserialize json: %v", jsonErr)
      return nil, jsonErr
    }

  } else {
    log.Printf("error requesting segments: %v", err)
    return nil, err
  }

  return &prov, nil
}

/*
 * Mark segments as downloaded
 */
func (p *SegmentProvider) SetDownloaded(segments []data.DownloadedSegment) {
  //the post body
	body, jsonErr := json.Marshal(segments)
	if jsonErr != nil {
		log.Printf("error serializing downloaded segments to send to segment provider: %v", jsonErr)
		return
	}
	postBody := bytes.NewBuffer(body)

  //make the request
  resp, postErr := http.Post(fmt.Sprintf("http://%s/downloaded", p.segmentProviderAddr), "application/json", postBody)
  if postErr != nil {
    log.Printf("error forwarding downloaded segments to segment provider %s: %v", postErr)
    return
  }
  defer resp.Body.Close()
}

/*
 * Called when simulation has completed
 */
func (p *SegmentProvider) Complete() {
  getPath := fmt.Sprintf("http://%s/complete", p.segmentProviderAddr)
  if resp, err := http.Get(getPath); err == nil {
    defer resp.Body.Close()
  }
}

/*
 * For a given timestep, get connected vehicles
 */
func (p *SegmentProvider) GetVehicles(ts int) (*VehicleCoverage, error) {
  var cov VehicleCoverage

  getPath := fmt.Sprintf("http://%s/tower/%s/%d", p.segmentProviderAddr, p.towerId, ts)

  //request connected vehicles
  if resp, err := http.Get(getPath); err == nil {
    defer resp.Body.Close()

    if jsonErr := json.NewDecoder(resp.Body).Decode(&cov); jsonErr != nil {
      log.Printf("failed to deserialize json: %v", jsonErr)
      return nil, jsonErr
    }

  } else {
    log.Printf("error requesting vehicles: %v", err)
    return nil, err
  }

  return &cov, nil
}
