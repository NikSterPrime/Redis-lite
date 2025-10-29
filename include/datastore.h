#ifndef DATASTORE_H
#define DATASTORE_H

#include <string>
using namespace std;
// Single entry: pass raw command; tokenization happens inside datastore.cpp
string commandExecute(const string &command);

#endif // DATASTORE_H