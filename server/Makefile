GO = /usr/local/go/bin/go

.PHONY: all vehicleserver segmentprovider get

all: vehicleserver segmentprovider
vehicleserver:
	cd server && $(GO) build . && cd .. && mv server/vehicleserver .
segmentprovider:
	cd segment_provider && $(GO) build . && cd .. && mv segment_provider/segmentprovider .
get:
	cd server && $(GO) get . && cd ..
	cd segment_provider && $(GO) get . && cd ..
clean::
	rm vehicleserver
	rm segmentprovider
