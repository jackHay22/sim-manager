/*
 * Jack Hay, Sept 2021
 */

#ifndef _VEHICLE_POSITION_H
#define _VEHICLE_POSITION_H

#include <string>

namespace recognitions {
  /*
   * Defines the position of a vehicle at some timestep
   */
  struct vehicle_position_t {
  private:
    //the id of the vehicle
    std::string vehicle_id;
    //the position
    double x;
    double y;
    //the speed
    double speed;

  public:
    /**
     * Constructor
     * @param vehicle_id the id of the vehicle
     * @param x          position x
     * @param y          position y
     * @param speed      the speed of the vehicle
     */
    vehicle_position_t(const std::string& vehicle_id,
                       double x,
                       double y,
                       double speed);

    //no copy
    vehicle_position_t(const vehicle_position_t&) = delete;
    vehicle_position_t& operator=(const vehicle_position_t&) = delete;
  };
}

#endif /*_VEHICLE_POSITION_H*/
