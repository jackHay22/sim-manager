# Sumo Simulation Manager
- Automates the creation and management of Sumo simulations for vehicle communications optimization.

## Setup
- Use `vagrant` to run in a vm
  - Run: `vagrant up && vagrant ssh`
  - In the vm: `cd /vagrant/simulation_manager && export SUMO_HOME=/home/vagrant/sumo && make libs && make -j4`

## Use

### Simulation Manager
[Simulation Manager](simulation_manager/README.md)

- Run example simulation
```bash
make simulate-example
```

- Open example simulation in sumo-gui
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
      -e 500 -p 0.01
```
