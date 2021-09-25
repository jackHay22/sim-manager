/*
 * Jack Hay, Sept 2021
 */

#ifndef _CSV_WRITER_H
#define _CSV_WRITER_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

namespace output {

  /*
   * Write lines of output to a csv file
   */
  struct csv_writer_t {
  private:
    //the output file
    std::ofstream out_file;

  public:
    /**
     * Open the output file
     * @param path path to the file to write to
     */
    csv_writer_t(const std::string& path);

    /**
     * Destructor closes file
     */
    ~csv_writer_t();

    //no copy
    csv_writer_t(const csv_writer_t&) = delete;
    csv_writer_t& operator=(const csv_writer_t&) = delete;

    /**
     * Write the header to the file
     * @param header the header
     */
    void write_header(const std::vector<std::string>& header);

    /**
     * Write a row
     * @param row the values in the row to write
     */
    void write_row(const std::vector<double>& row);
  };
}

#endif /*_CSV_WRITER_H*/
