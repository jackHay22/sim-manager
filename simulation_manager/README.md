# Simulation Manager
- Takes a network, and a simulation configuration, manages the simulation and generates the output for analysis

## Running the simulation manager

```bash
LD_LIBRARY_PATH=$SUMO_HOME/bin ./simulation_manager.o
```

## Options
```bash
-c <cfg_path>   #(default: "simulation_cfg.json") -- The configuration file
-o <output_dir> #(default: current directory) -- The directory to write the output to
```

## Configuration
- Default values for `default_network_path`, `default_routes_path`, and `default_towers_path` must be provided
- They can be omitted in simulation objects (and defaults will be used)
- If omitted, `simulation_steps` and `step_length` take on default values
```json
{
  "default_network_path" : "path/to/network.net.xml",
  "default_routes_path" : "path/to/routes.rou.xml",
  "default_towers_path" : "path/to/towers.towers.xml",
  "simulations" : [
    {
      "network_path" : "path/to/network.net.xml",
      "routes_path" : "path/to/routes.rou.xml",
      "towers_path" : "path/to/towers.towers.xml",
      "simulation_steps" : 1000,
      "step_length" : 0.1
    },
    {
      "towers_path" : "path/to/towers2.towers.xml",
      "step_length" : 0.1
    }
  ]
}
```
- `network_path` : Path to the network file to simulate on (required)
- `routes_path` : Path to the routes file to simulate on (required)
- `servers_path` : Path to the servers file to simulate on (required)
- `simulation_steps` : Number of steps to simulate (optional)
  - Default: 1000
- `step_length` : the duration of a step (optional) -- range is beteen 0.001 and 1.0
  - Default: `0.1` (100ms)
