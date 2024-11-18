void createGroup(const string &groupName, const string &username, vector<ClientInfo> &clients, int clientSocket) {
    // Check if the group already exists
    if (groups.find(groupName) != groups.end()) {
        string msg = "Group already exists.\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
        return;
    }

    // Create the new group and add the user
    Group newGroup;
    newGroup.groupName = groupName;
    newGroup.members.push_back(username);

    // Add the group to the global group map
    groups[groupName] = newGroup;

    // Add the group to the user's list of groups
    auto client = find_if(clients.begin(), clients.end(), [&username](const ClientInfo &client) {
        return client.username == username;
    });

    if (client != clients.end()) {
        client->groups.push_back(groupName);
    }

    string msg = "Group " + groupName + " created successfully.\n";
    send(clientSocket, msg.c_str(), msg.length(), 0);
}

// Function to add a user to a group
void addUserToGroup(const string &groupName, const string &username, vector<ClientInfo> &clients, int clientSocket) {
    // Check if the group exists
    if (groups.find(groupName) == groups.end()) {
        string msg = "Group does not exist.\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
        return;
    }

    // Check if the user is already in the group
    if (find(groups[groupName].members.begin(), groups[groupName].members.end(), username) != groups[groupName].members.end()) {
        string msg = "User is already a member of the group.\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
        return;
    }

    // Add user to the group
    groups[groupName].members.push_back(username);

    // Add the group to the user's list of groups
    auto client = find_if(clients.begin(), clients.end(), [&username](const ClientInfo &client) {
        return client.username == username;
    });

    if (client != clients.end()) {
        client->groups.push_back(groupName);
    }

    string msg = "User " + username + " added to group " + groupName + " successfully.\n";
    send(clientSocket, msg.c_str(), msg.length(), 0);
}

// Function to remove a user from a group
void removeUserFromGroup(const string &groupName, const string &username, vector<ClientInfo> &clients, int clientSocket) {
    // Check if the group exists
    if (groups.find(groupName) == groups.end()) {
        string msg = "Group does not exist.\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
        return;
    }

    // Check if the user is in the group
    auto &members = groups[groupName].members;
    auto it = find(members.begin(), members.end(), username);
    if (it == members.end()) {
        string msg = "User is not a member of the group.\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
        return;
    }

    // Remove user from the group
    members.erase(it);

    // Remove the group from the user's list of groups
    auto client = find_if(clients.begin(), clients.end(), [&username](const ClientInfo &client) {
        return client.username == username;
    });

    if (client != clients.end()) {
        auto groupIt = find(client->groups.begin(), client->groups.end(), groupName);
        if (groupIt != client->groups.end()) {
            client->groups.erase(groupIt);
        }
    }

    string msg = "User " + username + " removed from group " + groupName + " successfully.\n";
    send(clientSocket, msg.c_str(), msg.length(), 0);
}

// Function to send a message to all members of a group
void sendMessageToGroup(const string &groupName, const string &message, vector<ClientInfo> &clients, int clientSocket) {
    // Check if the group exists
    if (groups.find(groupName) == groups.end()) {
        string msg = "Group does not exist.\n";
        send(clientSocket, msg.c_str(), msg.length(), 0);
        return;
    }

    // Send the message to all members of the group
    for (const auto &member : groups[groupName].members) {
        auto client = find_if(clients.begin(), clients.end(), [&member](const ClientInfo &client) {
            return client.username == member;
        });

        if (client != clients.end()) {
            string fullMessage = "Message to " + groupName + " from " + member + ": " + message + "\n";
            send(client->socket, fullMessage.c_str(), fullMessage.length(), 0);
        }
    }
}
