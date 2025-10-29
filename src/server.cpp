#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>
#include "datastore.h"

#pragma comment(lib, "ws2_32.lib") // Winsock Library (for MSVC; harmless with MinGW)

using namespace std;

int main() {
    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "socket() failed" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "bind() failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 16) == SOCKET_ERROR) {
        cerr << "listen() failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server started on port 8080" << endl;
    cout << "Waiting for incoming connections..." << endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "accept() failed" << endl;
            break;
        }

        thread clientThread([clientSocket]() {
            string lineBuffer;  // accumulator for partial lines
            char tempBuf[1024];
            string response;
            while (true) {
                int received = recv(clientSocket, tempBuf, sizeof(tempBuf) - 1, 0);
                if (received <= 0) {
                    // Client disconnected or error
                    break;
                }
                
                // Append received bytes to accumulator
                tempBuf[received] = '\0';
                lineBuffer.append(tempBuf, received);
                
                // Process all complete lines (delimited by '\n')
                size_t pos;
                while ((pos = lineBuffer.find('\n')) != string::npos) {
                    // Extract one line
                    string line = lineBuffer.substr(0, pos);
                    lineBuffer.erase(0, pos + 1);
                    
                    // Trim trailing '\r' if present (handle CRLF)
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    
                    // Skip empty lines
                    if (line.empty()) {
                        continue;
                    }
                    
                    // Check for EXIT command
                    if (line == "EXIT") {
                        response = commandExecute(line);  // Let it print goodbye message
                        closesocket(clientSocket);
                        return;
                    }
                    
                    // Execute the command
                    response = commandExecute(line);
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
            }
            
            closesocket(clientSocket);
        });
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}