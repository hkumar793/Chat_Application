#include "messagehandler.h"
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1){
        cout << "Socket creation failed" << endl;
        return 1;
    }

    sockaddr_in serverAddr;
    string serverAddress = "127.0.0.1";

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = inet_addr(serverAddress.c_str());

    if (connect(socketFD, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
        cout << "Not able to connect to server" << endl;
        close(socketFD);
        return 1;
    }

    thread senderThread(SendMsg, socketFD);
    thread receiverThread(ReceiveMessage, socketFD);

    senderThread.join();
    receiverThread.join();

    return 0;
}
