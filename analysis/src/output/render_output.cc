/*
 * Jack Hay, Sept 2021
 */

#include "render_output.h"
#include <json.hpp>
#include <filesystem>
#include <fstream>
#include <exception>
#include <iostream>

namespace output {

  typedef nlohmann::json json_t;

  #define TOWER_ID_KEY "tower_id"
  #define VEHICLES_KEY "vehicles"
  #define TOWERS_KEY   "towers"
  #define TS_KEY       "ts"
  #define V_KEY        "v"

  /**
   * Join a filename to a path that may or may not have a trailing slash
   * @param  dir  the directory
   * @param  file the file
   * @return      the full path
   */
  std::string join(const std::string& dir, const std::string& file) {
    std::filesystem::path out_dir(dir);
    std::filesystem::path out_file(file);
    std::filesystem::path full = out_dir / out_file;
    return full.string();
  }

  /**
   * Write the tower output format
   * @see docs/output.md
   * @param out_file_path      the path to the output file
   * @param tower_recognitions tower recognitions map
   * @param vehicles           a set of unique vehicle ids
   * @param timesteps          all timesteps in the simulation
   */
  void tower_output(std::string&& out_file_path,
                    const std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>& tower_recognitions,
                    const std::set<std::string>& vehicles,
                    const std::set<std::string>& timesteps) {

    json_t out_obj = json_t::object();
    out_obj[VEHICLES_KEY] = json_t::array();
    out_obj[TOWERS_KEY] = json_t::array();

    //add the vehicle ids
    for (const std::string& vehicle_id : vehicles) {
      out_obj[VEHICLES_KEY].push_back(vehicle_id);
    }

    std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>::const_iterator it
      = tower_recognitions.begin();

    //read through all tower recognitions
    while (it != tower_recognitions.end()) {
      json_t elem = json_t::object();
      elem[TOWER_ID_KEY] = it->first;
      elem[VEHICLES_KEY] = json_t::array();

      //create array for each timestep
      for (const std::string& ts : timesteps) {
        json_t positions = json_t::object();
        positions[TS_KEY] = std::stod(ts);
        positions[V_KEY] = json_t::array();

        for (const std::string& vehicle_id : vehicles) {
          positions[V_KEY].push_back(it->second->distance(ts, vehicle_id));
        }

        elem[VEHICLES_KEY].push_back(positions);
      }

      //add the tower element
      out_obj[TOWERS_KEY].push_back(elem);

      it++;
    }

    //write to the file
    try {
      std::ofstream out_file(out_file_path);
      out_file << out_obj << std::endl;
      out_file.close();

      std::cerr << "INFO: wrote tower output to: " << out_file_path << std::endl;

    } catch (const std::exception& e) {
      std::cerr << "ERR: failed to write tower output to file: " << e.what() << std::endl;
    }
  }



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
                    const std::set<std::string>& /*towers*/,
                    const std::set<std::string>& vehicles,
                    const std::set<std::string>& timesteps) {

    //write towers output
    tower_output(join(out_path, "tower_output.json"),
                 tower_recognitions,
                 vehicles,
                 timesteps);

    return EXIT_SUCCESS;
  }
}
