#include "filehandler.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

void file_command_handler(int socket, const string &name, const string &command, const string &filename, const string &sentuser) {
    FILE *file = fopen(filename.c_str(), "r");
    if (file == NULL) {
        cout << "Error opening file" << endl;
        return;
    }

    string msg;
    if (command == "fileupload") 
        msg = name + " : " + command + " : " + filename;
    else if (command == "fileshare") 
        msg = name + " : " + command + " : " + filename + " : " + sentuser;

    int bytesSent = send(socket, msg.c_str(), msg.length(), 0);
    if (bytesSent == -1) {
        cout << "Error sending message" << endl;
        fclose(file);
        return;
    }

    // sending the file contents to server
    size_t bytesRead;
    char file_buffer[512];
    while ((bytesRead = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        int bytesSent = send(socket, file_buffer, bytesRead, 0);
        if (bytesSent == -1) {
            cout << "Error sending file data" << endl;
            fclose(file); 
            return;
        }
        memset(file_buffer, 0, sizeof(file_buffer));
    }
    string message = "###";
    send(socket, message.c_str(), message.length(), 0);
    // exit(0);

    fclose(file);
}

void file_download_handler(int socket, const string &name, const string &command, const string &filename) {

    string msg;
    msg = name + " : " + command + " : " + filename;

    int bytesSent = send(socket, msg.c_str(), msg.length(), 0);
    if (bytesSent == -1) {
        cout << "Error sending message" << endl;
        return;
    }
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        cout << "Error in creating file!!!" << endl;
        return;
    }

    size_t bytesRead;
    char file_buffer[512] = {0};
    while ((bytesRead = recv(socket, file_buffer, sizeof(file_buffer), 0)) > 0) {
        if(containsSubstring(file_buffer, "File does not exist")){
            close(fd);
            cout<<"File does not exist!!!!";

            if (remove(filename.c_str()) != 0) 
                cout<<"Error deleting file";
            return;
        }
        if (containsSubstring(file_buffer, "###")) {
            write(fd, file_buffer, bytesRead - 3);
            close(fd);
            cout<<"File download complete.\n\n";
            return;
        }
        ssize_t bytesWritten = write(fd, file_buffer, bytesRead);
        if (bytesWritten == -1) {
            cout<<"Write failed"<<endl;;
            close(fd);
            return;
        }
        memset(file_buffer, 0, sizeof(file_buffer));
    }
}


void file_transfer_handler(const string &sub_message, int socket) {
    size_t pos = sub_message.rfind(".");
    // string filename = sub_message.substr(0, pos) + "_" + getCurrentDateTime() + sub_message.substr(pos);
    string filename = sub_message.substr(0, pos) + sub_message.substr(pos);

    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        cout << "Error in creating file!!!" << endl;
        return;
    }

    size_t bytesRead;
    char file_buffer[512] = {0};
    while ((bytesRead = recv(socket, file_buffer, sizeof(file_buffer), 0)) > 0) {
        if (containsSubstring(file_buffer, "###")) {
            write(fd, file_buffer, bytesRead - 3);
            close(fd);
            string msg = "File upload complete.\n";
            send(socket, msg.c_str(), msg.length(), 0);
            return;
        }
        ssize_t bytesWritten = write(fd, file_buffer, bytesRead);
        if (bytesWritten == -1) {
            perror("Write failed");
            close(fd);
            return;
        }
        memset(file_buffer, 0, sizeof(file_buffer));
    }
}
