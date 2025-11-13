import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math

def clean_column(series):
    """Convert values to float, ignore non-numeric ones."""
    cleaned = []
    for val in series:
        
        cleaned.append(float(val))
        
    return cleaned
plt.rcParams.update({'font.size': 22})
# Read the three CSV files
joker_df = pd.read_csv('joker_times.csv', skiprows=1, header=None)
best_df = pd.read_csv('BE_times.csv', skiprows=1, header=None) #0
print(joker_df)
print(best_df)

joker_times = joker_df.iloc[:, 6].values
best_times = best_df.iloc[:, 6].values #[,8]
joker_times = clean_column(joker_times)
best_times = clean_column(best_times)

print(joker_times)
print(best_times)

joker_mean = np.mean(joker_times)
best_mean = np.mean(best_times)
print(joker_mean)
print(best_mean)

joker_df_2 = pd.read_csv('joker_safe_times.csv', skiprows=1, header=None)
best_df_2 = pd.read_csv('BE_safe_times.csv', skiprows=0, header=None) 
 
joker_times_2 = joker_df_2.iloc[:, 6].values
best_times_2 = best_df_2.iloc[:, 8].values #[,8]
joker_times_2 = clean_column(joker_times_2)[:8]
best_times_2 = clean_column(best_times_2)[:8]

print(joker_times_2)
print(best_times_2)

joker_mean_2 = np.mean(joker_times_2)
best_mean_2 = np.mean(best_times_2)
print(joker_mean_2)
print(best_mean_2)