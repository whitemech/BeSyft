import csv

def compute_means(filename):
    means = []
    current_values = []
    
    with open(filename, newline='') as csvfile:
        reader = csv.reader(csvfile)
        next(reader)  # Skip header
        
        for i, row in enumerate(reader, start=1):
            if row and row[-1] != "TIMEOUT" and row[-1] != "MEM":
                value = float(row[-1])  # Take the last column
                current_values.append(value)
            
            if row and i % 10 == 0:
                mean = sum(current_values) / len(current_values)
                means.append(mean)
                print(len(current_values))
                current_values = []  # Reset for next group
                
    return means

# Example usage
joker_file = "joker_running_times_triangle_tre_new.csv"
means = compute_means(joker_file)

for idx, mean in enumerate(means, start=1):
    print(f"joker robot {idx}: {mean:.4f}")

joker_file = "best_effort_running_times_triangle_tre_new.csv"
means = compute_means(joker_file)

for idx, mean in enumerate(means, start=1):
    print(f"BE robot {idx}: {mean:.4f}")

