/*
 * Jack Hay, Sept 2021
 */

#include "tower_recognitions.h"

namespace recognitions {

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
  void tower_recognitions_t::add_recognition(double timestep, std::unique_ptr<vehicle_position_t> position) {
    vehicles[timestep].push_back(std::move(position));
  }
}
