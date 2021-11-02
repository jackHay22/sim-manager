package sim

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"os"
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
 * Load simulation data from files
 */
func LoadSimInfo(towerOutPath *string,
	vehicleOutPath *string,
	segmentOutPath *string) *SimInfo {
	var simInfo SimInfo
	load(towerOutPath, &simInfo.towerData)
	load(vehicleOutPath, &simInfo.vehicleData)
	load(segmentOutPath, &simInfo.segmentData)
	return &simInfo
}
