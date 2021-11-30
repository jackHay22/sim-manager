package sim

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"os"
	"fmt"
)

/*
 * Load from a file into struct
 */
func load(path *string, v interface{}) {
	file, fileErr := os.Open(*path)
	if fileErr != nil {
		log.Fatalf("unable to open file %s: %v", *path, fileErr)
	}
	defer file.Close()

	//read the file
	fileBytes, readErr := ioutil.ReadAll(file)
	if readErr != nil {
		log.Fatalf("failed to read from file %s: %v", *path, readErr)
	}

	jsonErr := json.Unmarshal(fileBytes, v)
	if jsonErr != nil {
		log.Fatalf("failed to parse file %s: %v", *path, jsonErr)
	}
}

/*
 * For a given timestep and tower, get a list of all vehicles
 * connected to this tower by id
 */
func (s *SimInfo) VehiclesConnected(ts string, towerId string) (vids []string) {
	return vids
}

/*
 * For a given tower, get all segments that it
 * is responsible for
 */
func (s *SimInfo) TowerCoverage(towerId string) (sids []string, e error) {
	//TODO
	return sids, nil
}

/*
 * Load simulation data from files
 */
func LoadSimInfo(towerOutPath *string,
	vehicleOutPath *string,
	segmentOutPath *string) *SimInfo {

	var towerData towerOutput
	var vehicleData vehicleOutput
	var segmentData segmentOutput

	//load json
	load(towerOutPath, &towerData)
	load(vehicleOutPath, &vehicleData)
	load(segmentOutPath, &segmentData)

	//convert to more efficient lookup structures
	var simInfo SimInfo
	simInfo.towerCoverage.towers = make(map[string]map[string][]vehicleDist)

	for _, t := range towerData.Towers {
		//check if we already have a mapping for this tower
		if _, found := simInfo.towerCoverage.towers[t.TowerId]; !found {
			simInfo.towerCoverage.towers[t.TowerId] = make(map[string][]vehicleDist)
		}

		//all vehicles
		for _, v := range t.Vehicles {
			//convert the timestep to a string for use as a map key
			ts := fmt.Sprintf("%f", v.Ts)
			//check if we already have a mpaaing for this timestep
			if _, found := simInfo.towerCoverage.towers[t.TowerId][ts]; !found {
				simInfo.towerCoverage.towers[t.TowerId][ts] = make([]vehicleDist, len(v.V))
			}

			//vehicles at this timestep
			for _, data := range v.V {
				vid := int(data[0])

				simInfo.towerCoverage.towers[t.TowerId][ts] =
					append(simInfo.towerCoverage.towers[t.TowerId][ts], vehicleDist{
						vehicleId: towerData.VehicleIds[vid],
						dist: data[1],
				})
			}
		}
	}

	return &simInfo
}
