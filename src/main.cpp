#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>
#include "datastore.h"
using namespace std;
std::vector<std::string> tokenize(const std::string &input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
int main() {
    unordered_map<string,string> store;

    cout<< "Welcome to the Redis-lite, here you need to use the SET, GET and DEL commands"<<endl<<"to enter values to the Redis store for faster and efficient retrieval."<<endl;
    cout<<"To exit the program, type EXIT"<<endl;

    while(true)
    {
        cout<< ">";
        string command;
        getline(cin,command);
        if(command == "EXIT")
        {
            cout<<"Exiting the Redis-lite. Goodbye!"<<endl;
            break;
        }
    }
    exit(0);
    return 0;
}