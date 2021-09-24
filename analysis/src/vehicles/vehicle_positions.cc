/*
 * Jack Hay, Sept 2021
 */

#include "vehicle_positions.h"

namespace vehicles {
  /**
   * Construct a vehicle by id
   * @param vehicle_id the id
   */
  vehicle_positions_t::vehicle_positions_t(const std::string& vehicle_id)
    : vehicle_id(vehicle_id), positions() {}

  /**
   * Add a new position for thie vehicle
   * @param timestep the timestep
   * @param x        the x position
   * @param y        the y position
   * @param speed    the speed of the vehicle
   */
  void vehicle_positions_t::add_position(double timestep, double x, double y, double speed) {
    this->positions.insert(std::make_pair(
      timestep, std::make_tuple(x,y,speed)
    ));
  }
}
