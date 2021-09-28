#!/bin/bash

docker build -t sim_manager:latest .
docker run --volume ${PWD}/data/:/data \
           --volume ${PWD}/output_data/:/output_data sim_manager:latest
