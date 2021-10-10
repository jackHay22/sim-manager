#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: ./simulate_analyze.sh <input_directory> <output_directory> <file_prefix>"
  exit 1
fi

echo "$SUMO_HOME/bin/sumo "
echo "   -n $1/$3.net.xml "
echo "   -r $1/$3.rou.xml "
echo "   -a $1/$3.towers.xml "
echo "   --bt-output $2/${3}.bt_output.xml "
echo "   --netstate-dump $2/${3}.ns_output.xml"
echo "   --device.btreceiver.range 1000.0 "
echo "   --device.btreceiver.all-recognitions"
#Run the simulation
$SUMO_HOME/bin/sumo \
      -n $1/$3.net.xml \
      -r $1/$3.rou.xml \
      -a $1/$3.towers.xml \
      --bt-output $2/${3}.bt_output.xml \
      --netstate-dump $2/${3}.ns_output.xml \
      --device.btreceiver.range 1000.0 \
      --device.btreceiver.all-recognitions

OUT_DIR=${2}/${3}/
# Run the output transformer
echo "mkdir -p $OUT_DIR"
mkdir -p $OUT_DIR
echo "./analysis/analysis_transformer.o "
echo "   -b $2/${3}.bt_output.xml "
echo "   -r $2/${3}.ns_output.xml"
echo "   -n $1/${3}.net.xml "
echo "   -o $OUT_DIR"
./analysis/analysis_transformer.o \
    -b $2/${3}.bt_output.xml \
    -r $2/${3}.ns_output.xml \
    -n $1/${3}.net.xml \
    -o $OUT_DIR
