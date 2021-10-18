import logging
import sys
import os
import argparse
import math

#Verify that the SUMO_HOME environment var is set to we can load the python lib
if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib

log = logging.getLogger()
log.setLevel(logging.DEBUG)
stream = logging.StreamHandler(sys.stderr)
formatter = logging.Formatter('[tower_placer] %(asctime)s %(message)s')
stream.setFormatter(formatter)
stream.setLevel(logging.DEBUG)
log.addHandler(stream)

TOWER_POSITION_OPTIONS = ["center", "junction"]

class Vec2:
    def __init__(x, y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return Vec2(self.x + other.x,
                    self.y + other.y)

    def __sub__(self, other):
        return Vec2(self.x - other.x,
                    self.y - other.y)

    def __mul__(self, s):
        return Vec2(self.x * s, self.y * s)

    def dist(self, other):
        return math.sqrt((other.x - self.x) ** 2 +
                         (other.y - self.y) ** 2)

    def dot(self, other):
        return self.x * other.x + self.y * other.y


def distance(pt, line):
    """Get the distance between a point and a line segment
    @param pt   Vec2
    @param line (Vec2, Vec2)"""
    l0 = line[0]
    l1 = line[1]
    #get segment distance squared
    dist_sqrd = ((l1.x - l0.x) ** 2 + (l1.y - l0.y) ** 2)

    if dist_sqrd == 0:
        #line seqment has no length
        return pt.dist(l0)

    #calculate projection
    proj = l0 + (l1 - l0) * max(0, min(1, (pt - l0).dot(l1 - l0) / dist_sqrd))
    return pt.dist(proj)

class Lane:
    def __init__(self, s_lane, parent_edge):
        self._id = s_lane.id
        self._pts = []
        self._len = float(s_lane.length)
        self._parent = parent_edge

        #add points that make up lane shape
        pairs = s_lane.shape.split(' ')
        for p in pairs:
            comps = p.split(',')
            self._pts.append(Vec2(float(comps[0]),
                                  float(comps[1])))

    def min_dist(self, pt):
        """Get the min distance from this lane to a point"""
        closest = sys.float_info.max
        for p in self._pts:
            d = p.dist(pt)
            if d < closest:
                closest = d
        return closest

    def get_edge_id(self):
        """Get the edge id of the lane's parent"""
        return self._parent._id

class Edge:
    def __init__(self, s_edge):
        """constructed with a sumo edge"""
        self._id = s_edge.id
        self._lanes = []
        self._min_x = None
        self._max_x = None
        self._min_y = None
        self._max_y = None

        #add lanes
        for lane in s_edge.lane:
            self._lanes.append(Lane(lane, self))

    def range_x(self):
        """Get the (min,max) range for the edge (x)"""
        if (self._min_x != None) and (self._max_x != None):
            return self._min_x, self._max_x
        min_x = sys.float_info.max
        max_x = sys.float_info.min
        for l in self._lanes:
            for p in l._pts:
                if p[0] < min_x:
                    min_x = p[0]
                if p[0] > max_x:
                    max_x = p[0]
        self._min_x = min_x
        self._max_x = max_x
        return min_x, max_x

    def range_y(self):
        """Get the (min,max) range for the edge (y)"""
        if (self._min_y != None) and (self._max_y != None):
            return self._min_y, self._max_y
        min_y = sys.float_info.max
        max_y = sys.float_info.min
        for l in self._lanes:
            for p in l._pts:
                if p[1] < min_y:
                    min_y = p[1]
                if p[1] > max:
                    max_y = p[1]
        self._min_y = min_y
        self._max_y = max_y
        return min_y, max_y

    def min_dist(self, pt):
        """Get the distance from this edge to a point (plus lane)"""
        closest = sys.float_info.max
        lane = None
        for l in self._lanes:
            d = l.min_dist(pt)
            if d < closest:
                closest = d
                lane = l
        return closest, l

class Tower:
    def __init__(self, lane, tower_num):
        """Tower constructor takes the lane to construct the tower on"""
        self._tower_id = str(tower_num)
        self._lane_id = lane._id
        self._end_pos = lane._len / 2
        self._edge_id = lane.get_edge_id()

    def generate_xml(self):
        """Generate an xml representation for this tower"""
        #generate an xml representation for this tower
        #For example:
        # <parkingArea id="<tower_id>_PA" lane="<lane_id>" endPos="10" roadsideCapacity="1"/>
        # <vehicle id="<tower_id>" depart="0.00" departPos="stop">
        #     <route edges="<edge_id>" color="cyan"/>
        #     <stop parkingArea="<tower_id>_PA" parking="true"/>
        #     <param key="has.btreiver.device" value="true"/>
        # </vehicle>
        pass

def place_towers_grid(edges, tower_count, placement_threshold):
    """Superimpose a grid of towers on a roadway"""
    range_x = (sys.float_info.max,sys.float_info.min)
    range_y = (sys.float_info.max,sys.float_info.min)
    #determine the bounds of the network
    for e in edges:
        (e_xmin, e_xmax) = e.range_x()
        (e_ymin, e_ymax) = e.range_y()
        range_x = (min(e_xmin, range_x[0]), max(e_xmax, range_x[1]))
        range_y = (min(e_ymin, range_y[0]), max(e_ymax, range_y[1]))

    logging.info("network ranges from (" +
                 str(range_x[0]) + ", " +
                 str(range_y[0]) + ") to (" +
                 str(range_x[1]) + ", " +
                 str(range_y[1]) + ")")

    len_x = range_x[1] - range_x[0]
    len_y = range_y[1] - range_y[0]

    #get the ratio of width to height to determine rows vs cols
    r = len_x / len_y
    rows = math.sqrt(tower_count / r)
    cols = rows * r

    incr_x = len_x / cols
    incr_y = len_y / rows

    towers = []
    for i in range(rows):
        for j in range(cols):
            p = Vec2(range_x + (j * incr_x),
                     range_y + (i * incr_y))
            closest_dist = sys.float_info.max
            closest_lane = None
            #find the edge closest to the tower
            for e in edges:
                d, lane = e.min_dist(p)
                if d < closest_dist:
                    closest_dist = d
                    closest_lane = lane

            #check if the closest edge is within the threshold
            if closest_dist < placement_threshold:
                #create the tower next to the closest lane
                towers.append(Tower(closest_lane, len(towers)))
    return towers

def write_towers(towers, out_file):
    """Write the new towers to an exl file (path to file should be out_file)"""
    pass

def main(net_file, out_file, tower_count, placement_threshold):
    """Takes the network file to position towers for and the number of towers to place"""
    logging.info("placing " + str(tower_count) + " towers for network " + net_file)

    #get edges in the network, filter for normal
    edges = []
    for edge in sumolib.xml.parse(net_file, ['edge']):
        if edge.function == None:
            edges.append(Edge(edge))

    logging.info("found " + str(len(edges)) + " normal edges for network")

    #Position towers based on normal edge position
    write_towers(place_towers_grid(edges, tower_count, placement_threshold), out_file)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--net', type=str, required=True)
    parser.add_argument('--out', type=str, required=True)
    parser.add_argument('--count', type=int, required=True)
    #placement threshold: maximum allowed distance from ideal tower position to assigned edge
    parser.add_argument('--threshold', type=int, required=True)
    args = parser.parse_args()

    #call main
    main(args.net, args.out, args.count, args.threshold)
