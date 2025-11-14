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
string setHandler(const vector<string> &tokens)
{
    if(tokens.size() != 3)
    {
        return "Error: SET command requires exactly 2 arguments.";
    }
    string key = tokens[1];
    string value = tokens[2];
    datastore[key] = value;
    return "Value " + value +" for key "+ key +" set successfully.";
}
string getHandler(const vector<string> &tokens)
{
    if(tokens.size() != 2)
    {
        return "Error: GET expects exactly 1 argument.";
    }
    string key = tokens[1];
    if(datastore.find(key) != datastore.end())
    {
        return datastore[key];
    }
    else{
        return "Error: Key " + key + " not found.";
    }
    
}
string delHandler(const vector<string> &tokens)
{
    if(tokens.size() != 2)
    {
        return "Error: DEL expects exactly 1 argument.";
    }
    string key = tokens[1];
    if(datastore.find(key) != datastore.end())
    {
        datastore.erase(key);
        return "Key " + key + " deleted successfully.";
    }
    else{
        return "Error: Key " + key + " not found.";
    }
    
}

// Single entry point: take raw command, tokenize here, then dispatch
string commandExecute(const string &command)
{
    const vector<string> tokens = tokenize(command);
    if (tokens.empty()) return "Error: Empty command.";
    string cmd = tokens[0];
    string response;
    if(cmd == "SET")
    {   
        response =  setHandler(tokens);
    }
    else if(cmd == "GET")
    {
        response = getHandler(tokens);
    }
    else if(cmd == "DEL")
    {
        response = delHandler(tokens);
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
    return response;

}
