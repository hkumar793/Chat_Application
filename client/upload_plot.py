import re
import matplotlib.pyplot as plt

def read_data_from_file(file_path):
    data = []
    pattern = r"File size: (\d+)MB Clients: (\d+) Time: ([\d\.]+) seconds"
    
    with open(file_path, 'r') as file:
        for line in file:
            match = re.search(pattern, line.strip())
            if match:
                try:
                    file_size = int(match.group(1))  
                    clients = int(match.group(2))    
                    time = float(match.group(3))    
                    data.append((file_size, clients, time))
                except ValueError as e:
                    print(f"Error parsing line: {line}. Error: {e}")

    return data

def plot_data(data):
    file_sizes = sorted(set([entry[0] for entry in data]))

    time_data = {size: [] for size in file_sizes}

    for size in file_sizes:
        time_data[size] = [(entry[1], entry[2]) for entry in data if entry[0] == size]

    plt.figure(figsize=(10, 6))

    for size in file_sizes:
        clients = [entry[0] for entry in time_data[size]]
        times = [entry[1] for entry in time_data[size]]

        if clients and times:
            plt.plot(clients, times, label=f"File size: {size}MB", marker='o')

    plt.xlabel('Number of Clients')
    plt.ylabel('Upload Time (seconds)')
    plt.title('Upload Time vs Number of Clients for Different File Sizes')

    plt.legend()

    plt.grid(True)

    output_image_path = 'upload_time_plot.png'  
    plt.savefig(output_image_path)

def main():
    file_path = 'upload_results.txt'  

    data = read_data_from_file(file_path)
    plot_data(data)

if __name__ == "__main__":
    main()
