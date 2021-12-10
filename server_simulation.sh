#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: ./simulate_analyze.sh <output_directory> <towers_count>"
  exit 1
fi

make server -B || exit 1

echo "starting segment provider"
./server/segmentprovider \
  -tower-output $1/tower_output.json \
  -vehicle-output $1/vehicle_history_output.json \
  -segment-output $1/tower_coverage_output.json &

sleep 10

vehicle_server () {
  ./server/vehicleserver \
    -idx $1 \
    -peer-count $2 \
    -port-range-start 9000 \
    -sprov 127.0.0.1:8080
}

MAX_TOWER_IDX="$(($2-1))"

for ((i=0; i<=$MAX_TOWER_IDX; i++))
do
  if [ $i != $MAX_TOWER_IDX ]
  then
    vehicle_server $i $MAX_TOWER_IDX &
  else
    vehicle_server $i $MAX_TOWER_IDX
  fi
done
