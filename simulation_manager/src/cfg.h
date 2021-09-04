/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#ifndef _SIMULATION_MANAGER_CFG_H
#define _SIMULATION_MANAGER_CFG_H

#include <string>
#include <json.hpp>

namespace simulation_manager {

  typedef nlohmann::json json_t;

  const int DEFAULT_SIMULATION_STEPS = 1000; //TODO
  const float DEFAULT_STEP_LENGTH = 0.1;

  /**
   * Simulation configuration values
   */
  struct cfg_t {
    //the sumo network file
    std::string network_file_path;
    //the sumo route file
    std::string routes_file_path;
    //additional file containing server locations
    std::string servers_file_path;
    //the number of simulation steps
    int simulation_steps;
    //duration in between steps
    float step_length;

    //must be constructed with json contents
    cfg_t() = delete;

    /**
     * Constructor takes the path to the configuration file
     * @param cfg_path the path to the simulation configuration file
     */
    cfg_t(const std::string& cfg_path);

    //disable copying
    cfg_t(const cfg_t&) = delete;
    cfg_t& operator=(const cfg_t&) = delete;
  };

  /**
   * Check if a key exists in the json object
   * @param  j   the json object
   * @param  key the key
   * @return     whether a string with the given key is found
   */
  bool contains_string(const json_t& j, const std::string& key);

  /**
   * Check if a key exists in the json object
   * @param  j   the json object
   * @param  key the key
   * @return     whether a number with the given key is found
   */
  bool contains_number(const json_t& j, const std::string& key);

}

#endif /*_SIMULATION_MANAGER_CFG_H*/
