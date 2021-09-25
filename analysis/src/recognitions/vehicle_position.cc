/*
 * Jack Hay, Sept 2021
 */

#include "vehicle_position.h"

namespace recognitions {

  /**
   * Constructor
   * @param vehicle_id the id of the vehicle
   * @param x          position x
   * @param y          position y
   * @param speed      the speed of the vehicle
   */
  vehicle_position_t::vehicle_position_t(const std::string& vehicle_id,
                                         double x,
                                         double y,
                                         double speed)
    : vehicle_id(vehicle_id),
      x(x),
      y(y),
      speed(speed) {}
}
