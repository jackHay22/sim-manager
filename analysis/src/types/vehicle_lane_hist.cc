/*
 * Jack Hay, Oct 2021
 */

#include "vehicle_lane_hist.h"

namespace types {

  /**
   * Constructor
   */
  vehicle_lane_hist_t::vehicle_lane_hist_t()
    : segments() {}

  /**
   * Constructor with initial value pair
   * @param segment_id segment id
   * @param timestep   current timestep
   */
  vehicle_lane_hist_t::vehicle_lane_hist_t(const std::string& segment_id, double timestep)
    : segments() {
    this->at_segment(segment_id, timestep);
  }

  /**
   * Set the timestep for a vehicle within some segment
   * @param segment_id the segment the vehicle is currently on
   * @param timestep   the current simulation timestep
   */
  void vehicle_lane_hist_t::at_segment(const std::string& segment_id, double timestep) {
    this->segments.insert(std::make_pair(segment_id, timestep));
  }

  /**
   * Get the timesteps since a segment was last seen
   * @param  segment_id the segment
   * @param  current_ts the current timestep
   * @return            the number of timesteps since being on that segment or -1 if never seen
   */
  double vehicle_lane_hist_t::timesteps_since_seen(const std::string& segment_id, double current_ts) const {
    std::unordered_map<std::string,double>::const_iterator it
      = segments.find(segment_id);

    if (it != segments.end()) {
      return current_ts - it->second;
    }
    return -1;
  }
}
