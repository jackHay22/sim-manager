/*
 * Jack Hay, Sept 2021
 */

#include "road_edge.h"
#include <cmath>
#include <limits>

namespace types {
  /**
   * Constructor
   * @param edge_id the id of the edge
   */
  road_edge_t::road_edge_t() : vertices() {}

  /**
   * Add a vertex for this edge
   * @param x vertex position x
   * @param y vertex position y
   */
  void road_edge_t::add_vertex(double x, double y) {
    this->vertices.emplace_back(std::make_pair(x,y));
  }

  /**
   * Get the distance from the edge to some point
   * @param  x      position x
   * @param  y      position y
   * @return        the distance to this edge
   */
  double road_edge_t::distance(double x, double y) const {
    double min = std::numeric_limits<double>::max();

    for (size_t i=0; i<this->vertices.size(); i++) {
      double d = sqrt(pow(this->vertices.at(i).first - x, 2) +
                      pow(this->vertices.at(i).second - y, 2));
      if (d < min) {
        min = d;
      }
    }
    return min;
  }
}
