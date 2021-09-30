/*
 * Jack Hay, Sept 2021
 */

#ifndef _RENDER_OUTPUT_H
#define _RENDER_OUTPUT_H

#include <string>
#include <unordered_map>
#include "../types/tower_recognitions.h"
#include "../types/road_edge.h"
#include <memory>
#include <set>
#include <utility>

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
                          const std::unordered_map<std::string, std::unique_ptr<types::tower_recognitions_t>>& tower_recognitions,
                          const std::set<std::string>& vehicles,
                          const std::set<std::string>& timesteps);

  // int write_vehicle_output(const std::string& out_dir_path,
  //                          );

  /**
   * Determine the tower coverage over segments within the road network
   * @param  out_dir_path    directory to write output files to
   * @param  tower_positions the positions of towers in the network
   * @param  road_edges      the edges of all roads in the network
   * @return                 the status
   */
  int write_tower_coverage_output(const std::string& out_dir_path,
                                  const std::unordered_map<std::string,std::pair<double,double>>& tower_positions,
                                  const std::unordered_map<std::string,std::unique_ptr<types::road_edge_t>>& road_edges);
}

#endif /*_RENDER_OUTPUT_H*/
