#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <chrono>

std::string trim(const std::string &str);
std::string getCurrentDateTime();
std::string getPassword();
bool containsSubstring(const std::string &str, const std::string &substring);

#endif 
