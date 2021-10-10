/*
 * Jack Hay, Sept 2021
 */

#ifndef _PROCESS_H
#define _PROCESS_H

#include <string>

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
                        const std::string& output_path);

#endif /*_PROCESS_H*/
