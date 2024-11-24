import subprocess
import time
import argparse
import os
from multiprocessing import Pool

def create_file(filename, size_mb):
    subprocess.run(['dd', 'if=/dev/zero', f'of={filename}', f'bs=1M', f'count={size_mb}', 'status=none'])

def upload_file(args):
    port, username, password, filename = args
    try:
        # Start the client process
        process = subprocess.Popen(
            ['./client', str(port)], 
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True  # This ensures input/output is handled as strings
        )

        # Sending the username, password, and file upload command
        process.stdin.write(f"{username}\n")
        process.stdin.write(f"{password}\n")
        process.stdin.write(f"fileupload:{filename}\n")

        # Close the stdin to signal that no more input will be sent
        process.stdin.close()

        # Wait for the process to complete and capture stdout and stderr
        stdout, stderr = process.communicate(timeout=10)

        # Check the output for a successful upload message
        if "upload complete" in stdout:
            print(f"File {filename} uploaded successfully.")
            return f"File upload completed for {filename}", ""
        else:
            print(f"Error uploading {filename}: {stderr}")
            return f"Error uploading {filename}: {stderr}", ""

    except subprocess.TimeoutExpired:
        process.kill()
        return f"Error: {filename} timed out during upload", ""

    except Exception as e:
        process.kill()
        return f"Error with {filename}: {str(e)}", ""


def main():
    parser = argparse.ArgumentParser(description="Upload files to server")
    parser.add_argument("port", type=int, help="Port number for the client to connect")
    parser.add_argument("file_size_mb", type=int, help="Size of each file to upload (in MB)")
    parser.add_argument("num_clients", type=int, help="Number of clients to upload files")
    args = parser.parse_args()

    FILE_SIZE_MB = args.file_size_mb 
    NUM_CLIENTS = args.num_clients 
    password = "123456" 

    filenames = []

    for i in range(1, NUM_CLIENTS + 1): 
        filename = f"file_name{i}.txt"
        create_file(filename, FILE_SIZE_MB)  
        filenames.append(filename)

    start_time = time.time()
    tasks = []
    for j in range(1, NUM_CLIENTS + 1):
        username = f"client{j}"  
        filename = filenames[j - 1]  
        tasks.append((args.port, username, password, filename)) 

    with Pool(processes=NUM_CLIENTS) as pool:
        results = pool.map(upload_file, tasks)

    end_time = time.time()
    total_time = end_time - start_time
    print(f"Total time for file upload: {total_time:.2f} seconds")
    
    for filename in filenames:
        if os.path.exists(filename):
            os.remove(filename)

if __name__ == "__main__":
    main()
