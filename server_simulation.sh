#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: ./simulate_analyze.sh <output_directory>"
  exit 1
fi

make server -B || exit 1

echo "starting segment provider"
./server/segmentprovider \
  -tower-output $1/tower_output.json \
  -vehicle-output $1/vehicle_history_output.json \
  -segment-output $1/tower_coverage_output.json &

sleep 10

#Start servers for all towers
for i in {0..80}
do
  ./server/vehicleserver \
    -idx $i \
    -peer-count 80 \
    -port-range-start 9000 \
    -sprov 127.0.0.1:8080 \
    -proc-constraint 0 \
    -store-constraint 0 \
    -band-constraint 0 &
done
