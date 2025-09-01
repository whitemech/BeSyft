import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def clean_column(series):
    """Convert values to float, ignore non-numeric ones."""
    cleaned = []
    for val in series:
        try:
            cleaned.append(float(val))
        except ValueError:
            if val == "TIMEOUT":
                cleaned.append(1000)  # Assign NaN if not a number
            else:
                cleaned.append(np.nan)
    return cleaned

# Read the two CSV files
joker_df = pd.read_csv('joker_running_times_new_new.csv', skiprows=1, header=None)
best_df = pd.read_csv('best_effort_running_times_new_new.csv', skiprows=1, header=None)
plt.rcParams.update({'font.size': 22})
# Take only the first 15 rows and the last column
for i in range(60):
    if i == 0:
        joker_times = joker_df.iloc[:15, -1].values
        best_times = best_df.iloc[:15, -1].values
        
        joker_times = clean_column(joker_times)
        
        best_times = clean_column(best_times)
        
        samples = [f"Instance {i+1}" for i in range(15)]
        print(samples)
        x = np.arange(len(samples))  # 15 samples
        print(x)
        width = 0.4  # width of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 20))
        fig.subplots_adjust(top=0.96, bottom=0.235)
        
        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')
        
        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title('Comparison of runtimes: COFFEE-1')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
        ax.set_yscale('log')
        ax.legend()
        all_res = joker_times.copy()
        all_res.extend(best_times)
        max_y = max(all_res)
        closest_power_10 = 10 ** round(np.log10(max_y))
        
        plt.ylim(top = closest_power_10 + 100)
        yticks = plt.yticks()[0]
        yticks = [tick if tick <= 1000 else 1000 for tick in yticks]
        ytick_labels = [tick if tick < 1000 else 'TIMEOUT' for tick in yticks]
        plt.yticks(yticks, ytick_labels)
        
        plt.show()
    elif i == 16:
        joker_times = joker_df.iloc[15:30, -1].values
        best_times = best_df.iloc[15:30, -1].values
        
        joker_times = clean_column(joker_times)
        best_times = clean_column(best_times)
        
        # Prepare the x-axis labels
        samples = [f"Instance {i+1}" for i in range(15)]
        x = np.arange(len(samples))  # 15 samples

        width = 0.40  # width of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        fig.subplots_adjust(top=0.96, bottom=0.235)

        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')

        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title('Comparison of runtimes: COFFEE-2')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
        ax.set_yscale('log')
        ax.legend()
        yticks = plt.yticks()[0]
        all_res = joker_times.copy()
        all_res.extend(best_times)
        max_y = max(all_res)
        closest_power_10 = 10 ** round(np.log10(max_y))
        print(closest_power_10)
        
        plt.ylim(top = closest_power_10 + 0.1)
        
        plt.show()
    elif i == 31:
        joker_times = joker_df.iloc[30:45, -1].values
        best_times = best_df.iloc[30:45, -1].values
        joker_times = clean_column(joker_times)
        best_times = clean_column(best_times)
       
        # Prepare the x-axis labels
        samples = [f"Instance {i+1}" for i in range(15)]
        x = np.arange(len(samples))  # 15 samples

        width = 0.40  # width of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        fig.subplots_adjust(top=0.96, bottom=0.235)

        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')

        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title('Comparison of runtimes: COFFEE-3')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
        ax.set_yscale('log')
        ax.legend()
        yticks = plt.yticks()[0]
        yticks = [tick if tick <= 1000 else 1000 for tick in yticks]
        ytick_labels = [tick if tick < 1000 else 'TIMEOUT' for tick in yticks]
        plt.yticks(yticks, ytick_labels)
        all_res = joker_times.copy()
        all_res.extend(best_times)
        max_y = max(all_res)
        closest_power_10 = 10 ** round(np.log10(max_y))
        print(closest_power_10)
        
        plt.ylim(top = closest_power_10 + 0.1)
        
        plt.show()
    elif i == 46:
        joker_times = joker_df.iloc[45:, -1].values
        best_times = best_df.iloc[45:, -1].values
        joker_times = clean_column(joker_times)
        best_times = clean_column(best_times)
        print(joker_times)
        # Prepare the x-axis labels
        samples = [f"Instance {i+1}" for i in range(15)]
        x = np.arange(len(samples))  # 15 samples

        width = 0.40  # width/2 of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        fig.subplots_adjust(top=0.96, bottom=0.235)

        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')

        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title('Comparison of runtimes: COFFEE-4')
        ax.set_xticks(x)
        ax.set_yscale('log')
        ax.set_xticklabels(samples, rotation=90)
        ax.legend()
        all_res = joker_times.copy()
        all_res.extend(best_times)
        max_y = max(all_res)
        print(max_y)
        closest_power_10 = 0.6
        print(closest_power_10)
        
        plt.ylim(top = closest_power_10 + 0.1)
        
        plt.show()
