#include <iostream>
#include <mutex>
#include <thread>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <vector>
#include <concurrent_unordered_map.h>
#include <queue>

#include "Client.h"
#include "ThreadSafeQueue.h"
#include "../../Protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

SOCKET ServerSocket, ClientSocket;
HANDLE IOCPHandle;
OverlappedEx GlobalOverlapped;

concurrency::concurrent_unordered_map<int, Client> clinets;

int roomID;
bool matchingState;
mutex matchingLock;

void Matching()
{
	// �����ͷ� ����
	// Ŭ���̾�Ʈ : ��Ī��Ŷ ����
	// ��Ī�ϰڴٰ� �� �ֵ� ��� ����
	//	��Ī ������ ������� ť �־ �����ϸ� �ɵ�
	//	��Ī ����Ҷ� ��� ���� ? << ť�δ� �����ϱ� ����
	// 
	//	��Ī �ȵ��ư��� ����->�̺�Ʈ�� ��Ī �߰�, ���ť�� �߰�
	//	��Ī ���ư�����->�׳� ť���� ����
	//	��Ī �Ϸ������
	//	����ο� ����->�ٽ� �̺�Ʈ �߰�
}

void MatchingComplete()
{
	// GameStart
}

void ProcessPacket(int key, char* packet)
{
	// ��Ŷ �۾�
	switch (packet[1])
	{
	case ClientMatching:
		Matching();
		break;
	}
}

void AcceptClient()
{
	cout << "Accept Success" << endl;
}

void DisconnectClient(int id)
{
	// �������
	// ������, �κ� ����
	// �������̸� ���� �׷쿡�� ����
	// �κ�� �׳� ����
}

void EventThread()
{
	// ������Ʈ ���� ���� 1/frame �ð� ��ŭ ���
}

void WorkerThread(HANDLE IOCP)
{
	while (true)
	{
		DWORD numBytes;
		ULONG_PTR ID;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(IOCP, &numBytes, &ID, &over, INFINITE);
		OverlappedEx* overlappedEx = reinterpret_cast<OverlappedEx*>(over);

		if (FALSE == ret) 
		{
			if (OverlappedType::Accept == overlappedEx->type)
			{
				cout << "Accept Error" << endl;
			}
			else 
			{
				cout << "GQCS Error on client [" << static_cast<int>(ID) << "]\n";
				DisconnectClient(static_cast<int>(ID));
				if (overlappedEx->type == OverlappedType::send) delete overlappedEx;
				continue;
			}
		}

		switch (overlappedEx->type)
		{
		case OverlappedType::Accept:
			AcceptClient();
			break;
		case OverlappedType::recv:
			ProcessPacket(static_cast<int>(ID), overlappedEx->sendBuf);
			break;
		case OverlappedType::send:
			break;
		default:
			break;
		}
	}
}

void ServerSetting()
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
}

int main()
{
	ServerSetting();

	vector <thread> workerThreads;
	int numThreads = std::thread::hardware_concurrency();

	thread eventThread{ EventThread };
	for (int i = 0; i < numThreads; ++i)
		workerThreads.emplace_back(WorkerThread, IOCPHandle);

	eventThread.join();
	for (auto& thread : workerThreads)
		thread.join();


	closesocket(ServerSocket);
	WSACleanup();
}