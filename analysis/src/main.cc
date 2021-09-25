/*
 * Jack Hay, Sept 2021
 */

#include <string>
#include <unistd.h>
#include "process.h"

/**
 * Run analysis pipeline
 */
int main(int argc, char **argv) {
  int c;
  //the path to the bluetooth output file
  std::string bt_output_path;
  //the location to write the output to
  std::string output_path;

  while ((c = getopt(argc, argv, "b:")) != -1) {
    if (c == 'b') {
      bt_output_path = std::string(optarg);
    }
  }

  return process_output_data(bt_output_path,
                             output_path);
}
