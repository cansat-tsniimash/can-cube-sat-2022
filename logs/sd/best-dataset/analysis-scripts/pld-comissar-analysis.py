import pandas as pd


for no in [0,1,2,3,4]:
	df = pd.read_csv(f"../COMMISSAR_REPORT-13-{no}.csv")
	df["delta_time"] = df["time_s_from_launch"].diff()

	df["delta_pun"] = df["punishments_counter"].diff()
	df["pun_per_sec"] = df["delta_pun"] / df["delta_time"]

	df["delta_bads"] = df["bad_reports_counter"].diff()
	df["delta_reps"] = df["reports_counter"].diff()
	df["rela_delta_bads"] = df["delta_bads"] / df["delta_reps"]
	df["rela_delta_bads_per_sec"] = df["rela_delta_bads"] / df["delta_time"]

	df.to_csv(f"com-a+COMMISSAR_REPORT-13-{no}.csv", sep=",")
