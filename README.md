To start server execute following commands
1   make
2 ./server <port>

To start client execute following commands
1   make
2 ./client <port>

Various Commands
1. all -> To broadcast the message to online users and syntax -> all:<message>.
2. <send username>:<message> -> To send message to username <username>.
3. listuser -> To display online users connected to application and syntax -> listuser
4. fileupload -> to upload file on server and syntax -> fileupload:<filename>
5. filedownload -> to download file from server and syntax -> filedownload:<filename>
6. fileshare -> To share file between clients and syntax -> fileshare:<filename>:<send username>
