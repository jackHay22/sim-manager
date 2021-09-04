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
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>

//need to forward declare this to include the polygon header
namespace libsumo { struct PositionVector; }

#include <libsumo/Polygon.h>

namespace simulation_manager {

  using namespace libsumo;

  #define SERVER_ID(idx) "server_" + std::to_string(idx)

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

    //positions of servers
    std::vector<std::pair<double,double>> server_positions;

    //identify the number of servers
    size_t servers = 0;

    while (Polygon::exists(SERVER_ID(servers))) {
      //get the polygon region
      TraCIPositionVector shape = Polygon::getShape(SERVER_ID(servers));
      //add this server position
      server_positions.push_back(std::make_pair(0,0)); //TODO
      servers++;
    }

    //log the number of polygons identified as servers in the servers file
    log::info("loaded " + std::to_string(servers) + " servers");

    //step through the simulation
    for (int i=0; i<cfg.simulation_steps; i++) {
      Simulation::step();

      //int current_time = Simulation::getCurrentTime();

      //identify car positions
      //static libsumo::TraCIPosition getPosition(const std::string& vehID, const bool includeZ = false);
    }

    //close the simulation
    Simulation::close();

    log::info("finished simulation");
  }

}
