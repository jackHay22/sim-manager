# Api Specification for server processes

## Segment provider server

### Segment ids
`GET /segments/{towerid}`
- On startup, tower servers request a list of segment ids that they are responsible for. A single tower is responsible for each segment.
- Response:
```json
["id1", "id2", "id3"]
```

### Vehicles in range
`GET /tower/{towerid}/{timestep}`
- For a given timestep and tower, get all vehicles currently in range of the tower, each of their distances, and the history of segments that vehicle has passed
- Response:
```json
{
  "vehicles" : [
    {
      "id" : "some_vehicle_id",
      "dist" : 0.0,
      "hist" : [
        {
          "elapsed" : 0.0,
          "id" : "some_segment_id"
        },
        ...
      ]
    },
    ...
  ],
  "next_ts": "1.0"
}
```
