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
#include "types/tower_recognitions.h"
#include "output/render_output.h"
#include <iostream>
#include <functional>
#include <exception>
#include <cstring>
#include <stdint.h>
#include <set>
#include <cmath>
#include <utility>

#define BT_OUTPUT_NODE          "bt-output"
#define BT_NODE                 "bt"
#define NET_NODE                "net"
#define EDGE_NODE               "edge"
#define LANE_NODE               "lane"
#define SEEN_NODE               "seen"
#define NETSTATE_NODE           "netstate"
#define VEHICLE_NODE            "vehicle"
#define TIMESTEP_NODE           "timestep"
#define RECOGNITION_POINT_NODE  "recognitionPoint"
#define ID_ATTR                 "id"
#define OBSERVER_POS_END_ATTR   "observerPosEnd"
#define T_ATTR                  "t"
#define TIME_ATTR               "time"
#define SEEN_POS_ATTR           "seenPos"
#define SEEN_SPEED_ATTR         "seenSpeed"
#define FUNCTION_ATTR           "function"
#define INTERNAL_VAL            "internal"
#define SHAPE_ATTR              "shape"

//tower vehicle prefix
#define TOWER_PREFIX "tower"

#define COMMA ','
#define SPACE ' '

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
[[nodiscard]] bool parse_position(const std::string& pos, double& x, double& y) {
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
 * Parse a list of space separated, comma separated pairs of doubles
 * @param  shape    the string encoding
 * @param  vertices the parsed pairs
 * @return          whether the pairs were parsed successfully
 */
[[nodiscard]] bool parse_shape(const std::string& shape, std::vector<std::pair<double,double>>& vertices) {
  std::stringstream sstream(shape);
  std::string pair;
  while (std::getline(sstream, pair, SPACE)) {
    //split the pair
    std::pair<double,double> p;
    int dim = 0;
    std::string v;

    std::stringstream pair_sstream(pair);
    while (std::getline(pair_sstream, v, COMMA)) {
      //parse as double
      if (dim == 0) {
        p.first = atof(v.c_str());
      } else {
        p.second = atof(v.c_str());
      }
      dim++;
    }

    if (dim == 2) {
      vertices.push_back(p);
    } else {
      return false;
    }
  }

  //shape assumes greater than a single vertex
  return vertices.size() > 1;
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
  if (fread(buff, filesize, 1, xml_file) == 0) {
    std::cout << "ERR: failed to read from " << path << std::endl;
    free(buff);
    fclose(xml_file);
    return false;
  }
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
 * Calculate the distance between two points
 * @param  x0,y0 first point
 * @param  x1,y1 second point
 * @return cartesian distance
 */
inline double distance(double x0, double y0,
                       double x1, double y1) {
  return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}

/**
 * Add a tower recognition collector if not already set for this id
 * @param  tower_id           the id of the tower
 * @param  tower_recognitions all tower recognitions
 * @return                    the tower recognition collector
 */
types::tower_recognitions_t& add_tower(std::string&& tower_id,
                                       std::unordered_map<std::string, std::unique_ptr<types::tower_recognitions_t>>& tower_recognitions) {
  std::unordered_map<std::string, std::unique_ptr<types::tower_recognitions_t>>::iterator it = tower_recognitions.find(tower_id);
  if (it != tower_recognitions.end()) {
    return *it->second;
  }

  //add new
  tower_recognitions.insert(std::make_pair(
    tower_id,
    std::make_unique<types::tower_recognitions_t>(tower_id)
  ));
  return add_tower(std::move(tower_id), tower_recognitions);
}

/**
 * Add all of the recognition points for a vehicle
 * @param tower             the tower
 * @param seen_node         the list of recognition points
 * @param vehicles          set of unique vehicle ids
 */
void add_recognition_points(types::tower_recognitions_t& tower,
                            rapidxml::xml_node<> *seen_node,
                            std::set<std::string>& vehicles,
                            std::set<std::string>& timesteps) {
  //extract the attributes
  std::string vehicle_id = "-";
  double tower_x = 0.0;
  double tower_y = 0.0;

  int not_found = 2;

  for (rapidxml::xml_attribute<> *sn_attr = seen_node->first_attribute();
       sn_attr;
       sn_attr = sn_attr->next_attribute()) {

    if (strcmp(sn_attr->name(), ID_ATTR) == 0) {
      vehicle_id = std::string(sn_attr->value());
      not_found--;

    } else if (strcmp(sn_attr->name(), OBSERVER_POS_END_ATTR) == 0) {
      std::string tower_pos = std::string(sn_attr->value());
      //parse the tower position
      if (!parse_position(tower_pos, tower_x, tower_y)) {
        std::cerr << "ERR unable to parse tower position " << tower_pos << std::endl;
        return;
      }
      not_found--;
    }
  }

  if (not_found) {
    std::cerr << "ERR unable to find " << not_found << " (seen) recognition point attributes" << std::endl;
    return;
  }

  //track this vehicle by id
  vehicles.insert(vehicle_id);

  //track the tower position
  tower.set_position(tower_x, tower_y);

  //add recognition points
  for (rapidxml::xml_node<> *rp_node = seen_node->first_node(RECOGNITION_POINT_NODE);
       rp_node;
       rp_node = rp_node->next_sibling()) {
    std::string ts = "0.0";
    double v_x = 0.0;
    double v_y = 0.0;

    not_found = 2;
    //look through attributes
    for (rapidxml::xml_attribute<> *rp_attr = rp_node->first_attribute();
         rp_attr;
         rp_attr = rp_attr->next_attribute()) {

      if (strcmp(rp_attr->name(), T_ATTR) == 0) {
        ts = std::string(rp_attr->value());
        not_found--;

      } else if (strcmp(rp_attr->name(), SEEN_POS_ATTR) == 0) {
        std::string seen_pos = std::string(rp_attr->value());
        if (!parse_position(seen_pos, v_x, v_y)) {
          std::cerr << "ERR unable to parse vehicle position " << seen_pos << std::endl;
          return;
        }

        not_found--;
      }
    }

    if (not_found) {
      std::cerr << "ERR unable to find " << not_found << " recognition point attributes" << std::endl;
      return;
    }

    //record the timestep
    timesteps.insert(ts);

    //add the recognition point
    tower.add_recognition(
      std::move(ts),
      std::move(vehicle_id),
      distance(tower_x, tower_y, v_x, v_y)
    );
  }
}

/**
 * Add an edge to the lookup
 * @param edge_node   the xml node in the net file
 * @param edges       all edge ids in the network
 * @param edge_shapes the shapes of all edges
 */
void add_edge(rapidxml::xml_node<> *edge_node,
              std::set<std::string>& edges,
              std::unordered_map<std::string, std::unique_ptr<types::road_edge_t>>& edge_shapes) {

  //get the edge attributes
  for (rapidxml::xml_attribute<> *edge_attr = edge_node->first_attribute();
       edge_attr;
       edge_attr = edge_attr->next_attribute()) {
    //check the attribute name
    if ((strcmp(edge_attr->name(),  FUNCTION_ATTR) == 0) &&
        (strcmp(edge_attr->value(), INTERNAL_VAL) == 0)) {
      //ignore edges with internal function
      return;
    }
  }

  //get lanes
  for (rapidxml::xml_node<> *lane_node = edge_node->first_node(LANE_NODE);
       lane_node;
       lane_node = lane_node->next_sibling()) {

    std::string lane_id;
    std::vector<std::pair<double, double>> vertices;
    int not_found = 2;

    //look at attributes for lane
    for (rapidxml::xml_attribute<> *lane_attr = lane_node->first_attribute();
         lane_attr;
         lane_attr = lane_attr->next_attribute()) {

      if (strcmp(lane_attr->name(), ID_ATTR) == 0) {
        lane_id = std::string(lane_attr->value());
        not_found--;

      } else if (strcmp(lane_attr->name(), SHAPE_ATTR) == 0) {
        std::string shape = std::string(lane_attr->value());
        not_found--;

        //parse vertex pairs
        if (!parse_shape(shape, vertices)) {
          std::cerr << "ERR: failed to parse shape: " << shape << std::endl;
          return;
        }
      }
    }

    if (not_found > 0) {
      std::cerr << "ERR: lane missing id or shape" << std::endl;
      return;
    }

    //add to lookups
    edges.insert(lane_id);

    std::unique_ptr<types::road_edge_t> edge = std::make_unique<types::road_edge_t>();

    //add the parsed vertices
    for (size_t i=0; i<vertices.size(); i++) {
      edge->add_vertex(
        vertices.at(i).first,
        vertices.at(i).second
      );
    }

    //add to lookup
    edge_shapes.insert(std::make_pair(lane_id, std::move(edge)));
  }
}

/**
 * Add vehicles and their positions to the history lookup for a given timestep
 * @param  edge_node         the edge node
 * @param  timestep          the current simulation timestep
 * @param  vehicle_lane_hist the history lookup
 */
void add_vehicle_hist(rapidxml::xml_node<> *edge_node,
                      double timestep,
                      std::unordered_map<std::string,std::unique_ptr<types::vehicle_lane_hist_t>>& vehicle_lane_hist) {
  //get each lane
  for (rapidxml::xml_node<> *lane_node = edge_node->first_node(LANE_NODE);
       lane_node;
       lane_node = lane_node->next_sibling()) {
    //parse the lane id
    std::string lane_id;
    bool lane_id_found = false;

    for (rapidxml::xml_attribute<> *lane_attr = lane_node->first_attribute();
         lane_attr;
         lane_attr = lane_attr->next_attribute()) {
      //check the attribute name
      if (strcmp(lane_attr->name(), ID_ATTR) == 0) {
        lane_id = std::string(lane_attr->value());
        lane_id_found = true;
      }
    }

    if (!lane_id_found) {
      std::cerr << "ERR lane id not found" << std::endl;
      throw std::exception();
    }

    //get vehicles in the lane
    for (rapidxml::xml_node<> *vehicle_node = lane_node->first_node(VEHICLE_NODE);
         vehicle_node;
         vehicle_node = vehicle_node->next_sibling()) {
      //parse the id
      std::string vehicle_id;
      bool vehicle_id_found = false;

      for (rapidxml::xml_attribute<> *vehicle_attr = vehicle_node->first_attribute();
           vehicle_attr;
           vehicle_attr = vehicle_attr->next_attribute()) {
        //check the attribute name
        if (strcmp(vehicle_attr->name(), ID_ATTR) == 0) {
          vehicle_id = std::string(vehicle_attr->value());
          vehicle_id_found = true;
        }
      }

      if (!vehicle_id_found) {
        std::cerr << "ERR vehicle id not found" << std::endl;
        throw std::exception();
      }

      //check that this is not a tower
      if (vehicle_id.rfind(TOWER_PREFIX, 0) == std::string::npos) {
        //add the mapping
        std::unordered_map<std::string,std::unique_ptr<types::vehicle_lane_hist_t>>::iterator it = vehicle_lane_hist.find(vehicle_id);
        if (it != vehicle_lane_hist.end()) {
          it->second->at_segment(lane_id, timestep);
        } else {
          //add new
          vehicle_lane_hist.insert(std::make_pair(
            vehicle_id,
            std::make_unique<types::vehicle_lane_hist_t>(lane_id, timestep) //constructor w/ initial values
          ));
        }
      }
    }
  }
}

/**
 * Read the output files and generate an aggregated report
 * @param  bt_output_path       the path to the bluetooth output file
 * @param  net_input_path       the path to the sumo network input file (input to simulation)
 * @param  raw_output_path      the raw simulation output to follow vehicle progress
 * @param  output_path          the path to a folder to write output files to
 * @return                      success or failure
 */
int process_output_data(const std::string& bt_output_path,
                        const std::string& net_input_path,
                        const std::string& raw_output_path,
                        const std::string& output_path) {
  //construct a mapping from tower id to all recognition points
  std::unordered_map<std::string, std::unique_ptr<types::tower_recognitions_t>> tower_recognitions;
  //sets of tower, vehicle ids, timesteps
  std::set<std::string> towers;
  std::set<std::string> vehicles;
  std::set<std::string> timesteps;

  //load the bt xml file, add recognitions to map
  if (!load_from_path(bt_output_path, [&tower_recognitions, &towers, &vehicles, &timesteps] (const rapidxml::xml_document<>& doc) {

    //verify the name of the root node
    if (strcmp(doc.first_node()->name(), BT_OUTPUT_NODE) != 0) {
      std::cerr << "ERR doc root node not: " << BT_OUTPUT_NODE << std::endl;
      throw std::exception();
    }

    //get each tower
    for (rapidxml::xml_node<> *tower_node = doc.first_node(BT_OUTPUT_NODE)->first_node(BT_NODE);
         tower_node;
         tower_node = tower_node->next_sibling()) {

      //get the timestep
      for (rapidxml::xml_attribute<> *tower_attr = tower_node->first_attribute();
           tower_attr;
           tower_attr = tower_attr->next_attribute()) {
        //check the attribute name
        if (strcmp(tower_attr->name(), ID_ATTR) == 0) {
          std::string tower_id = std::string(tower_attr->value());

          towers.insert(tower_id);

          //create a tower
          types::tower_recognitions_t& tower = add_tower(std::move(tower_id), tower_recognitions);

          //add vehicle recognitions
          for (rapidxml::xml_node<> *seen_node = tower_node->first_node(SEEN_NODE);
               seen_node;
               seen_node = seen_node->next_sibling()) {

            //add recognition points for this tower
            add_recognition_points(tower, seen_node, vehicles, timesteps);
          }
        }
      }
    }

  })) {
    return EXIT_FAILURE;
  }

  //write the tower output
  int tower_output_stat = output::write_tower_output(output_path,
                                                     tower_recognitions,
                                                     vehicles,
                                                     timesteps);
  if (tower_output_stat != EXIT_SUCCESS) {
    std::cerr << "ERR: failed to write tower output, skipping remaining output artifacts" << std::endl;
    return tower_output_stat;
  }

  //clear unused storage
  vehicles.clear();

  //load network edges
  std::set<std::string> edges;
  //record the shapes of edges in the network
  std::unordered_map<std::string, std::unique_ptr<types::road_edge_t>> edge_shapes;

  //load the network xml file
  if (!load_from_path(net_input_path, [&edges, &edge_shapes] (const rapidxml::xml_document<>& doc) {
    //verify the name of the root node
    if (strcmp(doc.first_node()->name(), NET_NODE) != 0) {
      std::cerr << "ERR doc root node not: " << NET_NODE << std::endl;
      throw std::exception();
    }

    //get each edge
    for (rapidxml::xml_node<> *edge_node = doc.first_node(NET_NODE)->first_node(EDGE_NODE);
         edge_node;
         edge_node = edge_node->next_sibling()) {
      //add the edge
      add_edge(edge_node, edges, edge_shapes);
    }

  })) {
    return EXIT_FAILURE;
  }


  //write the tower coverage output
  int tower_coverage_output_stat = output::write_tower_coverage_output(output_path,
                                                                       tower_recognitions,
                                                                       edge_shapes,
                                                                       edges,
                                                                       towers);
  if (tower_coverage_output_stat != EXIT_SUCCESS) {
    std::cerr << "ERR: failed to write tower overage output, skipping remaining output artifacts" << std::endl;
    return tower_coverage_output_stat;
  }

  //clear unused storage
  edge_shapes.clear();
  towers.clear();

  //record the lanes seen by a given vehicle
  std::unordered_map<std::string,std::unique_ptr<types::vehicle_lane_hist_t>> vehicle_lane_hist;

  //load the raw output
  if (!load_from_path(raw_output_path, [&vehicle_lane_hist] (const rapidxml::xml_document<>& doc) {
    //verify the name of the root node
    if (strcmp(doc.first_node()->name(), NETSTATE_NODE) != 0) {
      std::cerr << "ERR doc root node not: " << NETSTATE_NODE << std::endl;
      throw std::exception();
    }

    //get each timestep
    for (rapidxml::xml_node<> *ts_node = doc.first_node(NETSTATE_NODE)->first_node(TIMESTEP_NODE);
         ts_node;
         ts_node = ts_node->next_sibling()) {
      double ts = 0.0;
      bool ts_found = false;

      //parse the timestep value
      for (rapidxml::xml_attribute<> *ts_attr = ts_node->first_attribute();
           ts_attr;
           ts_attr = ts_attr->next_attribute()) {
        //check the attribute name
        if (strcmp(ts_attr->name(), TIME_ATTR) == 0) {
          ts = atof(ts_attr->value());
          ts_found = true;
        }
      }

      if (!ts_found) {
        std::cerr << "ERR no timestep attribute" << std::endl;
        throw std::exception();
      }

      //get each edge
      for (rapidxml::xml_node<> *edge_node = ts_node->first_node(EDGE_NODE);
           edge_node;
           edge_node = edge_node->next_sibling()) {
        add_vehicle_hist(edge_node, ts, vehicle_lane_hist);
      }
    }

  })) {
    return EXIT_FAILURE;
  }


  //write the vehicle history output
  int vehicle_hist_output_stat = output::write_vehicle_output(output_path,
                                                              vehicle_lane_hist,
                                                              edges,
                                                              timesteps);
  if (vehicle_hist_output_stat != EXIT_SUCCESS) {
    std::cerr << "ERR: failed to write tower overage output, skipping remaining output artifacts" << std::endl;
    return tower_coverage_output_stat;
  }

  return EXIT_SUCCESS;
  //TODO remaining
}
