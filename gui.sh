#!/bin/bash

# Run the gui on some configuration
if [ $# -lt 2 ]; then
  echo "Usage: ./gui.sh <directory> <file_prefix>"
  exit 1
fi
$SUMO_HOME/bin/sumo-gui \
      -n $1/$2.net.xml \
      -r $1/$2.rou.xml \
      -a $1/$2.towers.xml
