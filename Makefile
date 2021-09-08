.PHONY: setup build gui-example data-example towers-example

setup:
	$(MAKE) -C simulation_manager libs
build:
	$(MAKE) -C simulation_manager
	# Builds simulation_manager/simulation_manager.o
simulate-example: build
	LD_LIBRARY_PATH=$(SUMO_HOME)/bin ./simulation_manager/simulation_manager.o -c examples/example_simulation_manager_cfg.json
gui-example:
	./gui.sh examples example
