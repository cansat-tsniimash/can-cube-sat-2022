import pandas as pd
import matplotlib.pyplot as plt


df = pd.read_csv("../SINS_MONHW-11-0.csv")
df["jam_ind_smooth"] = df["jam_ind"].rolling(window=50).mean()
df["noise_per_ms_smooth"] = df["noise_per_ms"].rolling(window=50).mean()
df["agc_cnt_smooth"] = df["agc_cnt"].rolling(window=50).mean()
df.to_csv("gps-a+SINS_MONHW-11-0.csv", sep=",")


df = pd.read_csv("../SINS_MONHW2-11-0.csv")
df["ofs_i_smooth"] = df["ofs_i"].rolling(window=50).mean()
df["mag_i_smooth"] = df["mag_i"].rolling(window=50).mean()
df["ofs_q_smooth"] = df["ofs_q"].rolling(window=50).mean()
df["mag_q_smooth"] = df["mag_q"].rolling(window=50).mean()
df.to_csv("gps-a+SINS_MONHW2-11-0.csv", sep=",")


df = pd.read_csv("../SINS_NAVSVINFO-11-0.csv")
df["cno_0_smooth"] = df["cno_0"].rolling(window=50).mean()
df.to_csv("gps-a+SINS_NAVSVINFO-11-0.csv", sep=",")

# ax = df["quality_0"].plot.hist(bins=12, alpha=0.5)
# plt.show()