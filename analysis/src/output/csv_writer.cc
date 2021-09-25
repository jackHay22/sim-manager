/*
 * Jack Hay, Sept 2021
 */

#include "csv_writer.h"
#include <exception>

namespace output {

  #define COMMA ","

  /**
   * Open the output file
   * @param path path to the file to write to
   */
  csv_writer_t::csv_writer_t(const std::string& path) {
    this->out_file = std::ofstream(path);
    if (!this->out_file.is_open()) {
      std::cerr << "Err unable to open output file: " << path << std::endl;
      throw std::exception();
    }
  }

  /**
   * Destructor closes file
   */
  csv_writer_t::~csv_writer_t() {
    this->out_file.close();
  }

  /**
   * Write the header to the file
   * @param header the header
   */
  void csv_writer_t::write_header(const std::vector<std::string>& header) {
    for (size_t i=0; i<header.size(); i++) {
      this->out_file << header.at(i);
      if (i < (header.size() - 1)) {
        this->out_file << COMMA;
      }
    }
    this->out_file << "\n";
  }

  /**
   * Write a row
   * @param row the values in the row to write
   */
  void csv_writer_t::write_row(const std::vector<double>& row) {
    for (size_t i=0; i<row.size(); i++) {
      this->out_file << row.at(i);
      if (i < (row.size() - 1)) {
        this->out_file << COMMA;
      }
    }
    this->out_file << "\n";
  }
}
