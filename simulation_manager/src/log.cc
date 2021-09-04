/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#include "log.h"
#include <exception>

namespace simulation_manager {
namespace log {

  /**
   * Log information
   * @param message the message to log
   */
  void info(const std::string& message) {
    std::cerr << "[simulation_manager] INFO: " << message << std::endl;
  }

  /**
   * Log an error
   * @param message the error message
   */
  void err(const std::string& message) {
    std::cerr << "[simulation_manager] ERR: " << message << std::endl;
    //throw exception to terminate
    throw std::exception();
  }
}}
