#include "datastore.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <shared_mutex>
#include <mutex>
using namespace std;

//Declare global variables
static unordered_map<string,string> datastore;
static shared_mutex mutx;

//called by commandExecute
std::vector<std::string> tokenize(const std::string &input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
void setHandler(const vector<string> &tokens)
{
    unique_lock<shared_mutex> lock(mutx);
    if(tokens.size() != 3)
    {
        cout<<"Error: SET command requires exactly 2 arguments."<<endl;
        return;
    }
    string key = tokens[1];
    string value = tokens[2];
    datastore[key] = value;
    cout<<value<<" stored for "<<key<<endl;
}
void getHandler(const vector<string> &tokens)
{
    shared_lock<shared_mutex> lock(mutx);
    if(tokens.size() != 2)
    {
        cout<<"Error: GET expects exactly 1 argument."<<endl;
        return;
    }
    string key = tokens[1];
    if(datastore.find(key) != datastore.end())
    {
        cout<<datastore[key]<<endl;
        return;
    }
    else{
        cout<<"Argument doesnt exist in store"<<endl;
        return;
    }
    
}
void delHandler(const vector<string> &tokens)
{
    unique_lock<shared_mutex> lock(mutx);
    if(tokens.size() != 2)
    {
        cout<<"Error: DEL expects exactly 1 argument."<<endl;
        return;
    }
    string key = tokens[1];
    if(datastore.find(key) != datastore.end())
    {
        datastore.erase(key);
        cout<<"Key "<<key<<" deleted successfully."<<endl;
        return;
    }
    else{
        cout<<"Argument doesnt exist in store"<<endl;
        return;
    }
    
}

// Single entry point: take raw command, tokenize here, then dispatch
void commandExecute(const string &command)
{
    const vector<string> tokens = tokenize(command);
    if (tokens.empty()) return;
    string cmd = tokens[0];
    if(cmd == "SET")
    {   
        setHandler(tokens);
    }
    else if(cmd == "GET")
    {
        getHandler(tokens);
    }
    else if(cmd == "DEL")
    {
        delHandler(tokens);
    }
    else if(cmd == "HELP")
    {
        cout<<"Available commands:"<<endl;
        cout<<"SET <key> <value> : Store the value for the given key."<<endl;
        cout<<"GET <key>         : Retrieve the value for the given key."<<endl;
        cout<<"DEL <key>         : Delete the given key from the store."<<endl;
        cout<<"EXIT              : Exit the program."<<endl;
    }
    else{
        cout<<"Error: Unknown command"<<endl;
    }

}