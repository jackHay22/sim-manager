/*
 * Jack Hay, Sept 2021
 */

#ifndef _RENDER_OUTPUT_H
#define _RENDER_OUTPUT_H

#include <string>
#include <unordered_map>
#include "../recognitions/tower_recognitions.h"
#include <memory>

namespace output {

  /**
   * Transform and write output to a file
   * @param out_path           the path to write to
   * @param tower_recognitions all recognitions from the simulation
   * @return success or failure
   */
  int render_output(const std::string& out_path,
                     std::unordered_map<std::string, std::unique_ptr<recognitions::tower_recognitions_t>>& tower_recognitions);
}

#endif /*_RENDER_OUTPUT_H*/
