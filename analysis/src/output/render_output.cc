/*
 * Jack Hay, Sept 2021
 */

#include "render_output.h"
#include <json.hpp>
#include <fstream>
#include <exception>
#include <iostream>
#include <unistd.h>

namespace output {

  typedef nlohmann::json json_t;

  #define TOWER_ID_KEY "tower_id"
  #define VEHICLES_KEY "vehicles"
  #define TOWERS_KEY   "towers"
  #define SEGMENTS_KEY "segments"
  #define TS_KEY       "ts"
  #define V_KEY        "v"

  /**
   * Join a filename to a path that may or may not have a trailing slash
   * @param  dir  the directory
   * @param  file the file
   * @return      the full path
   */
  std::string join(const std::string& dir, const std::string& file) {
    std::string _dir = dir;
    if ((_dir.size() > 0) && (_dir.back() != '/')) {
      _dir += "/";
    }

    _dir += file;
    return _dir;
  }

  /**
   * Write the tower output format
   * @see docs/output.md
   * @param out_dir_path       path to the output directory
   * @param tower_recognitions tower recognitions map
   * @param vehicles           a set of unique vehicle ids
   * @param timesteps          all timesteps in the simulation
   * @return the status
   */
  int write_tower_output(const std::string& out_dir_path,
                          const std::unordered_map<std::string, std::unique_ptr<types::tower_recognitions_t>>& tower_recognitions,
                          const std::set<std::string>& vehicles,
                          const std::set<std::string>& timesteps) {

    json_t out_obj = json_t::object();
    out_obj[VEHICLES_KEY] = json_t::array();
    out_obj[TOWERS_KEY] = json_t::array();

    //add the vehicle ids
    for (const std::string& vehicle_id : vehicles) {
      out_obj[VEHICLES_KEY].push_back(vehicle_id);
    }

    std::unordered_map<std::string, std::unique_ptr<types::tower_recognitions_t>>::const_iterator it
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

        int vidx = 0;
        for (const std::string& vehicle_id : vehicles) {
          //get the distance
          double dist = it->second->distance(ts, vehicle_id);
          //add if in range
          if (dist > -1) {
            json_t pair = json_t::array();
            pair.push_back(vidx);
            pair.push_back(dist);
            positions[V_KEY].push_back(pair);
          }

          vidx++;
        }

        elem[VEHICLES_KEY].push_back(positions);
      }

      //add the tower element
      out_obj[TOWERS_KEY].push_back(elem);

      it++;
    }

    //write to the file
    try {
      std::string full_path = join(out_dir_path, "tower_output.json");

      std::ofstream out_file(full_path);
      out_file << out_obj << std::endl;
      out_file.close();

      std::cerr << "INFO: wrote tower output to: " << full_path << std::endl;

    } catch (const std::exception& e) {
      std::cerr << "ERR: failed to write tower output to file: " << e.what() << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }

  /**
   * Determine which segments in the network each tower covers
   * @param  out_dir_path        the directory to write output to
   * @param  tower_recognitions  recognitions for towers in the network
   * @param  edge_shapes         all of the edges (mapped by id)
   * @param  edges               the ids of all edges in the network
   * @param  towers              the ids of all towers in the network
   * @return the status
   */
  int write_tower_coverage_output(const std::string& out_dir_path,
                                  const std::unordered_map<std::string,std::unique_ptr<types::tower_recognitions_t>>& tower_recognitions,
                                  const std::unordered_map<std::string,std::unique_ptr<types::road_edge_t>>& edge_shapes,
                                  const std::set<std::string>& edges,
                                  const std::set<std::string>& towers) {
    json_t out_obj = json_t::object();
    out_obj[SEGMENTS_KEY] = json_t::array();
    out_obj[TOWERS_KEY] = json_t::array();

    for (const std::string& edge_id : edges) {
      out_obj[SEGMENTS_KEY].push_back(edge_id);
    }

    for (const std::string& tower_id : towers) {
      json_t tower = json_t::object();
      tower[TOWER_ID_KEY] = tower_id;
      tower[SEGMENTS_KEY] = json_t::array();

      std::unordered_map<std::string,std::unique_ptr<types::tower_recognitions_t>>::const_iterator tower_it
        = tower_recognitions.find(tower_id);

      if (tower_it != tower_recognitions.end()) {

        for (const std::string& edge_id : edges) {
          std::unordered_map<std::string,std::unique_ptr<types::road_edge_t>>::const_iterator edge_it
            = edge_shapes.find(edge_id);

          if (edge_it != edge_shapes.end()) {

            //add the distance
            tower[SEGMENTS_KEY].push_back(
              tower_it->second->edge_distance(*edge_it->second)
            );
          }
        }

        out_obj[TOWERS_KEY].push_back(tower);
      } else {
        std::cerr << "WARN: missing recognitions for tower " << tower_id << std::endl;
      }
    }

    //write to the file
    try {
      std::string full_path = join(out_dir_path, "tower_coverage_output.json");

      std::ofstream out_file(full_path);
      out_file << out_obj << std::endl;
      out_file.close();

      std::cerr << "INFO: wrote tower coverage output to: " << full_path << std::endl;

    } catch (const std::exception& e) {
      std::cerr << "ERR: failed to write tower coverage output to file: " << e.what() << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
}
