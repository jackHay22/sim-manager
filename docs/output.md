# Output formats

## Tower Output (tower communications)
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
      - first element: 0-index in vehicle id list
      - second element: distance from tower

## Vehicle Output (vehicle route history)
- Format: `json`

```json
{
  "segments" : ["s0", "s1", ...],
  "vehicles" : [
    {
      "vehicle_id" : "0",
      "segments" : [
        {"ts" : 0.0, "s" : [[0, 0], [1, 100], [2, 10], ...]},
        {"ts" : 1.0, "s" : [[0, 1], [1, 101], [2, 11], ...]},
        ...
      ]
    },
    {
      "vehicle_id" : "1",
      "segments" : [
        {"ts" : 0.0, "s" : [[6, 0], [9, 22], [13, 14], ...]},
        {"ts" : 1.0, "s" : [[6, 1], [9, 23], [13, 15], ...]},
        ...
      ]
    },
    ...
  ]
}
```
- `segments` : the uniquer identifiers for each segment in the vehicle `segments` arrays (same as segments in tower coverage output)
- `vehicles` : positions of vehicles relative to segments by timestep (ex: vehicle 1 was at segment s0 10 timesteps ago, segment s1 15 timesteps ago, and segment s43 35 timesteps ago):
  - `vehicle_id` : the unique identifier for the vehicle
  - `segments` : a list of segments the vehicle has passed by timestep:
    - `ts` : the timestep
    - `s` : visited segments in the map (if segment is not in list it has not been visited by the vehicle)
      - first element: 0-index into `segments` list
      - second element: how long ago in timesteps this vehicle passed this segment

## Segment Output (tower coverage)
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
  - `segments` : The distance to each segment from the tower (from the closest point in the segment). Index in list corresponds to segment identifier in `segments` at the same position
