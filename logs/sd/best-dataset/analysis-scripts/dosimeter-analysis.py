import pandas as pd


df = pd.read_csv("PLD_DOSIM_DATA-13-0.csv")
df["ticks_per_sec"] = df["count_tick"] / (df["delta_time"] / 1000)
df["ticks_per_sec_smooth"] = df["ticks_per_sec"].rolling(window=40).mean()
df.to_csv("dosim-a+PLD_DOSIM_DATA-13-0.csv", sep=",")

df = pd.read_csv("PLD_DOSIM_DATA-13-1.csv")
df["ticks_per_sec"] = df["count_tick"] / (df["delta_time"] / 1000)
df["ticks_per_sec_smooth"] = df["ticks_per_sec"].rolling(window=10).mean()
df.to_csv("dosim-a+PLD_DOSIM_DATA-13-1.csv", sep=",")

