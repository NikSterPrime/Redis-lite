#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include "datastore.h"
#include "server.h"
using namespace std;
int main(int argc, char* argv[]) {
    // Check for --server flag
    bool serverMode = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--server") == 0) {
            serverMode = true;
            break;
        }
    }

    if (serverMode) {
        // Run TCP server mode
        return TCPServer();
    }

    // Default: CLI mode
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
        if(command.size() == 0) continue;
        commandExecute(command);
    }
    return 0;
}