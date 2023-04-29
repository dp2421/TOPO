#include "pch.h"
#include "ServerBase.h"

ServerBase::ServerBase()
{
	InitServer();

	matchingManager = new MatchingManager(IOCPHandle);
	InitHandler();
}

ServerBase::~ServerBase()
{
	closesocket(ServerSocket);
	WSACleanup();
}

void ServerBase::Run()
{
	int numThreads = std::thread::hardware_concurrency();

	thread eventThread{ &ServerBase::EventThread, this };
	for (int i = 0; i < numThreads; ++i)
		workerThreads.emplace_back(&ServerBase::WorkerThread, this, IOCPHandle);

	eventThread.join();
	for (auto& thread : workerThreads)
		thread.join();
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
			Recv(static_cast<int>(ID), numBytes, overlappedEx);
			break;
		case OverlappedType::Send:
			delete overlappedEx;
			break;
		default:
			ServerEvent(overlappedEx);
			break;
		}
	}
}

void ServerBase::EventThread()
{
	Event event;
	event.objID = -1;
	while (true) {
		auto current_time = chrono::system_clock::now();
		if (event.objID == -1)
		{
			if (true == eventQueue.try_pop(event))
				continue;
			else
				event.objID = -1;
		}
		else
		{
			if (event.excuteTime > current_time) 
			{
				this_thread::sleep_for(1ms);
				continue;
			}
			switch (event.eventType)
			{
			case OverlappedType::Update:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = OverlappedType::Update;
				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			}
			continue;
		}
		this_thread::sleep_for(1ms);
	}
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
	::bind(ServerSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
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

	cout << "Start Server" << endl;
}

void ServerBase::InitHandler()
{
	// 패킷 내용 초기화
	// 패킷 처리 구조 초기화
}

void ServerBase::Accept()
{
	int newID = clientID++;

	clients[newID] = new Client;
	clients[newID]->ID = newID;
	clients[newID]->socket = ClientSocket;
	clients[newID]->position = Vector3(50, 100, 100);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(ClientSocket), IOCPHandle, newID, 0);
	clients[newID]->RecvPacket();
	ClientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	cout << "Accept Success" << endl;

	ZeroMemory(&GlobalOverlapped.overlapped, sizeof(GlobalOverlapped.overlapped));
	int addr_size = sizeof(SOCKADDR_IN);
	AcceptEx(ServerSocket, ClientSocket, GlobalOverlapped.sendBuf, 0, addr_size + 16, addr_size + 16, 0, &GlobalOverlapped.overlapped);
}

void ServerBase::Recv(const int id, DWORD recvByte, OverlappedEx* overlappedEx)
{
	int remainData = recvByte + clients[id]->prevRemainData;
	char* p = overlappedEx->wsaBuf.buf;
	while (remainData > 0) 
	{
		int packet_size = p[0];
		if (packet_size <= remainData) 
		{
			ProcessPacket(id, p);
			p = p + packet_size;
			remainData = remainData - packet_size;
		}
		else
		{
			break;
		}
	}
	clients[id]->prevRemainData = remainData;
	if (remainData > 0) 
	{
		memcpy(overlappedEx->sendBuf, p, remainData);
	}

	clients[id]->RecvPacket();
}

void ServerBase::ServerEvent(const int id, OverlappedEx* overlappedEx)
{
	auto client = clients[id];	
	switch (overlappedEx->type)
	{
	case OverlappedType::MatchingStart:
		break;
	case OverlappedType::MatchingComplete:
		break;
	case OverlappedType::Update:
		client->position = client->direction;
		break;
	default:
		break;
	}
}

void ServerBase::Disconnect(int ID)
{
	// 연결끊기
	// 게임중, 로비 구별
	// 게임중이면 같은 그룹에게 전달
	// 로비면 그냥 끊기
}

void ServerBase::ProcessPacket(const int id, char* packet)
{
	switch (packet[1])
	{
	case ClientLogin:
		clients[id]->SendServerLoginPacket(id);
		for (auto& client : clients)
		{
			if (client.second->ID == id) continue;

			client.second->SendAddPlayerPacket(id, clients[id]->position);
			clients[id]->SendAddPlayerPacket(client.second->ID, client.second->position);
		}
		break;
	case ClientKeyInput:
	{
		auto p = reinterpret_cast<ClientKeyInputPacket*>(packet);
		ProcessInput(id, p);
		break;
	}
	case ClientMove:
	{
		auto p = reinterpret_cast<ClientMovePacket*>(packet);
		Vector3 dir = Vector3(p->x, p->y, p->z);
		clients[id]->direction = dir;
		break;
	}
	case ClientMatching:
		
		break;
	}
}

void ServerBase::ProcessInput(const int id, ClientKeyInputPacket* packet)
{
	auto key = packet->key;
	Vector3 dir = Vector3(packet->x, packet->y, packet->z);
	auto client = clients[id];
	switch (key)
	{
	case KeyType::MoveStart: // 이동
	{
		client->direction = dir;
		client->position = client->direction;
		client->isMove = true;
		Event event{ id, OverlappedType::Update, chrono::system_clock::now() + DT};
		eventQueue.push(event);
		break;
	}	
	case KeyType::MoveEnd: 

		break;
	case KeyType::Jump: // 점프 

		break;
	case KeyType::Push: // 밀치기

		break;
	case KeyType::Boom: // 폭탄돌리기

		break;
	default:
		break;
	}

	//for (auto client : clients)
	//{
	//	client.second->SendPlayerInfoPacket(id, clients[id]->position, dir);
	//}
}

