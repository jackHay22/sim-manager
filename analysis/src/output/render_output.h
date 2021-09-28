/*
 * Jack Hay, Sept 2021
 */

#ifndef _RENDER_OUTPUT_H
#define _RENDER_OUTPUT_H

#include <string>
#include <unordered_map>
#include "../recognitions/tower_recognitions.h"
#include <memory>
#include <set>

namespace output {

  /**
   * Transform and write output to files
   * @param out_path           the path to write files to (should be a folder)
   * @param tower_recognitions all recognitions from the simulation
   * @param towers             the ids of all towers
   * @param vehicles           the ids of all vehicles
   * @param timesteps          all of the timestep values in the simulation
   * @return success or failure
   */
  int render_output(const std::string& out_path,
                    const std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>& tower_recognitions,
                    const std::set<std::string>& towers,
                    const std::set<std::string>& vehicles,
                    const std::set<std::string>& timesteps);
}

#endif /*_RENDER_OUTPUT_H*/
