#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <vector>

// Structure of client information
struct ClientInfo {
    int socket;
    std::string username;
    std::string password;
    bool conn;
    vector<string> groups;
};

void InteractWithClient(int clientSocket, std::vector<ClientInfo> &clients);
void commandExecution(std::string command, std::string sub_message, int clientSocket, std::string username, std::string extra, std::vector<ClientInfo> &clients);

#endif 