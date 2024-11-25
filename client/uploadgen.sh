#!/bin/bash

# Function to create a file of the specified size (in MB)
create_file() {
    local filename="$1"
    local size_mb="$2"
    dd if=/dev/zero of="$filename" bs=1M count="$size_mb" status=none
}

# Function to upload a file
upload_file() {
    local port="$1"
    local username="$2"
    local password="$3"
    local filename="$4"
    local server_folder="/mnt/c/MTech/Project/Chat_Application/server/files"  # Path to server's files folder

    # Run the client program to upload the file (output suppressed)
    (echo -e "$username\n$password\nfileupload:$filename" | ./client "$port" > /dev/null 2>&1) &
    client_pid=$!  # Capture the PID of the client process
    client_pids+=($client_pid)  # Store the client PID in the array

    file_uploaded="false"

    # Start monitoring the server/files folder for the uploaded file
    while true; do
        # Check if the file has appeared in the server/files directory
        if [[ -f "$server_folder/$filename" ]]; then
            file_uploaded="true"
            break
        fi

        # Check if the client process is still running, if not exit
        if ! ps -p $client_pid > /dev/null; then
            echo "Client process ended prematurely without completing upload."
            break
        fi

        # Sleep before checking again
        sleep 1
    done

    # Return success or failure based on whether the file was uploaded
    if [[ "$file_uploaded" == "true" ]]; then
        return 0
    else
        return 1
    fi
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
    local output_file="upload_results.txt"  # File where results will be saved

    # Create files for upload
    filenames=()
    for ((i = 1; i <= num_clients; i++)); do
        filename="file_name$i.txt"
        create_file "$filename" "$file_size_mb"
        filenames+=("$filename")
    done

    # Start timing the file uploads (including milliseconds)
    start_time=$(date +%s.%3N)

    # Upload files concurrently using background jobs
    for ((j = 1; j <= num_clients; j++)); do
        username="client$j"
        filename="${filenames[j-1]}"

        # Run the upload_file function for each client in the background
        upload_file "$port" "$username" "$password" "$filename" &
    done

    # Wait for all background jobs to finish
    wait

    # End the timing for file uploads (including milliseconds)
    end_time=$(date +%s.%3N)

    # Calculate total time in seconds and milliseconds
    total_time=$(echo "$end_time - $start_time" | bc)

    # Print the total time in seconds and milliseconds
    echo "Total time for file upload: $total_time seconds"

    # Kill all client processes after upload completes
    for pid in "${client_pids[@]}"; do
        if ps -p $pid > /dev/null; then
            kill $pid
        fi
    done

    # Clean up created files on local system
    for filename in "${filenames[@]}"; do
        if [[ -f "$filename" ]]; then
            rm "$filename"
        fi
    done

    # Clean up uploaded files from server
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
