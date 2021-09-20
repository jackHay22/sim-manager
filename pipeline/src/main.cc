/*
 * Jack Hay, Sept 2021
 */

#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <json.hpp>
#include <fstream>
#include <vector>

typedef nlohmann::json json_t;

//defines the configuration for a
//simulation process
struct cfg_t {
  //path to network file
  std::string network_path;
  //path to routes file
  std::string routes_path;
  //path to towers file
  std::string towers_path;
  //path to write bluetooth output to
  std::string output_path;
};

/**
 * Deserialize configuration
 * @param j the json configuration
 * @param c the configuration to pass to a pipeline process
 */
void from_json(const json_t& j, cfg_t& c) {
  j.at("network_path").get_to(c.network_path);
  j.at("routes_path").get_to(c.routes_path);
  j.at("towers_path").get_to(c.towers_path);
  j.at("output_path").get_to(c.output_path);
}

/**
 * Run the simulation on a given configuration, perform
 * analysis
 * @param cfg the configuration
 */
void run_simulation(const cfg_t& /*cfg*/) {

}

/**
 * Run analysis pipeline
 */
int main(int argc, char **argv) {
  int c;
  //the path to the configuration file
  std::string cfg_path;
  while ((c = getopt(argc, argv, "c:")) != -1) {
    if (c != 'c') {
      std::cerr << "invalid option: " + c << std::endl;
      return 1;
    } else {
      cfg_path = std::string(optarg);
    }
  }

  std::vector<cfg_t> cfgs;

  //load
  try {
    //load the config file
    std::ifstream in_stream(cfg_path);
    nlohmann::json config;

    in_stream >> config;
    cfgs = config.get<std::vector<cfg_t>>();

  } catch (...) {
    std::cerr << "failed to load pipeline configuration: " + cfg_path << std::endl;
    return EXIT_FAILURE;
  }


  //fork a new process for each configuration
  for (size_t i=0; i<cfgs.size(); i++) {
    pid_t pid = fork();

    if (pid < 0) {
      std::cerr << "Fork failed" << std::endl;
      return EXIT_FAILURE;
      
    } else if (pid == 0) {
      run_simulation(cfgs.at(i));
      return EXIT_SUCCESS;
    }
  }

  //wait for children to finish
  for (size_t i=0; i<cfgs.size(); i++) {
    int status;
    wait(&status);
  }

  return EXIT_SUCCESS;
}
