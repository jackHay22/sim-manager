#!/bin/bash

TOWERS_COUNT=20
TOWERS_THRESHOLD=100

if [ $# -eq 0 ]
then
  echo "No output directory specified"
  exit 1
fi

#Generate
generate () {
  NETWORK_PATH="$1/organic.net.xml"
  ROUTES_PATH="$1/organic.rou.xml"
  ROUTES_ALT_PATH="$1/organic.rou.alt.xml"
  TOWERS_PATH="$1/organic.towers.xml"

  #generate the random network
  $SUMO_HOME/bin/netgenerate \
        --rand \
        --rand.iterations=200 \
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
  python3 bt_enable.py --routes $ROUTES_PATH

  #Run the tower placer on this network
  python3 tower_placer.py \
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
