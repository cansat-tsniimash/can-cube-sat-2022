from collections import namedtuple
import csv
from shapely.geometry import Polygon, LineString, Point as ShapelyPoint
import pyproj

INPUT = '../GPS_UBX_NAV_SOL-11-0.csv'
OUTPUT = 'flat.csv'

Point = namedtuple("Point", ["lon", "lat", "height"])


transformer = pyproj.Transformer.from_crs('EPSG:4326','EPSG:32637') # EPSG:32637 -- UTM 37N

def distance(left, right):
    left_merc = ShapelyPoint(transformer.transform(left.x, left.y, left.z))
    right_merc = ShapelyPoint(transformer.transform(right.x, right.y, right.z))
    distance = left_merc.distance(right_merc)
    return distance

points = []
records = []

with open(INPUT, mode="r") as stream:
    reader = csv.DictReader(stream)
    for line in reader:
        lon, lat, height = line["lon"], line["lat"], line["h"]
        point = Point(float(lon), float(lat), float(height))
        points.append(point)
        records.append(line)


good_lines = []
good_records = []
prev_point = ShapelyPoint(points[0])

current_line = []
current_block = []
for no, cur_point in enumerate([ShapelyPoint(x) for x in points[1:]]):
    d = distance(cur_point, prev_point)
    record = records[no]

    if d > 1000000000000000:
        print("break at %s" % d)
        good_lines.append(LineString(current_line))
        good_records.append(current_block)
        current_line = [cur_point]
        current_block = [record]
    else:
        current_line.append(cur_point)
        current_block.append(record)

    prev_point = cur_point


if current_line:
    good_lines.append(LineString(current_line))
    good_records.append(current_block)


for no, block in enumerate(good_records):
    with open(str(no) + "_" + OUTPUT, mode="w") as outstream:
        writer = csv.DictWriter(outstream, fieldnames=records[0].keys())
        writer.writeheader()
        for line in block:
            writer.writerow(line)
