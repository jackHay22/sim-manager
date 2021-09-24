/*
 * Jack Hay, Sept 2021
 */

#ifndef _VEHICLE_POSITIONS_H
#define _VEHICLE_POSITIONS_H

#include <string>
#include <tuple>
#include <unordered_map>

namespace vehicles {
  /*
   * Defines the positions a vehicle is in over the coarse of the simulation
   */
  struct vehicle_positions_t {
  private:
    //the id of the vehicle
    std::string vehicle_id;
    //positions this vehicle is in for given timesteps (third elem is speed)
    std::unordered_map<double,std::tuple<double,double,double>> positions;

  public:
    /**
     * Construct a vehicle by id
     * @param vehicle_id the id
     */
    vehicle_positions_t(const std::string& vehicle_id);

    //no copy
    vehicle_positions_t(const vehicle_positions_t&) = delete;
    vehicle_positions_t& operator=(const vehicle_positions_t&) = delete;

    /**
     * Add a new position for thie vehicle
     * @param timestep the timestep
     * @param x        the x position
     * @param y        the y position
     * @param speed    the speed of the vehicle
     */
    void add_position(double timestep, double x, double y, double speed);
  };
}

#endif /*_VEHICLE_POSITIONS_H*/
