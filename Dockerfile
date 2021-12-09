FROM ubuntu:bionic

USER root

WORKDIR /
ENV SUMO_HOME=/sumo

RUN apt-get update -y --fix-missing && apt-get -y upgrade
RUN apt-get install -y wget valgrind make git cmake python3 g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev
RUN wget https://golang.org/dl/go1.16.5.linux-amd64.tar.gz && \
    rm -rf /usr/local/go && tar -C /usr/local -xzf go1.16.5.linux-amd64.tar.gz
RUN git clone --recursive https://github.com/eclipse/sumo
RUN mkdir /sumo/build/cmake-build
WORKDIR /sumo/build/cmake-build
RUN cmake ../..
RUN make -j$(nproc)

WORKDIR /
COPY generation/ generation/
COPY analysis/ analysis/
COPY server/ server/
COPY Makefile Makefile
COPY simulate_analyze.sh simulate_analyze.sh
COPY server_simulation.sh server_simulation.sh
RUN make analysis -B -j$(nproc)
RUN make server -B
CMD [ "./simulate_analyze.sh", "/data/grid", "/output_data" ]
