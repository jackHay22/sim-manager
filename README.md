# Sumo Simulation Manager
- Automates the creation and management of Sumo simulations for vehicle communications optimization.
- Runs server computation simulation based on Sumo simulation output

## Analysis transformer
- `make analysis`
- Requires (both can be installed as header only libraries in `analysis/libs/`)
  - [RapidXML](https://sourceforge.net/projects/rapidxml/)
  - [nlohmann json](https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp)
- `analysis/libs/` should contain the following files:
```
json.hpp
rapidxml.hpp
rapidxml_iterators.hpp
rapidxml_print.hpp
rapidxml_utils.hpp
```

## Server simulation
- Creates a "segment provider" that distributes simulation information to different towers
- Each tower is a separate process
- `make server`
- See: [API spec](docs/api.md)
```
./server_simulation.sh output_data 81
```

### Greedy Algorithm
- The greedy algorithm is implemented in [server/python/impl.py](server/python/impl.py)
- At each timestep, the python main() function receives the json structure defined [here](docs/api.md) containing the vehicles in range of the tower.
- The server decides which information to download. Of this data, it forwards some to peers and processes the rest. It can also process data that has been forwarded by a peer.

## Roadside towers
- Roadside towers are represented by parked cars:
![Screenshot](docs/images/example_server.png)

## Setup
- Use `vagrant` to run in a vm
  - Run: `vagrant up && vagrant ssh`
  - In the vm: `cd /vagrant && export SUMO_HOME=/home/vagrant/sumo`

## Use

### Simulation pipeline

## Docker
- To run the simulation and post processing pipeline in docker:
```bash
./run_docker.sh
```

## Host
- To run the simulation and post processing pipeline:
```bash
./simulate_analyze.sh data/grid output_data grid
```
- In this case `data/grid` is the input file location, `output_data` is the folder to write output to, and `grid` is the input file prefix

### Simulation Manager
- Run example simulation
```bash
make simulate-example
```

- Open example simulation in sumo-gui (can't run in vagrant)
```bash
make gui-example
```

### Creating random networks and vehicles
- Random network
```bash
$SUMO_HOME/bin/netgenerate --rand --rand.iterations=200 --output-file=example.net.xml
```

- Random vehicles and routes
```bash
python $SUMO_HOME/tools/randomTrips.py \
      -n examples/example.net.xml \
      -o examples/example.trip.xml \
      -r examples/example.rou.xml \
      -e 3600 -p 1.0 #defaults
```
