#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <chrono>
#include <fcntl.h>  
#include "client_handler.h"
#include "utils.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>

using namespace std;

void commandExecution(string command, string sub_message, int clientSocket, string username, string extra, vector<ClientInfo> &clients) {
    int found = 0;

    // sending message to all online users except the one who is sending
    if (command == "all") {
        for (const auto &client : clients) {
            if (client.socket != clientSocket and client.conn == true) {
                found = 1;
                sub_message=username+" sent:"+sub_message;
                send(client.socket, sub_message.c_str(), sub_message.length(), 0);
            }
        }
    }

    // to list the users which are online
    else if (command == "listuser") {
        found = 1;
        int user_found = 0;
        string userList = "Online users: ";
        for (const auto &client : clients) {
            if (client.socket != clientSocket and client.conn) {
                userList += client.username + ",";
                user_found = 1;
            }
        }
        userList.erase(userList.size() - 1);
        if (!user_found) {
            string msg = "No one is online\n";
            send(clientSocket, msg.c_str(), msg.length(), 0);
        } else {
            userList += "\n";
            send(clientSocket, userList.c_str(), userList.length(), 0);
        }
    }

    // to upload file to the server
    else if (command == "fileupload") {
        size_t pos = sub_message.rfind(".");
        string filename = sub_message.substr(0, pos) + "_" + username + "_" + getCurrentDateTime() + sub_message.substr(pos);

        int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Open the file
        if (fd < 0) {
            cout << "Error in creating file!!!" << endl;
            return;
        }

        size_t bytesRead;
        char file_buffer[512] = {0};
        while ((bytesRead = recv(clientSocket, file_buffer, sizeof(file_buffer), 0)) > 0) {
            if (containsSubstring(file_buffer, "###")) {
                // removeSubstr(file_buffer, "###");
                write(fd, file_buffer, bytesRead - 3);
                close(fd);
                // cout << "File " << filename << " has been uploaded by " << username << endl;
                string msg = "File upload complete.\n";
                send(clientSocket, msg.c_str(), msg.length(), 0);
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

    // to share file to the client
    else if (command == "fileshare") {
        int sentuser_sock;
        string sentuser_name;
        for (auto client : clients) {
            if (client.username == extra and client.conn == true) {
                sentuser_sock = client.socket;
                sentuser_name = client.username;
                break;
            }
        }

        size_t pos = sub_message.rfind(".");
        string filename = sub_message.substr(0, pos) + "_" + username + sub_message.substr(pos) + "###";

        send(sentuser_sock, filename.c_str(), filename.length(), 0);

        size_t bytesRead;
        char file_buffer[512] = {0};
        while ((bytesRead = recv(clientSocket, file_buffer, sizeof(file_buffer), 0)) > 0) {
            if (containsSubstring(file_buffer, "###")) {
                send(sentuser_sock, file_buffer, strlen(file_buffer), 0);
                memset(file_buffer, 0, sizeof(file_buffer));
                string msg = "File has been shared to " + sentuser_name + ".\n";
                send(clientSocket, msg.c_str(), msg.length(), 0);
                return;
            }
            send(sentuser_sock, file_buffer, sizeof(file_buffer), 0);
        }
    }

    else {
        // sending message to particular user
        for (auto client : clients) {
            if (client.username == command and client.conn == true) {
                sub_message=username+" sent:"+sub_message;
                send(client.socket, sub_message.c_str(), sub_message.length(), 0);
                found = 1;
                break;
            }
        }
    }
    if (!found) {
        string msg = "Invalid Command/User is not online\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
    }
}

void InteractWithClient(int clientSocket, vector<ClientInfo> &clients) {
    char buffer[4096];

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break;
        }

        string message(buffer, bytesReceived);

        // Parsing the message into username, command, and sub_message
        size_t firstColonPos = message.find(":");
        size_t secondColonPos = message.find(":", firstColonPos + 1);

        string username, command, sub_message, extra;
        username = trim(message.substr(0, firstColonPos));
        command = trim(message.substr(firstColonPos + 1, secondColonPos - firstColonPos - 1));

        if (secondColonPos != string::npos) {
            size_t thirdColonPos = message.find(":", secondColonPos + 1);
            sub_message = trim(message.substr(secondColonPos + 1, thirdColonPos - secondColonPos - 1));
            extra = (thirdColonPos != string::npos) ? trim(message.substr(thirdColonPos + 1)) : "";
        }

        if (command == "creategroup") {
            createGroup(sub_message, username, clients, clientSocket); 
        } 
        
        else if (command == "addtogroup") {
            addUserToGroup(sub_message, extra, clients, clientSocket);  
        } 
        
        else if (command == "removefromgroup") {
            removeUserFromGroup(sub_message, extra, clients, clientSocket);  
        } 
        
        else if (command == "groupmsg") {
            sendMessageToGroup(sub_message, extra, clients, clientSocket); 
        }

        else
            commandExecution(command, sub_message, clientSocket, username, extra, clients);
    }

    // Handle disconnection of the client
    auto it = std::find_if(clients.begin(), clients.end(), [clientSocket](const ClientInfo &client) { return client.socket == clientSocket; });
    if (it != clients.end()) {
        cout << it->username << " has been disconnected" << endl;
        string msg = "\n" + it->username + " has been disconnected";
        for (const auto &client : clients) {
            if (client.socket != clientSocket && client.conn) {
                send(client.socket, msg.c_str(), msg.length(), 0);
            }
        }
        it->conn = false;
    }

    close(clientSocket);
}
