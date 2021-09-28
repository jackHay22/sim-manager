/*
 * Jack Hay, Sept 2021
 */

#include "render_output.h"
#include <json.hpp>

namespace output {

  typedef nlohmann::json json_t;

  #define TOWER_ID_KEY "tower_id"
  #define VEHICLES_KEY "vehicles"

  /**
   * Write the tower output format
   * @see docs/output.md
   * @param out_file_path      the path to the output file
   * @param tower_recognitions tower recognitions map
   * @param vehicles           a set of unique vehicle ids
   */
  void tower_output(const std::string& out_file_path,
                    const std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>& tower_recognitions,
                    const std::set<std::string> vehicles) {

    json_t out_array = json_t::array();

    std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>::const_iterator it
      = tower_recognitions.begin();

    //read through all tower recognitions
    while (it != tower_recognitions.end()) {
      json_t elem = json_t::object();
      elem[TOWER_ID_KEY] = it->first;
      elem[VEHICLES_KEY] = json_t::array();

      //create array for each timestep
      for (const std::string& ts : timesteps) {
        json_t positions = json_t::array();

        for (const std::string& vehicle_id : vehicles) {

        }

        elem[VEHICLES_KEY].push_back(positions);
      }

      //add the tower element
      out_array.push_back(elem);

      it++;
    }

    //write to the file
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
                    const std::set<std::string>& towers,
                    const std::set<std::string>& vehicles,
                    const std::set<std::string>& timesteps) {

    //TODO vehicle segments visited matrix

    //create a csv writer
    csv_writer_t writer(out_path);

    //write the header

    //iterate over recognitions and add to csv
    std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>::iterator it
      = tower_recognitions.begin();

    //read through all tower recognitions
    while (it != tower_recognitions.end()) {
      //TODO

      it++;
    }

    return EXIT_SUCCESS;
  }
}
