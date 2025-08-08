# Terminal Chat App (C++ TCP)

A terminal-based multi-client chat system built in C++ using TCP sockets. It supports real-time messaging, file transfer, and password-based login.

---

## Features

- Username-password login (stored on server)
- Private & broadcast messaging
- Chat history (`<username>_chat.txt`)
- File upload/download
- File sharing with specific users
- List of online users

---

## Getting Started

### Run Server

```bash
cd server
make
./server <port>
```

### Run Client

```bash
cd client
make
./client <server_ip> <port>
```

---

## Commands

| Command           | Format                                       |
|-------------------|----------------------------------------------|
| Broadcast          | `all:<message>`                              |
| Private Message    | `<username>:<message>`                       |
| List Users         | `listuser`                                   |
| Upload File        | `fileupload:<filename>`                      |
| Download File      | `filedownload:<filename>`                    |
| Share File         | `fileshare:<filename>:<target_username>`     |

---

## Login System

- First-time users create a password.
- Passwords are stored on the server.
- Only one session per user is allowed.

---

## Structure

```
Chat_Application/
├── client/
│   ├── client.cpp
│   └── Makefile
├── server/
│   ├── server.cpp
│   ├── files/               # stores uploaded files
│   └── Makefile
└── README.md
```

---
