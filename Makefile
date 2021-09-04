.PHONY: setup build gui-example

setup:
	$(MAKE) -C simulation_manager libs
build:
	$(MAKE) -C simulation_manager
	# Builds simulation_manager/simulation_manager.o
simulate-example: build
	LD_LIBRARY_PATH=$(SUMO_HOME)/bin ./simulation_manager/simulation_manager.o -c examples/example_simulation_manager_cfg.json
gui-example:
	$(SUMO_HOME)/bin/sumo-gui -n examples/example.net.xml -r examples/example.rou.xml -a examples/example.servers.xml
