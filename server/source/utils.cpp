#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <unistd.h>
#include <string>
#include "utils.h"
#include <iomanip>

using namespace std;

string getCurrentDateTime() {
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    tm localTime;
    localtime_r(&time, &localTime);
    stringstream dateTimeStream;
    dateTimeStream << put_time(&localTime, "%Y%m%d_%H%M%S");
    return dateTimeStream.str();
}

bool containsSubstring(const std::string &str, const std::string &substring) {
    return str.find(substring) != std::string::npos;
}

string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
