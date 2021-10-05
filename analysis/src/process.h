/*
 * Jack Hay, Sept 2021
 */

#ifndef _PROCESS_H
#define _PROCESS_H

#include <string>

/**
 * Read the output files and generate an aggregated report
 * @param  bt_output_path       the path to the bluetooth output file
 * @param  fcd_output_path      the path to the sumo fcd output file
 * @param  output_path          the path to a folder to write output files to
 * @param  radius               the coverage radius
 * @return                      success or failure
 */
int process_output_data(const std::string& bt_output_path,
                        const std::string& fcd_output_path,
                        const std::string& output_path,
                        double radius);

#endif /*_PROCESS_H*/
