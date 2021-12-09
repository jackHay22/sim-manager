#!/bin/bash

TOWERS_THRESHOLD=100
GRID_JUNCTIONS=10

if [ $# -lt 2 ]; then
  echo "Usage: ./grid.sh <output_directory> <towers_count>"
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

  #create random trips within the network (6 minutes)
  python3 $SUMO_HOME/tools/randomTrips.py \
        -n $NETWORK_PATH \
        -r $ROUTES_PATH \
        -e 360 \
        -p 1.0

  rm $ROUTES_ALT_PATH || true
  rm trips.trips.xml || true

  #enable bluetooth on all cars in route file
  python3 generation/bt_enable.py \
        --routes $ROUTES_PATH

  #Run the tower placer on this network
  python3 generation/tower_placer.py \
        --net $NETWORK_PATH \
        --out $TOWERS_PATH \
        --count $2 \
        --threshold $TOWERS_THRESHOLD
}

if [ -d $1 ]
then
    generate $1 $2
else
    echo "directory $1 does not exist"
    exit 1
fi
