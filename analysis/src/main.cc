/*
 * Jack Hay, Sept 2021
 */

#include <string>
#include <unistd.h>
#include <iostream>
#include "process.h"
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Check if the file exists for the given path
 * @param  path path to the file
 * @return      whether the file exists
 */
inline bool is_file(const std::string& path) {
  struct stat f_info;
  return (stat(path.c_str(), &f_info) == 0) &&
         !(f_info.st_mode & S_IFDIR);
}

/**
 * Check if the folder exists for the given path
 * @param  path path to the folder
 * @return      whether the folder exists
 */
inline bool is_dir(const std::string& path) {
  struct stat d_info;
  return (stat(path.c_str(), &d_info) == 0) &&
         (d_info.st_mode & S_IFDIR);
}

/**
 * Run analysis pipeline
 */
int main(int argc, char **argv) {
  int c;
  //the path to the bluetooth output file
  std::string bt_output_path;
  //the path to the fcd output file
  std::string fcd_output_path;
  //the location to write the output to
  std::string output_path;
  //the coverage radius
  double radius = -1;

  while ((c = getopt(argc, argv, "b:o:f:r:")) != -1) {
    if (c == 'b') {
      bt_output_path = std::string(optarg);
    } else if (c == 'o') {
      output_path = std::string(optarg);
    } else if (c == 'f') {
      fcd_output_path = std::string(optarg);
    } else if (c == 'r') {
      radius = atof(optarg);
    }
  }

  //validate arguments
  if (!is_file(bt_output_path)) {
    std::cerr << "ERR: bt output file does not exist: " << bt_output_path << std::endl;
    return EXIT_FAILURE;
  }

  if (!is_file(fcd_output_path)) {
    std::cerr << "ERR: fcd output file does not exist: " << fcd_output_path << std::endl;
    return EXIT_FAILURE;
  }

  if (!is_dir(output_path)) {
    std::cerr << "ERR: output directory does not exist: " << output_path << std::endl;
    return EXIT_FAILURE;
  }

  if (radius < 0) {
    std::cerr << "ERR: coverage radius not specified: " << radius << std::endl;
    return EXIT_FAILURE;
  }

  //process the data and write to output file
  return process_output_data(bt_output_path, fcd_output_path, output_path, radius);
}
