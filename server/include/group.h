#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <sstream>
#include "utils.h"  

using namespace std;

struct Group {
    string groupName;
    vector<string> members; 
};

unordered_map<string, Group> groups; 
