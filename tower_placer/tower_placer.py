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

def main(net_file, out_file, tower_count):
    """Takes the network file to position towers for, the number of towers to place"""
    #get edges in the network
    edges = 0
    for edge in sumolib.xml.parse(net_file, ['edge']):
        edges += 1
    logging.info("found " + str(edges) + " edges")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--net', type=str, required=True)
    parser.add_argument('--out', type=str, required=True)
    parser.add_argument('--count', type=int, required=True)
    args = parser.parse_args()

    logging.info("placing " + str(args.count) + " towers for network " + args.net)

    #call main
    main(args.net, args.out, args.count)
