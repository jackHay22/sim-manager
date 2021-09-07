/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#ifndef _SIMULATION_MANAGER_CFG_H
#define _SIMULATION_MANAGER_CFG_H

#include <string>
#include <json.hpp>
#include <functional>

namespace simulation_manager {

  typedef nlohmann::json json_t;

  const int DEFAULT_SIMULATION_STEPS = 1000; //TODO
  const float DEFAULT_STEP_LENGTH = 0.1;

  /**
   * Simulation configuration values
   */
  struct sim_t {
    //the sumo network file
    std::string network_file_path;
    //the sumo route file
    std::string routes_file_path;
    //additional file containing tower locations
    std::string towers_file_path;
    //the number of simulation steps
    int simulation_steps;
    //duration in between steps
    float step_length;
  };

  struct cfg_t {
  private:
    //the default sumo network file
    std::string default_network_file_path;
    //the default sumo route file
    std::string default_routes_file_path;
    //default additional file containing tower locations
    std::string default_towers_file_path;

    //simulations
    std::vector<sim_t> simulations;

    //whether warnings have been issued already for taking defaults
    bool default_warn_sim_steps;
    bool default_warn_step_len;

    /**
     * Configure the contents of a simulation
     * @param s json representation
     */
    void configure_simulation(const json_t& s);

    /**
     * Check if json contains a value, assign default if not
     * (STRINGS)
     * @param s       json
     * @param key     the key to look for
     * @param value   the value to modify
     * @param default_val the default if key not found
     */
    void configure_or_default(const json_t&s,
                              const std::string& key,
                              std::string& value,
                              const std::string& default_val);

  public:
    /**
     * Constructor takes the path to the configuration file
     * @param cfg_path the path to the simulation configuration file
     */
    cfg_t(const std::string& cfg_path);

    //must be constructed with json contents
    cfg_t() = delete;
    cfg_t(const cfg_t&) = delete;
    cfg_t& operator=(const cfg_t&) = delete;

    /**
     * Takes a function executed for each simulation scenario as configured
     * @param fn the fn to execute
     */
    void for_each(std::function<void(const sim_t&)> fn) const;
  };

  /**
   * Check if a key exists in the json array
   * @param  j   the json object
   * @param  key the key
   * @return     whether a array with the given key is found
   */
  bool contains_array(const json_t& j, const std::string& key);

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
