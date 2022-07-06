import pandas as pd

source = "../its-broker-log-20220704T041957_mdt/radio.rssi_packet.csv"
dest = "a+radio.rssi_packet.csv"

df = pd.read_csv(source)
df["rssi_pkt_smooth"] = df["rssi_pkt"].rolling(window=40).mean()
df.to_csv(dest, sep=",")
