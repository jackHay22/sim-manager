# Sumo Simulation Manager
- Automates the creation and management of Sumo simulations for vehicle communications optimization.

## Analysis transformer
- Requires (both can be installed as header only libraries in `analysis/libs/`)
  - [RapidXML](https://sourceforge.net/projects/rapidxml/)
  - [nlohmann json](https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp)


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
