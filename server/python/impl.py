import json

class VehicleHistory:
    def __init__(self, json=None):
        self._elapsed = 0
        self._segment_id = ""
        self._downloaded = False
        if json != None:
            self.from_json(json)

    def from_json(self, json):
        self._elapsed = json['elapsed']
        self._segment_id = json['id']
        self._downloaded = json['downloaded']

    def get_data(self):
        return {
            "elapsed" : self._elapsed,
            "id" : self._segment_id,
            "downloaded" : self._downloaded
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
        self._dist = json['dist']
        for h in json['hist']:
            self._hist.append(VehicleHistory(h))

    def mark_downloaded(self, segment_id):
        for h in self._hist:
            if h._segment_id == segment_id:
                h._downloaded = True

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
        self._downloaded = []

    def forward(self, server, data, segment_ids):
        """Take vehicle data, mark as downloaded, and forward"""
        #remove undownloaded segment ids
        #TODO
        self._to_forward.append((server,data))

    def download_segments(self, vehicle_data, segment_ids):
        """Mark segments for a vehicle as downloaded, add to buffer,
        also notifies the segment provider to mark this segment as downloaded"""
        #TODO remove undownloaded segment ids
        for s in segment_ids:
            vehicle_data.mark_downloaded(s)
            self._downloaded.append((vehicle_data._id, s))

        #add to the buffer
        self._buffer.append(vehicle_data)

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

        downloaded = []
        for d in self._downloaded:
            downloaded.append({
                "vehicle_id" : d[0],
                "segment_id" : d[1]
            })

        return json.dumps({
            "to_forward" : to_forward_data,
            "buffer" : buffer_data,
            "downloaded" : downloaded
        })

def impl(segments, vehicle_coverage, buffer):
    """Implementation of the algorithm"""
    result = Result()

    #look through vehicles in range
    for v in vehicle_coverage:
        to_download = []
        #look through segment information for vehicle
        for s in v._hist:
            #only consider segments that haven't been previously downloaded
            if not s._downloaded:
                #check if tower responsible for this segment
                if s._segment_id in segments:
                    #download, add to buffer
                    to_download.append(s._segment_id)
                else:
                    pass
                    #forward to peer
                    #TODO based on segment id?
        result.download_segments(v, to_download)

    return result

def main(segments_s, vehicle_coverage_s, buffer_s):
    """Function called by the server. The server passes
    in a list of segment ids that this tower is responsible for,
    the vehicles currently in range of the tower and their
    respective segment history, and the buffer of segments
    forwarded to the tower"""

    #list of segments this tower is responsible for
    segments = json.loads(segments_s)
    if segments == None:
        segments = []

    #vehicles in range of this tower
    vehicle_coverage = []
    coverage_json = json.loads(vehicle_coverage_s)
    for c in coverage_json:
        vehicle_coverage.append(VehicleData(c))
    #vehicle data storage and forwarded data
    buffer = []
    buffer_json = json.loads(buffer_s)
    for b in buffer_json:
        buffer.append(VehicleData(b))

    return impl(segments, vehicle_coverage, buffer).to_json()
