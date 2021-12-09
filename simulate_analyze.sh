#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: ./simulate_analyze.sh <input_directory> <output_directory>"
  exit 1
fi

FILE_PREFIX=grid

#Generate a new simulation network into the input directory (grid)
echo "./generation/grid.sh ${1}"
./generation/grid.sh $1

echo "$SUMO_HOME/bin/sumo "
echo "   -n $1/${FILE_PREFIX}.net.xml "
echo "   -r $1/${FILE_PREFIX}.rou.xml "
echo "   -a $1/${FILE_PREFIX}.towers.xml "
echo "   --bt-output $2/${FILE_PREFIX}.bt_output.xml "
echo "   --netstate-dump $2/${FILE_PREFIX}.ns_output.xml"
echo "   --device.btreceiver.range 100.0 "
echo "   --device.btreceiver.all-recognitions"
#Run the simulation
$SUMO_HOME/bin/sumo \
      -n $1/${FILE_PREFIX}.net.xml \
      -r $1/${FILE_PREFIX}.rou.xml \
      -a $1/${FILE_PREFIX}.towers.xml \
      --bt-output $2/${FILE_PREFIX}.bt_output.xml \
      --netstate-dump $2/${FILE_PREFIX}.ns_output.xml \
      --device.btreceiver.range 100.0 \
      --device.btreceiver.all-recognitions

OUT_DIR=${2}/${3}/
# Run the output transformer
echo "mkdir -p $OUT_DIR"
mkdir -p $OUT_DIR
echo "./analysis/analysis_transformer.o "
echo "   -b $2/${FILE_PREFIX}.bt_output.xml "
echo "   -r $2/${FILE_PREFIX}.ns_output.xml"
echo "   -n $1/${FILE_PREFIX}.net.xml "
echo "   -o $OUT_DIR"
./analysis/analysis_transformer.o \
    -b $2/${FILE_PREFIX}.bt_output.xml \
    -r $2/${FILE_PREFIX}.ns_output.xml \
    -n $1/${FILE_PREFIX}.net.xml \
    -o $OUT_DIR

#Run server simulations
./server_simulation.sh $2
