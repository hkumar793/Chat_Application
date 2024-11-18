#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>

void file_command_handler(int socket, const std::string &name, const std::string &command, const std::string &filename, const std::string &sentuser);
void file_transfer_handler(const std::string &sub_message, int socket);

#endif 
