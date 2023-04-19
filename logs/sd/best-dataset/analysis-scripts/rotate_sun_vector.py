import pandas as pd
import numpy as np
import quaternion

df = pd.read_csv("../SINS_ISC-11-0.csv")
print(df)

df["r"] = pd.to_numeric(df["light_dir_real[0]"])
df["theta"] = pd.to_numeric(df["light_dir_real[1]"])
df["phi"] = pd.to_numeric(df["light_dir_real[2]"])

df["sp_real[x]"] = df["r"] * np.sin(df["theta"]) * np.cos(df["phi"])
df["sp_real[y]"] = df["r"] * np.sin(df["theta"]) * np.sin(df["phi"])
df["sp_real[z]"] = df["r"] * np.cos(df["theta"])

df["sp_real_q"] = df.apply(
	lambda row: np.quaternion(0, row["sp_real[x]"], row["sp_real[y]"], row["sp_real[z]"]),
	axis=1
)

df["quaternion"] = df.apply(
	lambda row: np.quaternion(row["quaternion[0]"], row["quaternion[1]"], row["quaternion[2]"], row["quaternion[3]"]),
	axis=1
)

df["sp_real_rot_q"] = df["quaternion"] * df["sp_real_q"] * np.conjugate(df["quaternion"])
#df["sp_real_rot_q"] = np.conjugate(df["quaternion"]) * df["sp_real_q"] * df["quaternion"]
df["sp_real_rot[x]"] = df.apply(lambda row: row["sp_real_rot_q"].x, axis=1)
df["sp_real_rot[y]"] = df.apply(lambda row: row["sp_real_rot_q"].y, axis=1)
df["sp_real_rot[z]"] = df.apply(lambda row: row["sp_real_rot_q"].z, axis=1)

df["sp_real_rot[r]"] = np.sum(
	np.square(df[["sp_real_rot[x]", "sp_real_rot[y]", "sp_real_rot[z]"]]),
	axis=1
)
df["sp_real_rot[theta]"] = np.arccos(df["sp_real_rot[z]"] / df["sp_real_rot[r]"])
df["sp_real_rot[phi]"] = np.arctan2(df["sp_real_rot[y]"], df["sp_real_rot[x]"])

df = df.drop(columns=["r", "theta", "phi"])
df = df.drop(columns=["sp_real[x]", "sp_real[y]", "sp_real[z]"])
df = df.drop(columns=["sp_real_q"])
df = df.drop(columns=["quaternion", "sp_real_rot_q"])

print(df)
df.to_csv(f"SINS_ISC-11-0+rotated-sun.csv", sep=",")

