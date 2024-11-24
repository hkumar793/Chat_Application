#ifndef FILEHANDLER_H
#define FILEHANDLER_H
// #define folder "files"

#include <string>

void file_command_handler(int socket, const std::string &name, const std::string &command, const std::string &filename, const std::string &sentuser);
void file_download_handler(int socket, const std::string &name, const std::string &command, const std::string &filename);
void file_transfer_handler(const std::string &sub_message, int socket);

#endif 
