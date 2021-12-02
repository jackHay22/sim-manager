package sim

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"os"
	"fmt"
	"sync"
	"errors"
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
 *
 * Note: this will block until all servers are ready to start the next
 * timestep
 */
func (s *SimInfo) VehiclesConnected(ts string, towerId string) (*vehicleCoverage, error) {
	//check if the tower is requesting info for the current timestep
	if ts == s.allTs[s.currentTs] {
		//serve information for the current timestep
		if t, found := s.towerCoverage.towers[towerId]; found {
			//look for the requested timestep
			if vs, vsFound := t[ts]; vsFound {
				var cov vehicleCoverage
				//add the next timestep
				if s.currentTs < len(s.allTs) - 1 {
					cov.NextTs = s.allTs[s.currentTs + 1]
				} //otherwise empty, causes tower to stop

				//generate a response with the connected vehicles
				for _, v := range vs {
					cov.Vehicles = append(cov.Vehicles, vehicle{
						
					})
				}
				return &cov, nil

			} else {
				//failed to locate the tower
				return nil, errors.New(fmt.Sprintf("failed to find timestep %s for tower %s", ts, towerId))
			}

		} else {
			//failed to locate the tower
			return nil, errors.New(fmt.Sprintf("failed to find tower entry for %s", towerId))
		}

	} else {
		s.mutex.Lock()
		//tower waiting for a future timestep
		s.towersWaiting++

		if s.towersWaiting == s.towers {
			log.Printf("timestep %s complete", s.currentTs)

			//this tower was the last one, wake the others, continue
			s.towersWaiting = 0
			s.currentTs++
			//release the lock on siminfo
			s.mutex.Unlock()

			//wake waiting towers
			s.cond.Signal()

			//try again
			return s.VehiclesConnected(ts, towerId)

		} else {
			//release the lock on Siminfo and wait for the current ts to complete
			s.mutex.Unlock()

			s.cond.L.Lock()
			s.cond.Wait()
			s.cond.L.Unlock()

			//try again
			return s.VehiclesConnected(ts, towerId)
		}
	}
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
	simInfo.towers = 0
	simInfo.towersWaiting = 0
	simInfo.currentTs = 0
	simInfo.cond.L = new(sync.Mutex)

	for i, t := range towerData.Towers {
		//check if we already have a mapping for this tower
		if _, found := simInfo.towerCoverage.towers[t.TowerId]; !found {
			simInfo.towerCoverage.towers[t.TowerId] = make(map[string][]vehicleDist)
			simInfo.towers++
		}

		//all vehicles
		for _, v := range t.Vehicles {
			//convert the timestep to a string for use as a map key
			ts := fmt.Sprintf("%f", v.Ts)
			if i == 0 {
				//add the timestep to the full list
				simInfo.allTs = append(simInfo.allTs, ts)
			}

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
