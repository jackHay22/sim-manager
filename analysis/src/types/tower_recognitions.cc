/*
 * Jack Hay, Sept 2021
 */

#include "tower_recognitions.h"

namespace types {

  /**
   * Constructor
   * @param tower_id the tower id
   * @param x        the tower position x
   * @param y        the tower position y
   */
  tower_recognitions_t::tower_recognitions_t(const std::string& tower_id)
    : tower_id(tower_id),
      x(0),
      y(0),
      vehicles() {}


  /**
   * Set the position for this tower
   * @param x position x
   * @param y position y
   */
  void tower_recognitions_t::set_position(double x, double y) {
    this->x = x;
    this->y = y;
  }

  /**
   * Get the distance from a vehicle to the tower
   * (-1 if out of range)
   * @param  timestep   the timestep to check for
   * @param  vehicle_id the id of the vehicle
   * @return            the distance
   */
  double tower_recognitions_t::distance(const std::string& timestep, const std::string& vehicle_id) const {
    //look for the vehicle in the map
    std::unordered_map<std::string,std::unordered_map<std::string,double>>::const_iterator it
      = vehicles.find(timestep);

    if (it != vehicles.end()) {
      //find the vehicle for this timestep
      std::unordered_map<std::string,double>::const_iterator it2 = it->second.find(vehicle_id);
      if (it2 != it->second.end()) {
        return it2->second;
      }
    }
    return -1;
  }

  /**
   * Add a vehicle recognition for this twoer
   * @param timestep   the current timestep
   * @param vehicle_id the id of the vehicle
   * @param dist       the distance from the tower to the vehicle
   */
  void tower_recognitions_t::add_recognition(std::string&& timestep, std::string&& vehicle_id, double dist) {
    //add to lookup
    vehicles[timestep][vehicle_id] = dist;
  }

  /**
   * Check if this tower covers a given edge by some radius
   * @param  edge   the edge
   * @param  radius the radius of the coverage range
   * @return        whether the tower covers the edge
   */
  bool tower_recognitions_t::covers_edge(const road_edge_t& edge, double radius) const {
    return edge.in_range(this->x, this->y, radius);
  }
}
