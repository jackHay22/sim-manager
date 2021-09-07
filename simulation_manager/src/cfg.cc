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

  #define NETWORK_PATH_KEY "network_path"
  #define ROUTES_PATH_KEY "routes_path"
  #define TOWERS_PATH_KEY "towers_path"
  #define DEFAULT_NETWORK_PATH_KEY "default_network_path"
  #define DEFAULT_ROUTES_PATH_KEY "default_routes_path"
  #define DEFAULT_TOWERS_PATH_KEY "default_towers_path"
  #define SIMULATIONS_KEY "simulations"
  #define STEPS_KEY "simulation_steps"
  #define STEP_LENGTH_KEY "step_length"

  /**
   * Constructor takes the path to the configuration file
   * @param cfg_path the path to the simulation configuration file
   */
  cfg_t::cfg_t(const std::string& cfg_path)
    : default_network_file_path(),
      default_routes_file_path(),
      default_towers_file_path(),
      simulations(),
      default_warn_sim_steps(false),
      default_warn_step_len(false) {

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

     if (contains_string(contents, DEFAULT_NETWORK_PATH_KEY)) {
       default_network_file_path = contents[DEFAULT_NETWORK_PATH_KEY].get<std::string>();

     } else {
       log::err("simulation configuration missing default sumo network path");
     }

     if (contains_string(contents, DEFAULT_ROUTES_PATH_KEY)) {
       default_routes_file_path = contents[DEFAULT_ROUTES_PATH_KEY].get<std::string>();

     } else {
       log::err("simulation configuration missing default sumo routes path");
     }

     if (contains_string(contents, DEFAULT_TOWERS_PATH_KEY)) {
       default_towers_file_path = contents[DEFAULT_TOWERS_PATH_KEY].get<std::string>();

     } else {
       log::err("simulation configuration missing default sumo towers path");
     }


    if (contains_array(contents, SIMULATIONS_KEY)) {
      for (const json_t& sim : contents[SIMULATIONS_KEY]) {
        this->configure_simulation(sim);
      }

    } else {
      log::err("simulation configuration missing sumo simulations");
    }
  }

  /**
   * Configure the contents of a simulation
   * @param s json representation
   */
  void cfg_t::configure_simulation(const json_t& s) {
    //add a new simulation cfg
    simulations.emplace_back();

    if (contains_number(s, STEPS_KEY)) {
      simulations.back().simulation_steps = s[STEPS_KEY].get<int>();

    } else {
      if (!default_warn_sim_steps) {
        default_warn_sim_steps = true;
        simulations.back().simulation_steps = DEFAULT_SIMULATION_STEPS;
        log::info("using default simulation steps " +
                  std::to_string(DEFAULT_SIMULATION_STEPS) + " for simulation " +
                  std::to_string(simulations.size()) + " (will only warn once)");
      }
    }

    if (contains_number(s, STEP_LENGTH_KEY)) {
      simulations.back().step_length = s[STEP_LENGTH_KEY].get<float>();

    } else {
      if (!default_warn_step_len) {
        default_warn_step_len = true;
        simulations.back().step_length = DEFAULT_STEP_LENGTH;
        log::info("using default simulation step length " +
                  std::to_string(DEFAULT_STEP_LENGTH) + " for simulation " +
                  std::to_string(simulations.size()) + " (will only warn once)");
      }
    }

    //check for optionals
    configure_or_default(s, NETWORK_PATH_KEY,
                          simulations.back().network_file_path,
                          default_network_file_path);
    configure_or_default(s, ROUTES_PATH_KEY,
                          simulations.back().routes_file_path,
                          default_routes_file_path);
    configure_or_default(s, TOWERS_PATH_KEY,
                          simulations.back().towers_file_path,
                          default_towers_file_path);
  }

  /**
   * Check if json contains a value, assign default if not
   * @param s       json
   * @param key     the key to look for
   * @param value   the value to modify
   * @param default_val the default if key not found
   */
  void cfg_t::configure_or_default(const json_t&s,
                                   const std::string& key,
                                   std::string& value,
                                   const std::string& default_val) {
    if (contains_string(s, key)) {
      value = s[key].get<std::string>();
    } else {
      value = default_val;
    }
  }

  /**
   * Takes a function executed for each simulation scenario as configured
   * @param fn the fn to execute
   */
  void cfg_t::for_each(std::function<void(const sim_t&)> fn) const {
    for (size_t i=0; i<simulations.size(); i++) {
      fn(simulations.at(i));
    }
  }

  /**
   * Check if a key exists in the json array
   * @param  j   the json object
   * @param  key the key
   * @return     whether a array with the given key is found
   */
  bool contains_array(const json_t& j, const std::string& key) {
    return j.count(key) && j[key].is_array();
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
