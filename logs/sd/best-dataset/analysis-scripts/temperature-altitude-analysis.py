import pandas as pd


for no in range(0, 6):
	df = pd.read_csv(f"../THERMAL_STATE-10-{no}.csv")
	up_df = df[:4980]
	down_df = df[4980:]

	up_df = up_df.sort_values("baro_altitude")
	down_df = down_df.sort_values("baro_altitude")

	print(up_df)
	print(down_df)
	merged_df = pd.merge_asof(up_df, down_df, on="baro_altitude", tolerance=1)
	merged_df["delta_t"] = merged_df["temperature_x"] - merged_df["temperature_y"]
	merged_df.to_csv(f"temp-a+THERMAL_STATE-10-{no}.csv", sep=",")


bounds = [4100, 3700, 3000]
for i, no in enumerate([11, 12, 13]):
	df = pd.read_csv(f"../OWN_TEMP-{no}-0.csv")
	up_df = df[:bounds[i]]
	down_df = df[bounds[i]:]

	up_df = up_df.sort_values("baro_altitude")
	down_df = down_df.sort_values("baro_altitude")

	print(up_df)
	print(down_df)
	merged_df = pd.merge_asof(up_df, down_df, on="baro_altitude", tolerance=1)
	merged_df["delta_t"] = merged_df["deg_x"] - merged_df["deg_y"]
	merged_df.to_csv(f"temp-a+OWN_TEMP-{no}-0.csv", sep=",")


for no in range(0, 4):
	df = pd.read_csv(f"../THERMAL_STATE-12-{no}.csv")
	up_df = df[:4450]
	down_df = df[4450:]

	up_df = up_df.sort_values("baro_altitude")
	down_df = down_df.sort_values("baro_altitude")

	print(up_df)
	print(down_df)
	merged_df = pd.merge_asof(up_df, down_df, on="baro_altitude", tolerance=1)
	merged_df["delta_t"] = merged_df["temperature_x"] - merged_df["temperature_y"]
	merged_df.to_csv(f"temp-a+THERMAL_STATE-12-{no}.csv", sep=",")
