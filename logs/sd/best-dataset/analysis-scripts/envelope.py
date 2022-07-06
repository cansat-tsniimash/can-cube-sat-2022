import csv

field_name = "current"
infile_name = "balt+ELECTRICAL_STATE-10-2.csv"
ofile_name = "uenv+ELECTRICAL_STATE-10-2.csv"


def upper_envelope(data):
	odata = []
	window = []
	for record in data:
		window.append(record)
		if len(window) < 3:
			continue

		left = float(window[0][field_name])
		center = float(window[1][field_name])
		right = float(window[2][field_name])

		if center > left and center > right:
			odata.append(window[1])

		window = window[-3:]

	return odata

with open(infile_name, mode="r") as istream:
	reader = csv.DictReader(istream)
	data = list(reader)

odata = upper_envelope(data)

with open(ofile_name, mode="w") as ostream:
	writer = csv.DictWriter(ostream, fieldnames=odata[0].keys())
	writer.writeheader()
	for line in odata:
		writer.writerow(line)
