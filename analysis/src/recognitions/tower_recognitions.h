/*
 * Jack Hay, Sept 2021
 */

#ifndef _TOWER_RECOGNITIONS_H
#define _TOWER_RECOGNITIONS_H

#include <string>
#include "vehicle_position.h"
#include <unordered_map>
#include <memory>
#include <vector>

namespace recognitions {
  /*
   * Defines all of the bluetooth recognitions for a given tower
   */
  struct tower_recognitions_t {
  private:
    //the id of the tower
    std::string tower_id;
    //the tower position
    double x;
    double y;
    //vehicles this tower has seen for each given timestep
    std::unordered_map<double,std::vector<std::unique_ptr<vehicle_position_t>>> vehicles;

  public:
    /**
     * Constructor
     * @param tower_id the tower id
     */
    tower_recognitions_t(const std::string& tower_id);

    //no copy
    tower_recognitions_t(const tower_recognitions_t&) = delete;
    tower_recognitions_t& operator=(const tower_recognitions_t&) = delete;

    /**
     * Set the position for this tower
     * @param x position x
     * @param y position y
     */
    void set_position(double x, double y);

    /**
     * Add a vehicle recognition for a given timestep
     * @param timestep the timestep
     * @param position the position of a recognized vehicle
     */
    void add_recognition(double timestep, std::unique_ptr<vehicle_position_t> position);
  };
}

#endif /*_TOWER_RECOGNITIONS_H*/
