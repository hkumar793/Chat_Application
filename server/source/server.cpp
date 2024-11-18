#include <iostream>
#include <vector>
#include <thread>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include "client_handler.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[]) {
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        cout << "Socket creation failed" << endl;
        return 1;
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        cout << "Bind failed" << endl;
        close(listenSocket);
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == -1) {
        cout << "Listen failed" << endl;
        close(listenSocket);
        return 1;
    }

    cout << "Server has started, listening on port: " << atoi(argv[1]) << endl;

    vector<ClientInfo> clients;

    char name_buffer[100];

    while (true) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cout << "Invalid client socket" << endl;
            continue;
        }

        // managing user credentials and authentication
        string username, password;
        int len_client;
        if ((len_client = recv(clientSocket, name_buffer, sizeof(name_buffer), 0)) > 0) {
            string clientinfo(name_buffer, len_client);
            size_t firstColonPos = clientinfo.find(":");
            username = trim(clientinfo.substr(0, firstColonPos));
            password = trim(clientinfo.substr(firstColonPos + 1));
        } else {
            close(clientSocket);
            continue;
        }

        auto it = std::find_if(clients.begin(), clients.end(), [&username](const ClientInfo &client) 
                               { return client.username == username; });

        if (it != clients.end()) {
            if (password != it->password) {
                string msg = "Please enter correct password!!!";
                send(clientSocket, msg.c_str(), msg.length(), 0);
                close(clientSocket);
                continue;
            } else {
                string msg = "Successfully logged in!!!\n";
                cout << username << " has joined the chat." << endl;
                send(clientSocket, msg.c_str(), msg.length(), 0);
            }
        } else {
            cout << username << " has joined the chat." << endl;
            string message = username + " has come online.\n";

            // Broadcast the new user message to all connected clients
            for (const auto &client : clients) {
                if (client.conn) { // Only send to connected clients
                    send(client.socket, message.c_str(), message.length(), 0);
                }
            }
            string msg = "New user detected, saving password for future references!!!\n";
            send(clientSocket, msg.c_str(), msg.length(), 0);
        }

        clients.push_back({clientSocket, username, password, true});
        thread t(InteractWithClient, clientSocket, std::ref(clients));
        t.detach();
    }

    close(listenSocket);

    return 0;
}
