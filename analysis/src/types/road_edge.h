/*
 * Jack Hay, Sept 2021
 */

#ifndef _ROAD_EDGE_H
#define _ROAD_EDGE_H

#include <string>
#include <memory>
#include <vector>
#include <utility>

namespace types {
  /*
   * Defines some discrete edge of the road network made up of various vertices
   */
  struct road_edge_t {
  private:
    //the vertices of the edge (defines edge shape)
    std::vector<std::pair<double,double>> vertices;

  public:
    /**
     * Constructor
     */
    road_edge_t();

    //no copy
    road_edge_t(const road_edge_t&) = delete;
    road_edge_t& operator=(const road_edge_t&) = delete;

    /**
     * Add a vertex for this edge
     * @param x vertex position x
     * @param y vertex position y
     */
    void add_vertex(double x, double y);

    /**
     * Check if some other point is within a radius of all roadway points
     * @param  x      position x
     * @param  y      position y
     * @param  radius the radius to check against
     * @return        whether this point is closer than radius to all vertices
     */
    bool in_range(double x, double y, double radius) const;
  };
}

#endif /*_ROAD_EDGE_H*/
