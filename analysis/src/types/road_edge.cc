/*
 * Jack Hay, Sept 2021
 */

#include "road_edge.h"
#include <cmath>

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
   * Check if some other point is within a radius of all roadway points
   * @param  pos    the position to check
   * @param  radius the radius to check against
   * @return        whether this point is closer than radius to all vertices
   */
  bool in_range(const std::pair<double,double>& pos, double radius) const {
    for (size_t i=0; i<this->vertices.size(); i++) {
      double d = sqrt(pow(this->vertices.at(i).first - pos.first, 2) +
                      pow(this->vertices.at(i).second - pos.second, 2));
      if (d > radius) {
        return false;
      }
    }
    return true;
  }
}
