#!/bin/bash

TOWERS_COUNT=81
TOWERS_THRESHOLD=100
GRID_JUNCTIONS=10

if [ $# -eq 0 ]
then
  echo "No output directory specified"
  exit 1
fi

#Generate
generate () {
  NETWORK_PATH="$1/grid.net.xml"
  ROUTES_PATH="$1/grid.rou.xml"
  ROUTES_ALT_PATH="$1/grid.rou.alt.xml"
  TOWERS_PATH="$1/grid.towers.xml"

  #Use the net generate tool
  $SUMO_HOME/bin/netgenerate \
        --grid \
        --grid.number $GRID_JUNCTIONS \
        --output-file=$NETWORK_PATH

  #create random trips within the network
  python $SUMO_HOME/tools/randomTrips.py \
        -n $NETWORK_PATH \
        -r $ROUTES_PATH \
        -e 3600 -p 1.0 #defaults

  rm $ROUTES_ALT_PATH || true
  rm trips.trips.xml || true

  #enable bluetooth on all cars in route file
  python bt_enable.py --routes $ROUTES_PATH

  #Run the tower placer on this network
  python tower_placer.py \
        --net $NETWORK_PATH \
        --out $TOWERS_PATH \
        --count $TOWERS_COUNT \
        --threshold $TOWERS_THRESHOLD
}

if [ -d $1 ]
then
    generate $1
else
    echo "directory $1 does not exist"
    exit 1
fi
