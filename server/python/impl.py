import json

class VehicleHistory:
    def __init__(self, json=None):
        self._elapsed = 0
        self._segment_id = ""
        if json != None:
            self.from_json(json)

    def from_json(self, json):
        self._elapsed = json['elapsed']
        self._segment_id = json['id']

    def get_data(self):
        return {
            "elapsed" : self._elapsed,
            "id" : self._segment_id
        }

class VehicleData:
    def __init__(self, json=None):
        self._id = ""
        self._dist = 0.0
        #list of VehicleHistory
        self._hist = []
        if json != None:
            self.from_json(json)

    def from_json(self, json):
        self._id = json['id']
        self._dist = json[dist]
        for h in json['hist']:
            self._hist.append(VehicleHistory(h))

    def get_data(self):
        vehicle_hist = []
        for v in self._hist:
            vehicle_hist.append(v.get_data())
        return {
            "id" : self._id,
            "dist" : self._dist,
            "hist" : vehicle_hist
        }

class Result:
    """Contains segments to forward, updated buffer"""
    def __init__(self):
        self._buffer = []
        self._to_forward = []

    def forward(server, data):
        self._to_forward.append((server,data))

    def to_json(self):
        to_forward_data = []
        for f in self._to_forward:
            to_forward_data.append({
                "tower_id" : f[0],
                "data" : f[1].get_data()
            })

        buffer_data = []
        for b in self._buffer:
            buffer_data.append(b.get_data())

        return json.dumps({
            "to_forward" : to_forward_data,
            "buffer" : buffer_data
        })

def main(segments_s, vehicle_coverage_s, buffer_s):
    """Function called by the server. The server passes
    in a list of segment ids that this tower is responsible for,
    the vehicles currently in range of the tower and their
    respective segment history, and the buffer of segments
    forwarded to the tower"""
    result = Result()

    segments = json.loads(segments_s)
    vehicle_coverage = json.loads(vehicle_coverage_s)
    buffer = json.loads(buffer_s)

    return result.to_json()
