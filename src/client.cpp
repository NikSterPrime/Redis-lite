#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
using namespace std;
int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2),&wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM,0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sock,(sockaddr*)&serverAddr,sizeof(serverAddr)))
    {
        cout<<"Connection Failed"<<endl;
        return 1;
    }
    else{
        cout<<"Connected to server successfully"<<endl;
    }

    while(true)
    {
        string line;
        if (!getline(cin, line)) {
            break; // EOF or error
        }
        
        // Send command with newline terminator
        line += "\n";
        send(sock, line.c_str(), line.size(), 0);
        
        // Receive response
        char buffer[1024] = {0};
        int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            break; // Connection closed or error
        }
        buffer[received] = '\0';
        cout << "Server Response: " << buffer;
        
        // Check if server sent GOODBYE (EXIT command response)
        if (string(buffer).find("GOODBYE") != string::npos) {
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
}