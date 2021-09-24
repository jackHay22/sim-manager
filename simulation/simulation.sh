#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: ./simulation.sh <input_directory> <output_directory> <file_prefix>"
  exit 1
fi

#Run the simulation
$SUMO_HOME/bin/sumo \
      -n $1/$3.net.xml \
      -r $1/$3.rou.xml \
      -a $1/$3.towers.xml \
      --bt-output $2/$3.bt_output.xml \
      --fcd-output $2/$3.fcd.output.xml \
      --device.btreceiver.range 1000.0 \
      --device.btreceiver.all-recognitions
