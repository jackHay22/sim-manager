/*
 * Jack Hay, Sept 2021
 */

#include "process.h"
#include <rapidxml.hpp>
#include <string_view>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include "recognitions/vehicle_position.h"
#include "recognitions/tower_recognitions.h"
#include "output/transform.h"
#include <iostream>
#include <functional>
#include <exception>
#include <cstring>
#include <stdint.h>

#define BT_OUTPUT_NODE          "bt-output"
#define BT_NODE                 "bt"
#define SEEN_NODE               "seen"
#define RECOGNITION_POINT_NODE  "recognitionPoint"
#define ID_ATTR                 "id"
#define OBSERVER_POS_END_ATTR   "observerPosEnd"
#define T_ATTR                  "t"
#define SEEN_POS_ATTR           "seenPos"
#define SEEN_SPEED_ATTR         "seenSpeed"

#define COMMA ','

/**
 * Get the size of the file
 * @param  path the path to the input file
 * @return      the file size
 */
uint64_t get_size(const std::string& path) {
  FILE *file_p = NULL;
  file_p = fopen(path.c_str(),"rb");
  fseek(file_p,0,SEEK_END);
  uint64_t size = ftell(file_p);
  fclose(file_p);
  return size;
}

/**
 * Parse a signed pair of doubles from a string
 * @param  pos the position as a string
 * @param  x   parsed position x
 * @param  y   parsed position y
 * @return     whether the pair was parsed successfully
 */
bool parse_position(const std::string& pos, double& x, double& y) {
  std::stringstream sstream(pos);
  bool got_x = false;
  std::string substr;

  try {
    while(sstream.good()) {
      getline(sstream, substr, COMMA);
      if (got_x) {
        y = atof(substr.c_str());
      } else {
        x = atof(substr.c_str());
        got_x = true;
      }
    }
  } catch (...) {
    return false;
  }
  return true;
}

/**
 * Load the xml document from a path, execute handler, free memory
 * @param path    the path to the file
 * @param handler the lifetime of the xml in memory
 * @return success or failure
 */
[[nodiscard]] bool load_from_path(const std::string& path, std::function<void(const rapidxml::xml_document<>&)> handler) {
  //get the filesize and allocate space
  uint64_t filesize = get_size(path);
  char *buff = (char*) malloc(sizeof(char) * (filesize + 1));

  FILE *xml_file = fopen(path.c_str(), "rb");
  fread(buff, filesize, 1, xml_file);
  fclose(xml_file);

  buff[filesize] = 0;

  rapidxml::xml_document<> doc;

  bool success = true;

  try {
    //parse from the buffer
    doc.parse<0>(buff);
  } catch (rapidxml::parse_error& e) {
    std::cerr << "ERR: parse error: " << e.what() << std::endl;
    success = false;
  } catch (...) {
    std::cerr << "ERR: parse error" << std::endl;
    success = false;
  }

  if (success) {
    try {
      //call the handler
      handler(doc);
    } catch (...) {
      std::cerr << "ERR handler for " << path << " threw exception" << std::endl;
      success = false;
    }
  }

  //free the buffer (always)
  free(buff);

  return success;
}

/**
 * Add a tower recognition collector if not already set for this id
 * @param  tower_id           the id of the tower
 * @param  tower_recognitions all tower recognitions
 * @return                    the tower recognition collector
 */
recognitions::tower_recognitions_t& add_tower(std::string&& tower_id,
                                              std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>& tower_recognitions) {
  std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>::iterator it = tower_recognitions.find(tower_id);
  if (it != tower_recognitions.end()) {
    return *it->second;
  }

  //add new
  tower_recognitions.insert(std::make_pair(
    tower_id,
    std::make_unique<recognitions::tower_recognitions_t>(tower_id)
  ));
  return add_tower(std::move(tower_id), tower_recognitions);
}

/**
 * Add all of the recognition points for a vehicle
 * @param tower     the tower
 * @param seen_node the list of recognition points
 */
void add_recognition_points(recognitions::tower_recognitions_t& tower,
                            rapidxml::xml_node<> *seen_node) {
  //extract the attributes
  std::string vehicle_id = "-";

  int not_found = 2;

  for (rapidxml::xml_attribute<> *sn_attr = seen_node->first_attribute();
       sn_attr;
       sn_attr = sn_attr->next_attribute()) {

    if (strcmp(sn_attr->name(), ID_ATTR) == 0) {
      vehicle_id = std::string(sn_attr->value());
      not_found--;
    } else if (strcmp(sn_attr->name(), OBSERVER_POS_END_ATTR) == 0) {
      std::string tower_pos = std::string(sn_attr->value());
      double tower_x = 0.0;
      double tower_y = 0.0;
      //parse the tower position
      if (!parse_position(tower_pos, tower_x, tower_y)) {
        std::cerr << "ERR unable to parse tower position " << tower_pos << std::endl;
        return;
      }
      tower.set_position(tower_x,tower_y);
      not_found--;
    }
  }

  if (not_found) {
    std::cerr << "ERR unable to find " << not_found << " (seen) recognition point attributes" << std::endl;
    return;
  }

  //add recognition points
  for (rapidxml::xml_node<> *rp_node = seen_node->first_node(RECOGNITION_POINT_NODE);
       rp_node;
       rp_node = rp_node->next_sibling()) {
    double ts = 0.0;
    double v_x = 0.0;
    double v_y = 0.0;
    double v_s = 0.0;

    not_found = 3;
    //look through attributes
    for (rapidxml::xml_attribute<> *rp_attr = rp_node->first_attribute();
         rp_attr;
         rp_attr = rp_attr->next_attribute()) {

      if (strcmp(rp_attr->name(), T_ATTR) == 0) {
        ts = atof(rp_attr->value());
        not_found--;

      } else if (strcmp(rp_attr->name(), SEEN_POS_ATTR) == 0) {
        std::string seen_pos = std::string(rp_attr->value());
        if (!parse_position(seen_pos, v_x, v_y)) {
          std::cerr << "ERR unable to parse vehicle position " << seen_pos << std::endl;
          return;
        }

        not_found--;
      } else if (strcmp(rp_attr->name(), SEEN_SPEED_ATTR) == 0) {
        v_s = atof(rp_attr->value());
        not_found--;
      }
    }

    if (not_found) {
      std::cerr << "ERR unable to find " << not_found << " recognition point attributes" << std::endl;
      return;
    }

    //add the recognition point
    tower.add_recognition(
      ts,
      std::make_unique<recognitions::vehicle_position_t>(vehicle_id, v_x, v_y, v_s)
    );
  }
}

/**
 * Read the output files and generate an aggregated report
 * @param  bt_output_path       the path to the bluetooth output file
 * @param  output_path          the path to write the output data to
 * @return                      success or failure
 */
int process_output_data(const std::string& bt_output_path,
                        const std::string& output_path) {
  //construct a mapping from tower id to all recognition points
  std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>> tower_recognitions;
  {
    //load the bt xml file, add recognitions to map
    if (!load_from_path(bt_output_path, [&tower_recognitions] (const rapidxml::xml_document<>& doc) {

      //verify the name of the root node
      if (strcmp(doc.first_node()->name(), BT_OUTPUT_NODE) != 0) {
        std::cerr << "ERR doc root node not: " << BT_OUTPUT_NODE << std::endl;
        throw std::exception();
      }

      rapidxml::xml_node<> *bt_node = doc.first_node(BT_OUTPUT_NODE);

      //get each tower
      for (rapidxml::xml_node<> *tower_node = bt_node->first_node(BT_NODE);
           tower_node;
           tower_node = tower_node->next_sibling()) {

        //get the timestep
        for (rapidxml::xml_attribute<> *tower_attr = tower_node->first_attribute();
             tower_attr;
             tower_attr = tower_attr->next_attribute()) {
          //check the attribute name
          if (strcmp(tower_attr->name(), ID_ATTR) == 0) {
            //create a tower
            recognitions::tower_recognitions_t& tower = add_tower(std::string(tower_attr->value()),
                                                                  tower_recognitions);

            //add vehicle recognitions
            for (rapidxml::xml_node<> *seen_node = tower_node->first_node(SEEN_NODE);
                 seen_node;
                 seen_node = seen_node->next_sibling()) {

              //add recognition points for this tower
              add_recognition_points(tower, seen_node);
            }
          }
        }
      }

    })) {
      return EXIT_FAILURE;
    }
  }

  //reshape output and write to file
  return output::render_output(output_path, tower_recognitions);
}
