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
     * Get the distance from the edge to some point
     * @param  x      position x
     * @param  y      position y
     * @return        the distance to this edge
     */
    double distance(double x, double y) const;
  };
}

#endif /*_ROAD_EDGE_H*/
