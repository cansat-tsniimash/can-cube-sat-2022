import pandas as pd


for no in [11,12,13]:
	df = pd.read_csv(f"../I2C_LINK_STATS-{no}-0.csv")
	df["delta_time"] = df["time_s_from_launch"].diff()

	df["delta_restarts"] = df["restarts_cnt"].diff()
	df["rela_delta_restarts"] = df["delta_restarts"] / df["delta_time"]

	df["delta_tx_done"] = df["tx_packet_done_cnt"].diff()
	df["rela_delta_tx_done"] = df["delta_tx_done"] / df["delta_time"]
	df["rela_delta_tx_done_smooth"] = df["rela_delta_tx_done"].rolling(window=50).mean()

	df.to_csv(f"its-link-a+I2C_LINK_STATS-{no}-0.csv", sep=",")
