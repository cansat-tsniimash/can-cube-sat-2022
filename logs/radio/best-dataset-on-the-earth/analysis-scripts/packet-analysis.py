import pandas as pd


#sc_start_time = 1656909365 # Время запуска
sc_start_time = 1633357250 # Время запуска


#source = "../its-broker-log-20220704T041957_mdt/radio.rssi_packet.csv"
source = "../its-broker-log-20220704T041957_mdt_old/radio.rssi_packet.csv"
dest = "a+radio.rssi_packet.csv"

df = pd.read_csv(source)
df["rssi_pkt_smooth"] = df["rssi_pkt"].rolling(window=60).mean()
df["snr_pkt_smooth"] = df["snr_pkt"].rolling(window=60).mean()
df["mins_from_start"] = (df['time_s'] - sc_start_time + df["time_us"] / 1000_000) / 60
df.to_csv(dest, sep=",")


#source = "../its-broker-log-20220704T041957_mdt/radio.downlink_frame.csv"
source = "../its-broker-log-20220704T041957_mdt_old/radio.downlink_frame.csv"
dest = "a+radio.downlink_frame.csv"
stats_dest = "a+lost_frames.csv"


df = pd.read_csv(source)
df["lost_frames"] = df["frame_no"].diff() - 1
df["bad_frames"] = df["frame_no"].diff() - 1
df["mins_from_start"] = (df['time_s'] - sc_start_time + df["time_us"] / 1000_000) / 60
print(df)
df.to_csv(dest, sep=",")

bin_step = 2
bins = list(range(0, 120, bin_step))
groups = df.groupby(pd.cut(df["mins_from_start"], bins=bins, labels=bins[1:]), as_index=False)
print(pd.DataFrame(groups))
stats = groups.agg({"mins_from_start": ["min"], "lost_frames": ["sum"], "time_s": ["count"]})
stats.columns = ["mins_from_start", "lost_frames", "total_frames"]
stats["mins_from_start"] = (stats["mins_from_start"] - 0.5).round(0) + bin_step/2
stats["lost_frames"].fillna(0)
stats["total_frames"].fillna(0)

print(stats)
stats.to_csv(stats_dest, sep=",")



