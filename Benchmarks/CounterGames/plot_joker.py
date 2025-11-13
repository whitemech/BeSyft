import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math

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
plt.rcParams.update({'font.size': 22})
# Read the three CSV files
joker_df = pd.read_csv('joker_times.csv', skiprows=1, header=None)
best_df = pd.read_csv('BE_times.csv', skiprows=1, header=None) #0
print(joker_df)
print(best_df)

for i in range(80): #10
    if i == 0:
        joker_times = joker_df.iloc[:10, 6].values
        best_times = best_df.iloc[:10, 6].values #[,8]
        joker_times = clean_column(joker_times)
        
        best_times = clean_column(best_times)
        
        print(joker_times)
        print(best_times)
        
        # Prepare the x-axis labels
        samples = [f"Instance {i+1}" for i in range(10)]
        x = np.arange(len(samples))  # 15 samples

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
        #ax.set_title('Comparison of runtimes under ENV(2); n = 1,..,10')
        ax.set_title('Comparison of runtimes under ENV(1); K = 1,...,10; n = 1')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
        ax.set_yscale('symlog')
        ax.legend()
        plt.ylim(top=0.03)
        #plt.ylim(top=10)
        # yticks = plt.yticks()[0]
        # y_max = max(yticks)
        # exponent = math.ceil(math.log10(y_max))
        # new = 10 ** exponent
        #yticks = [tick if tick <= 1000 else 1000 for tick in yticks]
        # ytick_labels = [tick if tick != 1000 else 'TIMEOUT' for tick in yticks]
        # plt.yticks(yticks, ytick_labels)
        #yticks = plt.yticks()[0]
        #print(yticks)
        #yticks = np.append(yticks, 1000)

        #yticks = [tick if tick <= 1000 else 1000 for tick in yticks]
        #ytick_labels = [str(float(tick)) if tick != 1000 else 'TIMEOUT' for tick in yticks]
        #plt.yticks(yticks, ytick_labels)
        # Layout adjustment
        #plt.tight_layout()
        plt.show()
    elif i == 11 or i == 21 or i == 31 or i == 41 or i == 51 or i == 61 or i == 71:
        joker_times = joker_df.iloc[i-1:i+10-1, 6].values
        best_times = best_df.iloc[i-1:i+10-1, 6].values
        joker_times = clean_column(joker_times)
        
        best_times = clean_column(best_times)
        
        print(joker_times)
        print(best_times)
        # Prepare the x-axis labels
        samples = [f"Instance {i+1}" for i in range(10)]
        x = np.arange(len(samples))  # 15 samples

        width = 0.4  # width of the bars

        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        fig.subplots_adjust(top=0.96, bottom=0.235)
        #ax.set_yscale('log')

        # Plot the bars
        ax.bar(x - width/2, joker_times, width, label='MBE')
        ax.bar(x + width/2, best_times, width, label='BE')
        if i == 11:
            j = 2
        elif i == 21:
            j = 3
        elif i == 31:
            j = 4
        elif i == 41:
            j = 5
        elif i == 51:
            j = 6
        elif i == 61:
            j = 7
        elif i == 71:
            j = 8


        # Labels and title
        ax.set_xlabel('Instances')
        ax.set_ylabel('Runtime (s)')
        ax.set_title(f'Comparison of runtimes under ENV(1); K = 1,...,10; n = {j}')
        ax.set_xticks(x)
        ax.set_xticklabels(samples, rotation=90)
        ax.set_yscale('symlog')
        ax.legend()
        if j == 2:
            plt.ylim(top=0.04)
        elif j == 3:
            plt.ylim(top=0.06)
        elif j == 4:
            plt.ylim(top=0.1)
        elif j == 5:
            plt.ylim(top=0.2)
        elif j == 6:
            plt.ylim(top=0.6)
        else:
            plt.ylim(top=10)

        if j == 8:
            plt.legend(fontsize = 'x-small', loc='upper left', bbox_to_anchor=(0, 1))
        #yticks = plt.yticks()[0]
        #yticks = [tick if tick <= 1000 else 1000 for tick in yticks]
        #ytick_labels = [tick if tick != 1000 else 'TIMEOUT' for tick in yticks]
        #plt.yticks(yticks, ytick_labels)
        # Layout adjustment
        #plt.tight_layout()
        plt.show()
    
