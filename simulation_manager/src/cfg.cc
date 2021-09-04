/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#include "cfg.h"
#include "log.h"
#include <exception>
#include <fstream>

namespace simulation_manager {

  #define SIMULATION_NETWORK_PATH_KEY "network_path"
  #define SIMULATION_ROUTES_PATH_KEY "routes_path"
  #define SIMULATION_SERVERS_PATH_KEY "servers_path"
  #define SIMULATION_STEPS_KEY "simulation_steps"
  #define SIMULATION_STEP_LENGTH_KEY "step_length"

  /**
   * Constructor takes the path to the configuration file
   * @param cfg_path the path to the simulation configuration file
   */
  cfg_t::cfg_t(const std::string& cfg_path)
    : network_file_path(),
      routes_file_path(),
      servers_file_path(),
      simulation_steps(DEFAULT_SIMULATION_STEPS),
      step_length(DEFAULT_STEP_LENGTH) {

    //the contents to be read from file
    json_t contents;

    //load the configuration from json
    try {
      //read in the file
      std::ifstream in_stream(cfg_path);

      //read the file into the json object
      in_stream >> contents;
      in_stream.close();

    } catch (const std::exception& e) {
      log::err("failed to read from simulation manager configuration file " +
                cfg_path + " (" + e.what() + ")");
    }

    /*
     * Extract configuration values from json or use defaults
     */

    if (contains_string(contents, SIMULATION_NETWORK_PATH_KEY)) {
      network_file_path = contents[SIMULATION_NETWORK_PATH_KEY].get<std::string>();

    } else {
      log::err("simulation configuration missing sumo network path");
    }

    if (contains_string(contents, SIMULATION_ROUTES_PATH_KEY)) {
      routes_file_path = contents[SIMULATION_ROUTES_PATH_KEY].get<std::string>();

    } else {
      log::err("simulation configuration missing sumo routes path");
    }

    if (contains_string(contents, SIMULATION_SERVERS_PATH_KEY)) {
      servers_file_path = contents[SIMULATION_SERVERS_PATH_KEY].get<std::string>();

    } else {
      log::err("simulation configuration missing sumo servers path");
    }

    if (contains_number(contents, SIMULATION_STEPS_KEY)) {
      simulation_steps = contents[SIMULATION_STEPS_KEY].get<int>();

    } else {
      log::info("using default simulation steps " + std::to_string(simulation_steps));
    }

    if (contains_number(contents, SIMULATION_STEP_LENGTH_KEY)) {
      step_length = contents[SIMULATION_STEP_LENGTH_KEY].get<float>();

    } else {
      log::info("using default simulation step length " + std::to_string(step_length));
    }
  }

  /**
   * Check if a key exists in the json object
   * @param  j   the json object
   * @param  key the key
   * @return     whether a string with the given key is found
   */
  bool contains_string(const json_t& j, const std::string& key) {
    return j.count(key) && j[key].is_string();
  }

  /**
   * Check if a key exists in the json object
   * @param  j   the json object
   * @param  key the key
   * @return     whether a number with the given key is found
   */
  bool contains_number(const json_t& j, const std::string& key) {
    return j.count(key) && j[key].is_number();
  }

}
