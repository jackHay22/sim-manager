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
func (s *SimInfo) getVehicleHist(vehicleId string, ts int) (h []vehicleHist, e error) {
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
	return nil, errors.New(fmt.Sprintf("no history for vehicle %s at timestep %d", vehicleId, ts))
}

/*
 * For a given timestep and tower, get a list of all vehicles
 * connected to this tower by id
 *
 * Note: this will block until all servers are ready to start the next
 * timestep
 */
func (s *SimInfo) VehiclesConnected(ts int, towerId string) (*vehicleCoverage, error) {
	if ts > s.maxTs {
		return nil, errors.New(fmt.Sprintf("timestep %d out of range", ts))
	}

	//check if the tower is requesting info for the current timestep
	if ts == s.currentTs {
		//serve information for the current timestep
		if t, found := s.towerCoverage.towers[towerId]; found {
			var cov vehicleCoverage
			//the max timestep (inclusive)
			cov.MaxTs = s.maxTs
			//look for the requested timestep
			if vs, vsFound := t[ts]; vsFound {
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
				//no vehicles in range at this ts
				return &cov, nil
			}

		} else {
			//failed to locate the tower
			return nil, errors.New(fmt.Sprintf("failed to find tower entry for %s", towerId))
		}

	} else {
		//lock
		s.cond.L.Lock()
		//update waiting count
		s.towersWaiting++

		if s.towersWaiting == s.towers {
			log.Printf("timestep %d complete", s.currentTs)
			//this tower was the last one, wake the others, continue
			s.towersWaiting = 0
			s.currentTs++
			s.cond.L.Unlock()

			//wake waiting towers
			s.cond.Broadcast()
			//try again
			return s.VehiclesConnected(ts, towerId)

		} else {
			//wait for all towers to complete, then continue
			for s.currentTs < ts {
				s.cond.Wait()
			}
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
		log.Printf("warning, no segments found for tower %s", towerId)
		var empty []string
		return empty, nil
	}
}

/*
 * Called when a tower is done processing
 */
func (s *SimInfo) TowersComplete(count int) {
	if count == s.towers {
		log.Fatalf("all towers completed simulation")
	}
}

/*
 * Load simulation data from files
 */
func LoadSimInfo(towerOutPath *string,
	vehicleOutPath *string,
	segmentOutPath *string,
	towersOverride int) *SimInfo {

	var towerData towerOutput
	var vehicleData vehicleOutput
	var segmentData segmentOutput

	//load json
	load(towerOutPath, &towerData)
	load(vehicleOutPath, &vehicleData)
	load(segmentOutPath, &segmentData)

	//convert to more efficient lookup structures
	var simInfo SimInfo
	simInfo.towerCoverage.towers = make(map[string]map[int][]vehicleDist)
	simInfo.vehicleHist.vehicles = make(map[string]map[int][]segmentPos)
	simInfo.towerAssignments.towers = make(map[string][]string)
	simInfo.towers = 0
	simInfo.towersWaiting = 0
	simInfo.currentTs = 1
	simInfo.maxTs = 0
	simInfo.cond = sync.NewCond(&simInfo.cMutex)

	for _, t := range towerData.Towers {
		//check if we already have a mapping for this tower
		if _, found := simInfo.towerCoverage.towers[t.TowerId]; !found {
			simInfo.towerCoverage.towers[t.TowerId] = make(map[int][]vehicleDist)
			simInfo.towers++
		}

		//all vehicles
		for _, v := range t.Vehicles {
			//check if we already have a maping for this timestep
			if _, found := simInfo.towerCoverage.towers[t.TowerId][v.Ts]; !found {
				simInfo.towerCoverage.towers[t.TowerId][v.Ts] = make([]vehicleDist, 0, len(v.V))
			}

			//vehicles at this timestep
			for _, data := range v.V {
				vid := int(data[0])

				simInfo.towerCoverage.towers[t.TowerId][v.Ts] =
					append(simInfo.towerCoverage.towers[t.TowerId][v.Ts], vehicleDist{
						vehicleId: towerData.VehicleIds[vid],
						dist: data[1],
				})
			}
		}
	}

	for _, v := range vehicleData.Vehicles {
		//check if we already have a mapping for this vehicle
		if _, found := simInfo.vehicleHist.vehicles[v.VehicleId]; !found {
			simInfo.vehicleHist.vehicles[v.VehicleId] = make(map[int][]segmentPos)
		}

		//get segments
		for _, s := range v.Segments {
			if s.Ts > simInfo.maxTs {
				simInfo.maxTs = s.Ts
			}

			//check if we already have a maping for this timestep
			if _, found := simInfo.vehicleHist.vehicles[v.VehicleId][s.Ts]; !found {
				simInfo.vehicleHist.vehicles[v.VehicleId][s.Ts] = make([]segmentPos, 0, len(s.S))
			}

			//segments at this timestep
			for _, data := range s.S {
				sid := int(data[0])

				simInfo.vehicleHist.vehicles[v.VehicleId][s.Ts] =
					append(simInfo.vehicleHist.vehicles[v.VehicleId][s.Ts], segmentPos{
						segmentId: vehicleData.Segments[sid],
						tsAgo: int(data[1]),
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

	//set the override
	if towersOverride > 0 {
		simInfo.towers = towersOverride
	}

	return &simInfo
}
