.PHONY: setup build gui-example data-example

setup:
	$(MAKE) -C simulation_manager libs
build:
	$(MAKE) -C simulation_manager
	# Builds simulation_manager/simulation_manager.o
simulate-example: build
	LD_LIBRARY_PATH=$(SUMO_HOME)/bin ./simulation_manager/simulation_manager.o -c examples/example_simulation_manager_cfg.json
gui-example:
	$(SUMO_HOME)/bin/sumo-gui -n examples/example.net.xml -r examples/example.rou.xml -a examples/example.servers.xml
data-example:
	$(SUMO_HOME)/bin/netgenerate --rand --rand.iterations=200 --output-file=examples/example.net.xml
	python $(SUMO_HOME)/tools/randomTrips.py \
	      -n examples/example.net.xml \
	      -r examples/example.rou.xml \
	      -e 3600 -p 1.0 #defaults
	rm trips.trips.xml || true
	rm examples/example.rou.alt.xml || true
