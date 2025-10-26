#ifndef DATASTORE_H
#define DATASTORE_H

#include <string>

// Single entry: pass raw command; tokenization happens inside datastore.cpp
void commandExecute(const std::string &command);

#endif // DATASTORE_H