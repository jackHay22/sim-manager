#!/bin/bash

echo "starting segment provider"
./server/segmentprovider \
  -tower-output - \
  -vehicle-output - \
  -segment-output - &

sleep 2

#Start servers for all towers
for i in {1..81}
do
  ./server/vehicleserver \
    -idx $i \
    -peer-count 80 \
    -port-range-start 9000 \
    -sprov 127.0.0.1:8080 \
    -proc-constraint 0 \
    -store-constraint 0 \
    -band-constraint 0
done
