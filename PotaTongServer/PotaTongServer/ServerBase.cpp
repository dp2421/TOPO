#include "pch.h"
#include "ServerBase.h"

ServerBase::ServerBase()
{
	InitServer();


	int numThreads = std::thread::hardware_concurrency();

	thread eventThread{ &ServerBase::EventThread, this };
	for (int i = 0; i < numThreads; ++i)
		workerThreads.emplace_back(&ServerBase::WorkerThread, this, IOCPHandle);

	eventThread.join();
	for (auto& thread : workerThreads)
		thread.join();
}

ServerBase::~ServerBase()
{
	closesocket(ServerSocket);
	WSACleanup();
}

void ServerBase::WorkerThread(HANDLE IOCP)
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
				Disconnect(static_cast<int>(ID));
				if (overlappedEx->type == OverlappedType::Send) delete overlappedEx;
				continue;
			}
		}

		switch (overlappedEx->type)
		{
		case OverlappedType::Accept:
			Accept();
			break;
		case OverlappedType::Recv:
			//ProcessPacket(static_cast<int>(ID), overlappedEx->sendBuf);
			break;
		case OverlappedType::Send:
			break;
		case OverlappedType::ServerEvent:
			break;
		default:
			break;
		}
	}
}

void ServerBase::EventThread()
{

}

void ServerBase::InitServer()
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

void ServerBase::InitHandler()
{
	// 패킷 내용 초기화
	// 패킷 처리 구조 초기화
}

void ServerBase::Accept()
{
	cout << "Accept Success" << endl;
}

void ServerBase::Disconnect(int ID)
{
	// 연결끊기
	// 게임중, 로비 구별
	// 게임중이면 같은 그룹에게 전달
	// 로비면 그냥 끊기
}

