/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#include <unistd.h>
#include <string>
#include "simulation.h"
#include "cfg.h"
#include <iostream>

/**
 * Entrypoint
 */
int main(int argc, char **argv) {
  int c;

  //default configuration location
  std::string cfg_path = "simulation_cfg.json";
  std::string output_path = "";

  //get command line options
  while ((c = getopt(argc, argv, "c:o:")) != -1) {
    if (c == 'c') {
      cfg_path = std::string(optarg);
    } else if (c == 'o') {
      output_path = std::string(optarg);
    }
  }

  try {
    //load the configuration
    simulation_manager::cfg_t cfg(cfg_path);

    //run the sumo simulation on this configuration
    simulation_manager::run_simulation(cfg, output_path);

  } catch (...) {
    std::cerr << "failed to run simulation" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
