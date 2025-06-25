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
joker_df = pd.read_csv('joker_running_times_triangle_tre_new.csv', skiprows=1, header=None)
best_df = pd.read_csv('best_effort_running_times_triangle_tre_new.csv', skiprows=1, header=None)
plt.rcParams.update({'font.size': 22})

# Take only the first 10 rows and the last column
for i in range(40):
    if i == 0:
        joker_times = joker_df.iloc[:10, -1].values
        best_times = best_df.iloc[:10, -1].values
        joker_times = clean_column(joker_times)
        
        best_times = clean_column(best_times)

        # Prepare the x-axis labels
        samples = [f"Instance {i+1}" for i in range(10)]
        x = np.arange(len(samples))  # 10 samples

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
        ax.set_title('Comparison of runtimes: TRIANGLE-1')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
        ax.legend()
        
        ax.set_yscale('log')
        
        all_res = joker_times.copy()
        #all_res.extend(winning_times)
        all_res.extend(best_times)
        max_y = max(all_res)
        closest_power_10 = 10 ** np.ceil(np.log10(max_y))
        print(closest_power_10)
        plt.ylim(top = closest_power_10 + 100)
        
        plt.show()
    elif i == 11:
        joker_times = joker_df.iloc[10:20, -1].values
        best_times = best_df.iloc[10:20, -1].values
        
        joker_times = clean_column(joker_times)
        best_times = clean_column(best_times)
        
        samples = [f"Instance {i+1}" for i in range(10)]
        x = np.arange(len(samples))  # 10 samples

        width = 0.4  # width of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        fig.subplots_adjust(top=0.96, bottom=0.235)

        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')

        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title('Comparison of runtimes: TRIANGLE-2')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
         
        ax.set_yscale('log')
        ax.legend()
        all_res = joker_times.copy()
        all_res.extend(best_times)
        max_y = max(all_res)
        closest_power_10 = 10 ** np.ceil(np.log10(max_y))
        yticks = plt.yticks()[0]
        yticks = [tick if tick <= 1000 else 1000 for tick in yticks]
        ytick_labels = [tick if tick != 1000 else 'TIMEOUT' for tick in yticks]
        plt.yticks(yticks, ytick_labels)
        plt.ylim(top = closest_power_10 + 100)
       
        plt.show()
    elif i == 21:
        joker_times = joker_df.iloc[20:30, -1].values
        best_times = best_df.iloc[20:30, -1].values
        joker_times = clean_column(joker_times)
        best_times = clean_column(best_times)
        
        # Prepare the x-axis labels
        samples = [f"Instance {i+1}" for i in range(10)]
        x = np.arange(len(samples))  # 10 samples

        width = 0.4  # width of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        fig.subplots_adjust(top=0.96, bottom=0.235)

        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')

        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title('Comparison of runtimes: TRIANGLE-3')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
        ax.set_yscale('log')
        ax.legend()
        all_res = joker_times.copy()
        all_res.extend(best_times)
        max_y = max(all_res)
        closest_power_10 = 10 ** np.ceil(np.log10(max_y))
        
        plt.ylim(top = closest_power_10 + 10)
        
        plt.show()
    elif i == 31:
        joker_times = joker_df.iloc[30:, -1].values
        best_times = best_df.iloc[30:, -1].values
        joker_times = clean_column(joker_times)
        best_times = clean_column(best_times)
       
        samples = [f"Instance {i+1}" for i in range(10)]
        x = np.arange(len(samples))  # 10 samples

        width = 0.4  # width of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        fig.subplots_adjust(top=0.96, bottom=0.235)

        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')

        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title('Comparison of runtimes: TRIANGLE-4')
        ax.set_xticks(x)
         
        ax.set_xticklabels(samples, rotation=90)
        ax.set_yscale('log')
        ax.legend()
        all_res = joker_times.copy()
        all_res.extend(best_times)
        max_y = max(all_res)
        closest_power_10 = 10 ** np.ceil(np.log10(max_y))
        print(closest_power_10)
        yticks = plt.yticks()[0]
        yticks = np.append(yticks, 1000)
        print(yticks)
        ytick_labels = [tick if tick != 1000 else 'TIMEOUT' for tick in yticks]
        print(ytick_labels)
        plt.yticks(yticks, ytick_labels)
        plt.ylim(top = closest_power_10 + 100)
        # Layout adjustment
        #plt.tight_layout()
        plt.show()
