from collections import namedtuple
import csv
import fastkml
from fastkml import kml
from shapely.geometry import Polygon, LineString, Point as ShapelyPoint
import pyproj

INPUT = '../GPS_UBX_NAV_SOL-11-0.csv'
OUTPUT = 'GPS_UBX_NAV_SOL-11-0.kml'

Point = namedtuple("Point", ["lon", "lat", "height"])


transformer = pyproj.Transformer.from_crs('EPSG:4326','EPSG:32637') # EPSG:32637 -- UTM 37N

def distance(left, right):
    left_merc = ShapelyPoint(transformer.transform(left.x, left.y, left.z))
    right_merc = ShapelyPoint(transformer.transform(right.x, right.y, right.z))
    distance = left_merc.distance(right_merc)
    return distance


with open(INPUT, mode="r") as stream:
    points = []
    reader = csv.DictReader(stream)
    for line in reader:
        lon, lat, height = line["lon"], line["lat"], line["h"]
        point = Point(float(lon), float(lat), float(height))
        points.append(point)


polygons = []
prev_point = points[0]
for cur_point in points[1:]:
    polygon = [
        prev_point,
        cur_point,
        Point(cur_point.lon, cur_point.lat, 0),
        Point(prev_point.lon, prev_point.lat, 0),
        prev_point,
    ]

    polygons.append(polygon)
    prev_point = cur_point


k = kml.KML()
ns = '{http://www.opengis.net/kml/2.2}'

shadow_style = fastkml.styles.PolyStyle(ns=ns, color='ccccccaa', outline=0)
good_line_style = fastkml.styles.LineStyle(ns=ns, color='ff0000ff', width=10)
bad_line_style = fastkml.styles.LineStyle(ns=ns, color='0000ffff', width=10)
styles = [
    fastkml.styles.Style(ns=ns, id='style-shadow', styles=[shadow_style]),
    fastkml.styles.Style(ns=ns, id='style-good-line', styles=[good_line_style]),
    fastkml.styles.Style(ns=ns, id="style-bad-line", styles=[bad_line_style])
]

name = 'Траектория полёта "СНЕРГИРЬ МС"'
description = "Траектория полёта СНЕРГИРЬ МС в стратосферном запуске 04.07.2022"
document = kml.Document(ns, 'id-bullfinch-trajectory', name, description, styles=styles)
k.append(document)

shadow_folder = kml.Folder(ns, 'id-shadow', 'Проекция траектории', '')
good_line_folder = kml.Folder(ns, 'id-good-line', 'Достоверная траектория', 'Корректные точки')
bad_line_folder = kml.Folder(ns, 'id-bad-line', 'Отсутствующие фрагменты', 'В этих зонах координат не было')
document.append(shadow_folder)
document.append(good_line_folder)
document.append(bad_line_folder)

for no, polygon in enumerate(polygons):
    p = kml.Placemark(ns, f'id-shadow-{no}', 'Проекция траектории', '', styleUrl='#style-shadow')
    p.geometry = fastkml.geometry.Geometry(
        ns=ns,
        geometry=Polygon(polygon),
        altitude_mode='relativeToGround',
    )
    shadow_folder.append(p)


good_lines = []
prev_point = ShapelyPoint(points[0])
current_line = []
for cur_point in [ShapelyPoint(x) for x in points[1:]]:
    d = distance(cur_point, prev_point)

    if d > 200:
        print("break at %s" % d)
        good_lines.append(LineString(current_line))
        current_line = [cur_point]
    else:
        current_line.append(cur_point)

    prev_point = cur_point


if current_line:
    good_lines.append(LineString(current_line))


for no, line in enumerate(good_lines):
    placemark = kml.Placemark(ns, styleUrl='#style-good-line')
    placemark.geometry = fastkml.geometry.Geometry(
        ns=ns,
        geometry=LineString(line),
        altitude_mode='relativeToGround'
    )
    good_line_folder.append(placemark)


with open(OUTPUT, mode="w") as outstream:
    outstream.write(k.to_string(prettyprint=True))
