/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#ifndef _SIMULATION_MANAGER_SIMULATION_H
#define _SIMULATION_MANAGER_SIMULATION_H

#include <string>
#include "cfg.h"

namespace simulation_manager {

  /**
   * Run the sumo simulation with the given configuration
   * @param  cfg        the configuration
   * @param  output_dir the directory to write output to
   */
  void run_simulation(const cfg_t& cfg, const std::string& output_dir);

}

#endif /*_SIMULATION_MANAGER_SIMULATION_H*/
