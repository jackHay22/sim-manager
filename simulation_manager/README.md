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
```json
{
  "network_path" : "path/to/network.net.xml",
  "routes_path" : "path/to/routes.rou.xml",
  "servers_path" : "path/to/servers.servers.xml",
  "simulation_steps" : 1000,
  "step_length" : 0.1
}
```
- `network_path` : Path to the network file to simulate on (required)
- `routes_path` : Path to the routes file to simulate on (required)
- `servers_path` : Path to the servers file to simulate on (required)
- `simulation_steps` : Number of steps to simulate (optional)
  - Default: 1000
- `step_length` : the duration of a step (optional) -- range is beteen 0.001 and 1.0
  - Default: `0.1` (100ms)
