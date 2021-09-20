.PHONY: simulate-example gui-example edit-example setup

setup:
	$(MAKE) -C pipeline libs

simulate-example:
	./simulation/simulation.sh \
			examples \
			output_data \
			example
gui-example:
	./gui.sh examples example
edit-example:
	$(SUMO_HOME)/bin/netedit \
		-s examples/example.net.xml \
		-a examples/example.towers.xml
