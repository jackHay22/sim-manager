/*
 * Jack Hay, Sept 2021
 */

#ifndef _TOWER_RECOGNITIONS_H
#define _TOWER_RECOGNITIONS_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <tuple>

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
    //vehicles this tower has seen for each given timestep (timestep remains a string)
    std::unordered_map<std::string,std::unordered_map<std::string,double>> vehicles;

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
     * Get the distance from a vehicle to the tower
     * (-1 if out of range)
     * @param  timestep   the timestep to check for
     * @param  vehicle_id the id of the vehicle
     * @return            the distance
     */
    double distance(const std::string& timestep, const std::string& vehicle_id) const;

    /**
     * Add a vehicle recognition for this twoer
     * @param timestep   the current timestep
     * @param vehicle_id the id of the vehicle
     * @param dist       the distance from the tower to the vehicle
     */
    void add_recognition(std::string&& timestep, std::string&& vehicle_id, double dist);

  };
}

#endif /*_TOWER_RECOGNITIONS_H*/
