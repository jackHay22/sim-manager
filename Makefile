.PHONY: simulate-example gui-example edit-example setup

analysis:
	$(MAKE) -C analysis clean
	$(MAKE) -C analysis
simulate-example:
	./simulation/simulation.sh \
			data/organic \
			output_data \
			example
gui-example:
	./gui.sh data/organic example
edit-example:
	$(SUMO_HOME)/bin/netedit \
		-s data/organic/example.net.xml \
		-a data/organic/example.towers.xml
