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
 * Get the vehicle segment history up to the given timestep
 */
func (s *SimInfo) getVehicleHist(vehicleId string, ts string) (h []vehicleHist, e error) {
	if v, foundV := s.vehicleHist.vehicles[vehicleId]; foundV {
		if segments, foundT := v[ts]; foundT {
			for _, s := range segments {
				h = append(h, vehicleHist{
					Elapsed: s.tsAgo,
					SegmentId: s.segmentId,
				})
			}
			return h, nil
		}
	}
	return nil, errors.New(fmt.Sprintf("no history for vehicle %s at timestep %s", vehicleId, ts))
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
					if hist, err := s.getVehicleHist(v.vehicleId, ts); err == nil {
						//add the connected vehicle
						cov.Vehicles = append(cov.Vehicles, vehicle{
							VehicleId: v.vehicleId,
							Distance: v.dist,
							VehicleHistory: hist,
						})
					} else {
						return nil, err
					}
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
func (s *SimInfo) TowerCoverage(towerId string) ([]string, error) {
	//look for the tower
	if t, found := s.towerAssignments.towers[towerId]; found {
		return t, nil
	} else {
		return nil, errors.New(fmt.Sprintf("no such tower: %s", towerId))
	}
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
	simInfo.vehicleHist.vehicles = make(map[string]map[string][]segmentPos)
	simInfo.towerAssignments.towers = make(map[string][]string)
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

			//check if we already have a maping for this timestep
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

	for _, v := range vehicleData.Vehicles {
		//check if we already have a mapping for this vehicle
		if _, found := simInfo.vehicleHist.vehicles[v.VehicleId]; !found {
			simInfo.vehicleHist.vehicles[v.VehicleId] = make(map[string][]segmentPos)
		}

		//get segments
		for _, s := range v.Segments {
			ts := fmt.Sprintf("%f", s.Ts)

			//check if we already have a maping for this timestep
			if _, found := simInfo.vehicleHist.vehicles[v.VehicleId][ts]; !found {
				simInfo.vehicleHist.vehicles[v.VehicleId][ts] = make([]segmentPos, len(s.S))
			}

			//segments at this timestep
			for _, data := range s.S {
				sid := int(data[0])

				simInfo.vehicleHist.vehicles[v.VehicleId][ts] =
					append(simInfo.vehicleHist.vehicles[v.VehicleId][ts], segmentPos{
						segmentId: vehicleData.Segments[sid],
						tsAgo: data[1],
				})
			}
		}
	}

	//map segment id to the closest tower so far
	bestTower := make([]struct {
		//best tower so far
		t string
		//the distance to that tower
		d float64  `default:"1000000"`
	}, len(segmentData.Segments))

	//based on tower distances, determine segments that a tower is responsible for
	for _, t := range segmentData.Towers {
		for i, s := range t.Segments {
			if s < bestTower[i].d {
				//set as the best tower so far
				bestTower[i].t = t.TowerId
				bestTower[i].d = s
			}
		}
	}

	//make assignments based on closest tower
	for i, t := range bestTower {
		simInfo.towerAssignments.towers[t.t] =
			append(simInfo.towerAssignments.towers[t.t],
						 segmentData.Segments[i])
	}

	return &simInfo
}
