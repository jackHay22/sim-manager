/*
 * Simulation manager
 *
 * Written by Jack Hay, Sept 2021
 */

#ifndef _SIMULATION_MANAGER_LOG_H
#define _SIMULATION_MANAGER_LOG_H

#include <string>
#include <iostream>

namespace simulation_manager {
namespace log {

  /**
   * Log information
   * @param message the message to log
   */
  void info(const std::string& message);

  /**
   * Log an error, also throws exception
   * @param message the error message
   */
  void err(const std::string& message);
}}

#endif /*_SIMULATION_MANAGER_LOG_H*/
