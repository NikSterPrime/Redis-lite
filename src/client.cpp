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

    while(true)
    {
        char msg[1024];
        cin.getline(msg, sizeof(msg));
        send(sock, msg, strlen(msg), 0);
        //need to make it such that message is sent and it can receive text over package
        recv(sock, msg, sizeof(msg),0);
        cout<<"Server Response: "<<msg<<endl;
    }

    closesocket(sock);
    WSACleanup();
}