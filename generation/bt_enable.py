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
formatter = logging.Formatter('[bt_enable] %(asctime)s %(message)s')
stream.setFormatter(formatter)
stream.setLevel(logging.DEBUG)
log.addHandler(stream)

def main(routes_path, send=True, recv=False):
    """Take path to routes file, edit all vehicles and enable bluetooth comms"""
    logging.info("enabling bluetooth for vehicles in " + routes_path)

    routes = []
    vehicles = 0
    for r in sumolib.xml.parse(routes_path, "routes"):
        #update vehicles in route
        for v in r.vehicle:
            if send:
                v.addChild("param", {"key": "has.btsender.device", "value": "true"})
            if recv:
                v.addChild("param", {"key": "has.btreceiver.device", "value": "true"})
            vehicles += 1
        routes.append(r)

    with open(routes_path, "w") as out:
        for r in routes:
            out.write(r.toXML())

    logging.info("wrote to " + str(len(routes)) + " routes, " + str(vehicles) + " vehicles")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--routes', type=str, required=True)
    args = parser.parse_args()
    #call main
    main(args.routes)
