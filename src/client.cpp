#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
using namespace std;
int client_call()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2),&wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM,0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock,(sockaddr*)&serverAddr,sizeof(serverAddr));

    while(true)
    {
        char* msg;
        cin>>msg;
        send(sock, msg, strlen(msg), 0);
    }

    closesocket(sock);
    WSACleanup();
}