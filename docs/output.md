# Output formats

## Tower Output
- Format: `json`

```json
[
  {
    "tower_id" : "tower_parkingArea_A1A0_0_81",
    "vehicles" : [
      {"ts" : 0.0, "v" : [-1, -1, -1, 6, -1, -1, 8, ...]},
      {"ts" : 1.0, "v" : [-1, 20, -1, 9, -1, -1, 7, ...]}
      ...
    ]
  },
  {
    "tower_id" : "tower0_parkingArea_A2A1_0_71",
    "vehicles" : [
      {"ts" : 0.0, "v" : [-1, -1, -1, -1, -1, -1, 100, ...]},
      {"ts" : 1.0, "v" : [-1, -1, -1, -1, -1, -1, 99, ...]}
      ...
    ]
  },
  ...
]
```
- `tower_id` : unique identifier for the tower
- `vehicles` : a list of connections:
  - `ts` : the current timestep
  - `v` : a list of all vehicles.
    - -1 : The vehicle is not connected to the tower
    - non-negative: the distance from the vehicle to the tower

## Vehicle Output
- Format: `json`

```json
[
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
      {"ts" : 1.0, "s" : [0, -1, -1, ...]},
      ...
    ]
  },
  ...
]
```
- `vehicle_id` : the unique identifier for the vehicle
- `segments` : a list of segments the vehicle has passed by timestep:
  - `ts` : the timestep
  - `s` : all segments in the map
    - -1 : The vehicle has not visited this segment
    - non-negative : how long ago in timsteps this vehicle passed this segment

## Segment Output
- Format: `json`

```json
[
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
```
- `tower_id` : The unique identifier for this tower
- `segments` : The zero-indices of segments (relative to the "Vehicle Output" arrays) that the tower is in range of
