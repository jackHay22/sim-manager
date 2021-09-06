/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#include "simulation.h"
#include "log.h"
#include <unistd.h>
#include <utility>
#include <libsumo/Simulation.h>
#include <libsumo/Vehicle.h>

namespace simulation_manager {

  using namespace libsumo;

  /**
   * Run the sumo simulation with the given configuration
   * @param  cfg        the configuration
   * @param  output_dir the directory to write output to
   */
  void run_simulation(const cfg_t& cfg, const std::string& /*output_dir*/) {
    log::info("running simulation");

    //load the simulation
    Simulation::load({
      "-n", cfg.network_file_path,
      "-r", cfg.routes_file_path,
      "-a", cfg.servers_file_path,
      "--step-length", std::to_string(cfg.step_length)
    });

    //check if the simulation is loaded
    if (!Simulation::isLoaded()) {
      log::err("failed to load simulation from: " + cfg.network_file_path);
    }

    //step through the simulation
    for (int i=0; i<cfg.simulation_steps; i++) {
      Simulation::step();

      //int current_time = Simulation::getCurrentTime();
    }

    //close the simulation
    Simulation::close();

    log::info("finished simulation");
  }

}
