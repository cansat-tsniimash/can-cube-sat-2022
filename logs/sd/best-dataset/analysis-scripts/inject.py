import shutil
import os
import csv
import bisect
import itertools

alt_file = "PLD_MS5611_DATA-13-2.csv"
data_catalogue = "T167_mav_csv_flight"
ignored_files = [os.path.split(alt_file)[1]]
injected_alt_key = "baro_altitude"
injected_press_key = "baro_pressure"

def get_alt_column(record):
	return float(record["altitude"])

def get_press_column(record):
	return float(record["pressure"])

def get_time_column(record):
	return int(record["time_s_from_launch"]) + int(record["time_us"]) / 1000 / 1000

def interpolate(samples, x, getter):
	idx = bisect.bisect_left(samples, x, key=get_time_column)
	if idx <= 0:
		return getter(alt_data[0])
	elif idx >= len(alt_data):
		return getter(alt_data[-1])
	else:
		left = idx - 1
		right = idx
		left_record = samples[left]
		right_record = samples[right]

		x0, y0 = get_time_column(left_record), getter(left_record)
		x1, y1 = get_time_column(right_record), getter(right_record)

		k = (y1 - y0) / (x1 - x0)
		b = y0
		rela_x = x - x0
		return k * rela_x + b


print("reading alt data from %s" % alt_file)

with open(alt_file, mode="r") as stream:
	reader = csv.DictReader(stream)
	alt_data = list(reader)

alt_data.sort(key=get_time_column)
print("got %s alt records" % len(alt_data))

for fname in os.listdir("."):
	fext = os.path.splitext(fname)[1]
	if fname in ignored_files or fext != ".csv" or fname.startswith("balt+"):
		print("skipping %s" % fname)
		continue

	oname = "balt+" + fname
	print("injecting altitude in %s" % fname)
	with open(fname, mode="r") as istream, open(oname, mode="w") as ostream:
		reader = csv.DictReader(istream)

		peek = next(reader)
		columns = list(peek.keys()) + [injected_alt_key, injected_press_key]

		writer = csv.DictWriter(ostream, fieldnames=columns)
		writer.writeheader()

		for number, record in enumerate(itertools.chain([peek], reader)):
			record_time = get_time_column(record)
			record_alt = interpolate(alt_data, record_time, get_alt_column)
			record_pressure = interpolate(alt_data, record_time, get_press_column)
			record[injected_alt_key] = record_alt
			record[injected_press_key] = record_pressure
			writer.writerow(record)

		print("done %s records" % number)

	os.remove(fname)
	shutil.move(oname, fname)
