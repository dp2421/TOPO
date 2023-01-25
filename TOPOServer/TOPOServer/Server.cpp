#include <iostream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>

#include "Client.h"
#include "../../Protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

SOCKET ServerSocket, ClientSocket;
HANDLE IOCPHandle;
OverlappedEx GlobalOverlapped;

void ProcessPacket()
{

}

void WorkerThread(HANDLE IOCP)
{
	while (true)
	{
		DWORD numBytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(IOCP, &numBytes, &key, &over, INFINITE);
		OverlappedEx* overlappedEx = reinterpret_cast<OverlappedEx*>(over);

		switch (overlappedEx->type)
		{
		case OverlappedType::Accept:
			cout << "Accept Success" << endl;
			break;
		case OverlappedType::recv:
			ProcessPacket();
			break;
		case OverlappedType::send:
			break;
		default:
			break;
		}
	}
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORTNUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(ServerSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(ServerSocket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(ServerSocket), IOCPHandle, 9999, 0);
	ClientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	GlobalOverlapped.type = OverlappedType::Accept;
	AcceptEx(
		ServerSocket, 
		ClientSocket, 
		GlobalOverlapped.sendBuf, 
		0, 
		addr_size + 16, 
		addr_size + 16, 
		0, 
		&GlobalOverlapped.overlapped
	);

	vector <thread> workerThreads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		workerThreads.emplace_back(WorkerThread, IOCPHandle);

	for (auto& th : workerThreads)
		th.join();
	closesocket(ServerSocket);
	WSACleanup();
}