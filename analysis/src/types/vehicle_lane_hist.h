/*
 * Jack Hay, Oct 2021
 */

#ifndef _VEHICLE_LANE_HIST_H
#define _VEHICLE_LANE_HIST_H

#include <string>
#include <memory>
#include <unordered_map>
#include <utility>

namespace types {
  /*
   * Defines the history of lanes visited by a vehicle
   */
  struct vehicle_lane_hist_t {
  private:
    //the segments the vehicle has visited and when
    std::unordered_map<std::string,double> segments;

  public:
    /**
     * Constructor
     */
    vehicle_lane_hist_t();

    /**
     * Constructor with initial value pair
     * @param segment_id segment id
     * @param timestep   current timestep
     */
    vehicle_lane_hist_t(const std::string& segment_id, double timestep);

    //no copy
    vehicle_lane_hist_t(const vehicle_lane_hist_t&) = delete;
    vehicle_lane_hist_t& operator=(const vehicle_lane_hist_t&) = delete;

    /**
     * Set the timestep for a vehicle within some segment
     * @param segment_id the segment the vehicle is currently on
     * @param timestep   the current simulation timestep
     */
    void at_segment(const std::string& segment_id, double timestep);

    /**
     * Get the timesteps since a segment was last seen
     * @param  segment_id the segment
     * @param  current_ts the current timestep
     * @return            the number of timesteps since being on that segment or -1 if never seen
     */
    double timesteps_since_seen(const std::string& segment_id, double current_ts) const;
  };
}

#endif /*_VEHICLE_LANE_HIST_H*/
