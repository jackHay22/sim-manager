import logging
import sys
import os
import argparse

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

class Lane:
    def __init__(self, s_lane):
        self._id = s_lane.id
        self._pts = []

        #add points that make up lane shape
        pairs = s_lane.shape.split(' ')
        for p in pairs:
            comps = p.split(',')
            self._pts.append((float(comps[0]),
                              float(comps[1])))

class Edge:
    def __init__(self, s_edge):
        """constructed with a sumo edge"""
        self._id = s_edge.id
        self._lanes = []

        #add lanes
        for lane in s_edge.lane:
            self._lanes.append(Lane(lane))

class Tower:
    def __init__(self, towerId, edgeId, laneId):
        """Tower constructor takes id to assign, id of edge
            to attach to, id of lane within edge"""
        self._id = towerId
        self._edgeId = edgeId
        self._laneId = laneId

    def generate_xml(self):
        #generate an xml representation for this tower
        pass

def place_towers_grid(edges, tower_count, tower_pos):
    """Place some number of towers in a grid for the given roadway
    as represented by the edges. Returns towers to write"""
    if tower_pos == 'center':
        logging.info("centering towers in lanes")
    elif tower_pos == "junction":
        logging.info("placing towers at junctions")
    towers = []
    return towers

def write_towers(towers, out_file):
    """Write the new towers to an exl file (path to file should be out_file)"""
    pass

def main(net_file, out_file, placement_type, tower_count, tower_pos):
    """Takes the network file to position towers for, the placement type,
    the number of towers to place"""
    #Check the placement type
    if placement_type != 'grid':
        logging.error("invalid placement type: " + placement_type)
        return

    logging.info("placing " + str(tower_count) +
                 " towers for network " + net_file +
                 " as a " + placement_type)

    #get edges in the network, filter for normal
    edges = []
    for edge in sumolib.xml.parse(net_file, ['edge']):
        if edge.function == None:
            edges.append(Edge(edge))

    logging.info("found " + str(len(edges)) + " normal edges")

    #Position towers based on normal edge position
    write_towers(place_towers_grid(edges, tower_count, tower_pos), out_file)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--net', type=str, required=True)
    parser.add_argument('--out', type=str, required=True)
    parser.add_argument('--type', type=str, required=True)
    parser.add_argument('--count', type=int, required=True)
    #position of tower within lane
    parser.add_argument('--tpos', type=str, default="center")
    args = parser.parse_args()

    if args.tpos not in TOWER_POSITION_OPTIONS:
        logging.error("invalid tower position argument: " + args.tpos)
        exit(1)

    #call main
    main(args.net, args.out, args.type, args.count, args.tpos)
