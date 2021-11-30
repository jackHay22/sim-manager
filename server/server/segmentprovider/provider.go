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
func NewSegmentProvider(towerid string, segmentProviderAddr string) (*SegmentProvider, error) {
  prov := SegmentProvider{
      segmentProviderAddr: segmentProviderAddr,
  }

  getPath := fmt.Sprintf("http://%s/segments/%s", segmentProviderAddr, towerid)
  log.Printf("requesting segments: %s", getPath)

  //request segment ids from the server
  if resp, err := http.Get(getPath); err != nil {
    defer resp.Body.Close()

    if jsonErr := json.NewDecoder(resp.Body).Decode(prov.SegmentIds); jsonErr != nil {
      log.Printf("failed to deserialize json: %v", jsonErr)
      return nil, jsonErr
    }

  } else {
    log.Printf("error requesting segments: %v", err)
    return nil, err
  }

  return &prov, nil
}
