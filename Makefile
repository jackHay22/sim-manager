.PHONY: simulate-example gui-example edit-example all
all: analysis server
analysis:
	$(MAKE) -C analysis clean
	$(MAKE) -C analysis
server-get:
	$(MAKE) -C server get
server:
	$(MAKE) -C server
simulate-example:
	./simulation/simulation.sh \
			data/example_organic \
			output_data \
			example
gui-example:
	./gui.sh data/example_organic example
edit-example:
	$(SUMO_HOME)/bin/netedit \
		-s data/example_organic/example.net.xml \
		-a data/example_organic/example.towers.xml
