package segmentprovider

import (
  "net/http"
  "log"
  "fmt"
  "encoding/json"
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
  log.Printf("requesting segments: %s", getPath)

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
 * For a given timestep, get connected vehicles
 */
func (p *SegmentProvider) GetVehicles(ts int) (*VehicleCoverage, error) {
  var cov VehicleCoverage

  getPath := fmt.Sprintf("http://%s/tower/%s/%d", p.segmentProviderAddr, p.towerId, ts)
  log.Printf("requesting vehicles: %s", getPath)

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
