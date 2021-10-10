/*
 * Jack Hay, Sept 2021
 */

#ifndef _RENDER_OUTPUT_H
#define _RENDER_OUTPUT_H

#include <string>
#include <unordered_map>
#include "../types/tower_recognitions.h"
#include "../types/road_edge.h"
#include "../types/vehicle_lane_hist.h"
#include <memory>
#include <set>

namespace output {

  /**
   * Write the tower output format
   * @see docs/output.md
   * @param out_dir_path       path to the output directory
   * @param tower_recognitions tower recognitions map
   * @param vehicles           a set of unique vehicle ids
   * @param timesteps          all timesteps in the simulation
   * @return the status
   */
  int write_tower_output(const std::string& out_dir_path,
                          const std::unordered_map<std::string,std::unique_ptr<types::tower_recognitions_t>>& tower_recognitions,
                          const std::set<std::string>& vehicles,
                          const std::set<std::string>& timesteps);

  /**
   * Write the vehicle segment history to an output file
   * @param  out_dir_path      the path to write output to
   * @param  vehicle_lane_hist the history of
   * @param  edges             all edges in the simulation
   * @param  timesteps         all timesteps in the simulation
   * @return the status
   */
  int write_vehicle_output(const std::string& out_dir_path,
                           const std::unordered_map<std::string,std::unique_ptr<types::vehicle_lane_hist_t>>& vehicle_lane_hist,
                           const std::set<std::string>& edges,
                           const std::set<std::string>& timesteps);

  /**
   * Determine which segments in the network each tower covers
   * @param  out_dir_path        the directory to write output to
   * @param  tower_recognitions  recognitions for towers in the network
   * @param  edge_shapes         all of the edges (mapped by id)
   * @param  edges               the ids of all edges in the network
   * @param  towers              the ids of all towers in the network
   * @param  radius              the coverage radius
   * @return the status
   */
  int write_tower_coverage_output(const std::string& out_dir_path,
                                  const std::unordered_map<std::string,std::unique_ptr<types::tower_recognitions_t>>& tower_recognitions,
                                  const std::unordered_map<std::string,std::unique_ptr<types::road_edge_t>>& edge_shapes,
                                  const std::set<std::string>& edges,
                                  const std::set<std::string>& towers);
}

#endif /*_RENDER_OUTPUT_H*/
