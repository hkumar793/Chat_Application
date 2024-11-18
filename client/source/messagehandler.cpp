#include "messagehandler.h"
#include "utils.h"
#include "filehandler.h"
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>

using namespace std;

void SendMsg(int socket) {
    string name;
    string message;

    cout << "Enter your name: ";
    getline(cin, name);
    string password = getPassword();

    string clientinfo = name + ":" + password;
    send(socket, clientinfo.c_str(), clientinfo.length(), 0);
    cout << endl;

    while (true) {
        getline(cin, message);
        string msg = name + " : " + message;

        if (containsSubstring(message, "fileupload") || containsSubstring(message, "fileshare")) {
            size_t firstColonPos = message.find(":");
            size_t secondColonPos = message.find(":", firstColonPos + 1);

            string command, filename, sentuser;
            command = trim(message.substr(0, firstColonPos));
            filename = trim(message.substr(firstColonPos + 1, secondColonPos - firstColonPos - 1));

            if (secondColonPos == string::npos)
                sentuser = "";
            else
                sentuser = trim(message.substr(secondColonPos + 1));

            file_command_handler(socket, name, command, filename, sentuser);
            continue;
        }

        int bytesSent = send(socket, msg.c_str(), msg.length(), 0);
        if (bytesSent == -1) {
            cout << "Error sending message" << endl;
            break;
        }

        if (message == "quit") {
            break;
        }
    }
    close(socket);
    exit(0);
}

void ReceiveMessage(int socket) {
    char buffer[4096] = {0};
    int recvLength;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        recvLength = recv(socket, buffer, sizeof(buffer), 0);
        if (recvLength <= 0) {
            cout << "Disconnected from server" << endl;
            break;
        }

        else if (containsSubstring(buffer, "###")) {
            string str(buffer);
            str = str.substr(0, str.length() - 3);
            file_transfer_handler(str, socket);
        }

        else {
            string msg(buffer, recvLength);
            cout << msg << endl;
        }
    }
    close(socket);
    exit(1);
}
