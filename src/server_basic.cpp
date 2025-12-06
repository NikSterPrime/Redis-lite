#include <iostream>
#include <string>
#include <winsock2.h>
#include "datastore.h"
#include <sys/time.h>
#include <chrono>

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

    if (listen(serverSocket, 64) == SOCKET_ERROR) {
        cerr << "listen() failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server started on port 8080" << endl;
    cout << "Waiting for incoming connections..." << endl;

    int clientSockets[FD_SETSIZE];
    for (int i = 0; i < FD_SETSIZE; i++) {
        clientSockets[i] = -1;
    }

    int num_clients = 0;
    fd_set readfds;

    while (true) {
        FD_ZERO(&readfds);

        FD_SET(serverSocket, &readfds);
        int max_fd = serverSocket;
	
        for (int i = 0; i < num_clients; i++) {
            if (clientSockets[i] != -1) {
                FD_SET(clientSockets[i], &readfds);
                if (clientSockets[i] > max_fd)
                    max_fd = clientSockets[i];
            }
        }
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(serverSocket, &readfds)) {
		auto start = std::chrono::high_resolution_clock::now();
            int newSocket = accept(serverSocket, NULL, NULL);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double,std::milli> fp_ms = end - start;
		cout << "Accepted in " << fp_ms.count() << "ms\n";
            if (newSocket == INVALID_SOCKET) {
                cerr << "accept() failed" << endl;
                break;
            }
            clientSockets[num_clients++] = newSocket;
            cout << "New client connected: FD " << newSocket << endl;
        }
        for (int i = 0; i < num_clients; i++) {
            int fd = clientSockets[i];

            if (fd != -1 && FD_ISSET(fd, &readfds)) {

                string lineBuffer;
                char tempBuf[1024];
                int received = recv(fd, tempBuf, sizeof(tempBuf) - 1, 0);
                if (received < 0) {
                    continue;
                }
		if (received == 0){
		    closesocket(fd);
		    clientSockets[i] = -1;
		    continue;
		}

                tempBuf[received] = '\0';
                lineBuffer.append(tempBuf, received);
		//cout<<"Final message :"<<lineBuffer<<endl;
                size_t pos;
                while ((pos = lineBuffer.find('\n')) != string::npos) {
                    string line = lineBuffer.substr(0, pos);
                    lineBuffer.erase(0, pos + 1);

                    if (!line.empty() && line.back() == '\n')
                        line.pop_back();
                    if (line.empty())
                        continue;

                    if (line == "EXIT") {
                        send_text(fd, "GOODBYE\r\n");

                        // Properly disconnect inside loop
                        closesocket(fd);
                        clientSockets[i] = -1;
                        cout << "Client disconnected." << endl;
                        break;
                    }

                    string ans = commandExecute(line);
                    send_text(fd, ans);
                }
            }
        }	
    }

    // close all clients on exit
    for (int i = 0; i < num_clients; i++) {
        if (clientSockets[i] != -1)
	    cout<<"Closed client socket "<<clientSockets[i]<<endl;
            closesocket(clientSockets[i]);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}



