#!/bin/bash

make server -B || exit 1

echo "starting segment provider"
./server/segmentprovider \
  -tower-output output_data/tower_output.json \
  -vehicle-output output_data/vehicle_history_output.json \
  -segment-output output_data/tower_coverage_output.json &

sleep 10

./server/vehicleserver \
    -idx 0 \
    -peer-count 80 \
    -port-range-start 9000 \
    -sprov 127.0.0.1:8080 \
    -proc-constraint 0 \
    -store-constraint 0 \
    -band-constraint 0

#
# #Start servers for all towers
# for i in {1..81}
# do
#   ./server/vehicleserver \
#     -idx $i \
#     -peer-count 80 \
#     -port-range-start 9000 \
#     -sprov 127.0.0.1:8080 \
#     -proc-constraint 0 \
#     -store-constraint 0 \
#     -band-constraint 0
# done
