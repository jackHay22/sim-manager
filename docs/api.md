# Api Specification for server processes

## Segment provider server

### Segment ids
`GET /segments/{towerid}`
- On startup, tower servers request a list of segment ids that they are responsible for. A single tower is responsible for each segment.
- Response:
```json
["id1", "id2", "id3"]
```
