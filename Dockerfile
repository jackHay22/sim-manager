FROM ubuntu:bionic

USER root

WORKDIR /
ENV SUMO_HOME=/sumo

RUN apt-get update && apt-get -y upgrade
RUN apt-get install -y valgrind make git cmake python3 g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev
RUN git clone --recursive https://github.com/eclipse/sumo
RUN mkdir /sumo/build/cmake-build
WORKDIR /sumo/build/cmake-build
RUN cmake ../..
RUN make -j$(nproc)

WORKDIR /
COPY simulation_manager/ simulation_manager/
COPY Makefile Makefile
RUN make build
