#include <iostream>
#include <string>
#include <winsock2.h>
#include "datastore.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

static void send_text(SOCKET s, const string &text) {
    if (!text.empty()) {
        send(s, text.c_str(), static_cast<int>(text.size()), 0);
    }
}

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

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
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

        cout << "Client connected." << endl;
        string lineBuffer;
        char tempBuf[1024];

        for (;;) {
            int received = recv(clientSocket, tempBuf, sizeof(tempBuf) - 1, 0);
            if (received <= 0) {
                break; // disconnect or error
            }

            tempBuf[received] = '\0';
            lineBuffer.append(tempBuf, received);

            size_t pos;
            while ((pos = lineBuffer.find('\n')) != string::npos) {
                string line = lineBuffer.substr(0, pos);
                lineBuffer.erase(0, pos + 1);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                if (line.empty()) continue;

                if (line == "EXIT")
                {
                    send_text(clientSocket, string("GOODBYE\r\n"));
                    goto close_client;
                }

                string ans = commandExecute(line);
                send_text(clientSocket, ans);
            }
        }

close_client:
        closesocket(clientSocket);
        cout << "Client disconnected." << endl;
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
