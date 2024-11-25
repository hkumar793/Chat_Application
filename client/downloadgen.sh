#!/bin/bash

create_file_on_server() {
    local filename="$1"
    local size_mb="$2"
    local server_folder="/mnt/c/MTech/Project/Chat_Application/server/files"  # Path to server's files folder

    dd if=/dev/zero of="$server_folder/$filename" bs=1M count="$size_mb" status=none
}

# Function to download a file
download_file() {
    local port="$1"
    local username="$2"
    local password="$3"
    local filename="$4"
    local client_folder="./"  # Current directory for downloading files

    # Run the client program to request the file download (output suppressed)
    (echo -e "$username\n$password\nfiledownload:$filename" | ./client "$port" > /dev/null 2>&1) &

    client_pid=$!  # Capture the PID of the client process
    client_pids+=($client_pid)  # Store the client PID in the array

    # Monitor the current directory for the downloaded file
    while true; do
        # Check if the file has appeared in the current directory
        if [[ -f "$client_folder/$filename" ]]; then
            return 0  # Return success when file is downloaded
        fi

        # Check if the client process is still running, if not exit
        if ! ps -p $client_pid > /dev/null; then
            echo "Client process ended prematurely without completing download."
            return 1  # Failure if the client process ended prematurely
        fi

        # Sleep before checking again
        sleep 1
    done
}

# Main script
main() {
    # Parse command-line arguments
    if [[ $# -ne 3 ]]; then
        echo "Usage: $0 <port> <file_size_mb> <num_clients>"
        exit 1
    fi

    local port="$1"
    local file_size_mb="$2"
    local num_clients="$3"
    local password="123456"
    local server_folder="/mnt/c/MTech/Project/Chat_Application/server/files"  # Path to server's files folder
    local client_folder="./"  # Current directory for downloading files
    local output_file="download_results.txt"  # File where results will be saved

    # Create files on the server
    filenames=()
    for ((i = 1; i <= num_clients; i++)); do
        filename="file_name$i.txt"
        create_file_on_server "$filename" "$file_size_mb"
        filenames+=("$filename")
    done

    # Start timing the file downloads (including milliseconds)
    start_time=$(date +%s.%3N)

    # Download files concurrently using background jobs
    for ((j = 1; j <= num_clients; j++)); do
        username="client$j"
        filename="${filenames[j-1]}"

        # Run the download_file function for each client in the background
        download_file "$port" "$username" "$password" "$filename" &
    done

    # Wait for all download background jobs to finish
    wait

    # End the timing for file downloads (including milliseconds)
    end_time=$(date +%s.%3N)

    # Calculate total time in seconds and milliseconds
    total_time=$(echo "$end_time - $start_time" | bc)

    # Print the total time in seconds and milliseconds
    echo "Total time for file download: $total_time seconds"

    # Kill all client processes after download completes
    for pid in "${client_pids[@]}"; do
        if ps -p $pid > /dev/null; then
            kill $pid
        fi
    done

    # Clean up downloaded files from the current directory
    for filename in "${filenames[@]}"; do
        client_file="$client_folder/$filename"
        if [[ -f "$client_file" ]]; then
            rm "$client_file"
        fi
    done

    # Clean up downloaded files from server
    for filename in "${filenames[@]}"; do
        server_file="$server_folder/$filename"
        if [[ -f "$server_file" ]]; then
            rm "$server_file"
        fi
    done

    # Write results to the output file
    echo "File size: ${file_size_mb}MB Clients: $num_clients Time: $total_time seconds" >> "$output_file"
}

# Run the main function
main "$@"
