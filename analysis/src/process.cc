/*
 * Jack Hay, Sept 2021
 */

#include "process.h"
#include <rapidxml.hpp>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include "vehicles/vehicle_positions.h"
#include <iostream>
#include <functional>
#include <exception>
#include <cstring>

#define FCD_EXPORT_NODE "fcd-export"
#define TIMESTEP_NODE "timestep"
#define VEHICLE_NODE "vehicle"
#define TIME_ATTR "time"
#define ID_ATTR "id"
#define SPEED_ATTR "speed"
#define X_ATTR "x"
#define Y_ATTR "y"

/**
 * Get the size of the file
 * @param  path the path to the input file
 * @return      the file size
 */
int get_size(const std::string& path) {
  FILE *file_p = NULL;
  file_p = fopen(path.c_str(),"rb");
  fseek(file_p,0,SEEK_END);
  int size = ftell(file_p);
  fclose(file_p);
  return size;
}

/**
 * Load the xml document from a path, execute handler, free memory
 * @param path    the path to the file
 * @param handler the lifetime of the xml in memory
 * @return success or failure
 */
[[nodiscard]] bool load_from_path(const std::string& path, std::function<void(const rapidxml::xml_document<>&)> handler) {
  //get the filesize and allocate space
  int filesize = get_size(path);
  char *buff = (char*) malloc(sizeof(char) * (filesize + 1));

  FILE *xml_file = fopen(path.c_str(), "rb");
  fread(buff, filesize, 1, xml_file);
  fclose(xml_file);

  buff[filesize] = 0;

  rapidxml::xml_document<> doc;
  doc.parse<0>(buff);

  bool success = true;

  try {
    //call the handler
    handler(doc);
  } catch (...) {
    std::cout << "ERR handler for " << path << " threw exception" << std::endl;
    success = false;
  }

  //free the buffer
  free(buff);

  return success;
}

/**
 * Add a vehicle to the mapping at some timestep
 * @param timestep     the current timestep
 * @param vehicle_node the xml node for the vehicle position
 * @param positions    previously seen vehicles lookup
 */
void add_vehicle(double timestep,
                 rapidxml::xml_node<> *vehicle_node,
                 std::unordered_map<std::string, std::unique_ptr<vehicles::vehicle_positions_t>>& positions) {
  //extract the attributes
  std::string vehicle_id = "-";
  double x = 0.0;
  double y = 0.0;
  double speed = 0.0;

  int not_found = 4;

  for (rapidxml::xml_attribute<> *v_attr = vehicle_node->first_attribute();
       v_attr;
       v_attr = v_attr->next_attribute()) {

    if (strcmp(v_attr->name(), ID_ATTR) == 0) {
      vehicle_id = std::string(v_attr->value());
      not_found--;
    } else if (strcmp(v_attr->name(), SPEED_ATTR) == 0) {
      speed = atof(v_attr->value());
      not_found--;
    } else if (strcmp(v_attr->name(), X_ATTR) == 0) {
      x = atof(v_attr->value());
      not_found--;
    } else if (strcmp(v_attr->name(), Y_ATTR) == 0) {
      y = atof(v_attr->value());
      not_found--;
    }
  }

  if (not_found) {
    std::cout << "ERR unable to find " << not_found << "vehicle attributes" << std::endl;
    return;
  }

  //check if the vehicle has been seen
  std::unordered_map<std::string, std::unique_ptr<vehicles::vehicle_positions_t>>::iterator it = positions.find(vehicle_id);
  if (it != positions.end()) {
    it->second->add_position(timestep, x, y, speed);
    return;
  }

  std::unique_ptr<vehicles::vehicle_positions_t> new_positions = std::make_unique<vehicles::vehicle_positions_t>(vehicle_id);
  new_positions->add_position(timestep, x, y, speed);

  //insert a new vehicle mapping
  positions.insert(std::make_pair(vehicle_id, std::move(new_positions)));
}

/**
 * Read the output files and generate an aggregated report
 * @param  bt_output_path       the path to the bluetooth output file
 * @param  fcd_output_path      the path to the raw fcd output file
 * @param  output_path          the path to write the output data to
 * @return                      success or failure
 */
int process_output_data(const std::string& /*bt_output_path*/,
                        const std::string& fcd_output_path,
                        const std::string& /*output_path*/) {
  //construct a mapping from vehicle id to all the positions of that vehicle
  std::unordered_map<std::string, std::unique_ptr<vehicles::vehicle_positions_t>> vehicle_positions;
  {
    //load the fcd xml file, read all vehicles and insert into the vehicle_positions map
    if (!load_from_path(fcd_output_path, [&vehicle_positions] (const rapidxml::xml_document<>& doc) {

      //verify the name of the root node
      if (strcmp(doc.first_node()->name(), FCD_EXPORT_NODE) != 0) {
        std::cout << "ERR doc root node not: " << FCD_EXPORT_NODE << std::endl;
        throw std::exception();
      }

      rapidxml::xml_node<> *fcd_node = doc.first_node(FCD_EXPORT_NODE);

      //get each timestep
      for (rapidxml::xml_node<> *timestep_node = fcd_node->first_node(TIMESTEP_NODE);
           timestep_node;
           timestep_node = timestep_node->next_sibling()) {

        //get the timestep
        for (rapidxml::xml_attribute<> *ts_attr = timestep_node->first_attribute();
             ts_attr;
             ts_attr = ts_attr->next_attribute()) {
          //check the attribute name
          if (strcmp(ts_attr->name(), TIME_ATTR) == 0) {
            //parse the timestep as a double
            double timestep_value = atof(ts_attr->value());

            //add all vehicles within this timestep
            for (rapidxml::xml_node<> *vehicle_node = timestep_node->first_node(VEHICLE_NODE);
                 vehicle_node;
                 vehicle_node = vehicle_node->next_sibling()) {
              //add to the mapping
              add_vehicle(timestep_value, vehicle_node, vehicle_positions);
              added++;
            }
          }
        }
      }

    })) {
      return EXIT_FAILURE;
    }

    //TODO


    return EXIT_SUCCESS;
  }

  //go through the fcd positions

  //std::unique_ptr<rapidxml::xml_document<>> bt_doc = load_from_path(bt_output_path);

  return EXIT_SUCCESS;
}
