import pandas as pd
import shapely
from shapely.geometry import Polygon, LineString, Point
import pyproj


transformer = pyproj.Transformer.from_crs('EPSG:4326','EPSG:32637') # EPSG:32637 -- UTM 37N
station_loc = Point(40.051025390625, 56.6226081848145, 143)
station_loc = Point(transformer.transform(station_loc.x, station_loc.y, station_loc.z))

def distance(row):
	# print(row)
	sc_loc = Point(row["lon"], row["lat"], row["h"])
	sc_loc = Point(transformer.transform(sc_loc.x, sc_loc.y, sc_loc.z))
	distance = station_loc.distance(sc_loc)
	return distance


ubx_source = "../its-broker-log-20220704T041957-downlink_mav_csv/GPS_UBX_NAV_SOL-11-0.csv"
rssi_source = "../its-broker-log-20220704T041957_mdt/radio.rssi_packet.csv"
dest = "dist-a+GPS_UBX_NAV_SOL-11-0.csv"

ubx_df = pd.read_csv(ubx_source)
ubx_df["distance"] = ubx_df.apply(distance, axis=1)
ubx_df["time"] = ubx_df["time_s"] + ubx_df["time_us"] / 1000_000

rssi_df = pd.read_csv(rssi_source)
rssi_df["time"] = rssi_df["time_s"] + rssi_df["time_us"] / 1000_000
rssi_df["rssi_pkt_smooth"] = rssi_df["rssi_pkt"].rolling(window=60).mean()
rssi_df["snr_pkt_smooth"] = rssi_df["snr_pkt"].rolling(window=60).mean()

merged_df = pd.merge_asof(rssi_df, ubx_df, on="time")
print(merged_df)
merged_df.to_csv(dest, sep=",")
