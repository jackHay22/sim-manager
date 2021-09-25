/*
 * Jack Hay, Sept 2021
 */

#include "tower_recognitions.h"
#include <cmath>

namespace recognitions {

  /**
   * Calculate the distance between two points
   * @param  x0,y0 first point
   * @param  x1,y1 second point
   * @return cartesian distance
   */
  inline double distance(double x0, double y0,
                         double x1, double y1) {
    return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
  }

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
   * Add a vehicle recognition for a given timestep
   * @param timestep the timestep
   * @param position the position of a recognized vehicle
   */
  void tower_recognitions_t::add_recognition(std::string&& timestep,
                                             std::unique_ptr<vehicle_position_t> position) {
    vehicles[timestep].push_back(std::move(position));
  }
}
