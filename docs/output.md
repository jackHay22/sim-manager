# Output formats

## Tower Output
- Format: `json`

```json
{
  "vehicle_ids" : ["0", "1", ...],
  "towers" : [
    {
      "tower_id" : "tower_parkingArea_A1A0_0_81",
      "vehicles" : [
        {"ts" : 0.0, "v" : [[1, 20], [2, 3], [23, 12], ...]},
        {"ts" : 1.0, "v" : [[1, 19], [2, 5], [23, 11], ...]}
        ...
      ]
    },
    {
      "tower_id" : "tower0_parkingArea_A2A1_0_71",
      "vehicles" : [
        {"ts" : 0.0, "v" : [[1, 0], [2, 100], [7, 10], ...]},
        {"ts" : 1.0, "v" : [[1, 0], [47, 100], [7, 11], ...]}
        ...
      ]
    },
    ...
  ]
}

```
- `vehicle_ids` : the ids of vehicles corresponding to the `v` array for each tower
- `towers` : connections for each tower:
  - `tower_id` : unique identifier for the tower
  - `vehicles` : a list of connections:
    - `ts` : the current timestep
    - `v` : a list of vehicles in range.
      - first element: index in vehicle id list
      - second element: distance from tower

## Vehicle Output
- Format: `json`

```json
{
  "segments" : ["s0", "s1", ...],
  "vehicles" : [
    {
      "vehicle_id" : "0",
      "segments" : [
        {"ts" : 0.0, "s" : [-1, -1, -1, ...]},
        {"ts" : 1.0, "s" : [-1, 0, -1, ...]},
        ...
      ]
    },
    {
      "vehicle_id" : "1",
      "segments" : [
        {"ts" : 0.0, "s" : [-1, -1, -1, ...]},
        {"ts" : 1.0, "s" : [0, -1, -1, ...]}, TODO we may need to make this more sparse
        ...
      ]
    },
    ...
  ]
}
```
- `segments` : the uniquer identifiers for each segment in the vehicle `segments` arrays
- `vehicles` : positions of vehicles relative to segments by timestep:
  - `vehicle_id` : the unique identifier for the vehicle
  - `segments` : a list of segments the vehicle has passed by timestep:
    - `ts` : the timestep
    - `s` : all segments in the map
      - -1 : The vehicle has not visited this segment
      - non-negative : how long ago in timesteps this vehicle passed this segment

## Segment Output
- Format: `json`

```json
{
  "segments" : ["s0", "s1", ...],
  "towers" : [
    {
      "tower_id" : "tower_parkingArea_A1A0_0_81",
      "segments" : [0, 23, 6, 2, 87, ...]
    },
    {
      "tower_id" : "tower0_parkingArea_A2A1_0_71",
      "segments" : [0, 1, 4, 3, 97, ...]
    },
    ...
  ]
}
```
- `segments` : the unique identifiers for segments in `segments` arrays
- `towers` : towers:
  - `tower_id` : The unique identifier for this tower
  - `segments` : The zero-indices of segments (relative to the `segments`) that the tower is in range of
