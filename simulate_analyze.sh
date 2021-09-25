#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: ./simulate_analyze.sh <input_directory> <output_directory> <file_prefix>"
  exit 1
fi

# Run the simulation
./simulation/simulation.sh $1 $2 $3

# Run the output transformer
./analysis/analysis_transformer.o -b $2/$3.bt_output.xml -o $2/$3.output.csv
