/*
 * Jack Hay, Sept 2021
 */

#include "render_output.h"
#include "csv_writer.h"

namespace output {

  /**
   * Transform and write output to a file
   * @param out_path           the path to write to
   * @param tower_recognitions all recognitions from the simulation
   * @return success or failure
   */
  int render_output(const std::string& out_path,
                     std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>& tower_recognitions) {

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
