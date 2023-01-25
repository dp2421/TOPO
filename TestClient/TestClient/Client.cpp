#include <iostream>
#include <WS2tcpip.h>

#include "../../Protocol.h"

using namespace std;
#pragma comment(lib, "WS2_32.LIB")

WSAOVERLAPPED s_over;
SOCKET Socket;
WSABUF s_wsabuf[1];
char   s_buf[BUFFERSIZE];

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	Socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN svr_addr;
	memset(&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(PORTNUM);
	inet_pton(AF_INET, "127.0.0.1", &svr_addr.sin_addr);
	WSAConnect(Socket, reinterpret_cast<sockaddr*>(&svr_addr), sizeof(svr_addr), 0, 0, 0, 0);
	while (true) SleepEx(100, true);
	closesocket(Socket);
	WSACleanup();
}
