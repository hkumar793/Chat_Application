#include "utils.h"
#include <iostream>
#include <termios.h>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>

using namespace std;

bool containsSubstring(const std::string &str, const std::string &substring) {
    if (str.empty() || substring.empty()) {
        return false; 
    }
    return str.find(substring) != std::string::npos;  
}

string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

string getCurrentDateTime() {
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    tm localTime;
    localtime_r(&time, &localTime);
    stringstream dateTimeStream;
    dateTimeStream << put_time(&localTime, "%Y%m%d_%H%M%S");
    return dateTimeStream.str();
}

string getPassword() {
    string password;
    char ch;
    termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    cout << "Enter your password: ";
    while (true) {
        ch = getchar();
        if (ch == '\n' || ch == '\r') {
            break;
        }
        password += ch;
        cout << '*';
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cout << endl;

    return password;
}
