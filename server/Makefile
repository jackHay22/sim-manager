.PHONY: all vehicleserver segmentprovider get
all: vehicleserver segmentprovider 
vehicleserver:
	cd server && go build . && cd .. && mv server/vehicleserver .
segmentprovider:
	cd segment_provider && go build . && cd .. && mv segment_provider/segmentprovider .
get:
	cd server && go get . && cd ..
	cd segment_provider && go get . && cd ..
clean::
	rm vehicleserver
	rm segmentprovider
