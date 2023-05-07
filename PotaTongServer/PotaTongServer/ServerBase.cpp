#include "pch.h"
#include "ServerBase.h"

ServerBase::ServerBase()
{
	InitServer();

	matchingManager = new MatchingManager(IOCPHandle);
	InitHandler();
	InitObsatacleInfo();
	InitMapInfo();
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
		
		const int id = static_cast<int>(ID);

		if (FALSE == ret)
		{
			if (OverlappedType::Accept == overlappedEx->type)
			{
				cout << "Accept Error" << endl;
			}
			else
			{
				cout << "GQCS Error on client [" << id << "]\n";
				Disconnect(id);
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
			Recv(id, numBytes, overlappedEx);
			break;
		case OverlappedType::Send:
			delete overlappedEx;
			break;
		default:
			ServerEvent(id, overlappedEx);
			break;
		}
	}
}

void ServerBase::EventThread()
{
	Event event;
	event.objID = -1;
	while (true) {
		auto currentTime = chrono::system_clock::now();
		if (event.objID == -1)
		{
			if (true == eventQueue.try_pop(event))
				continue;
			else
				event.objID = -1;
		}
		else
		{
			if (event.excuteTime > currentTime) 
			{
				eventQueue.push(event);
				this_thread::sleep_for(1ns);
				event.objID = -1;
				continue;
			}

			switch (event.eventType)
			{
			case OverlappedType::RotateObs:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = OverlappedType::RotateObs;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::Update:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = OverlappedType::Update;

				// 시간 보간
				//auto diff = chrono::duration<float>((currentTime - event.excuteTime) / DeltaTimeMilli);
				//memcpy(overlappedEx->sendBuf, &diff, sizeof(diff));

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			}
			continue;
		}
		this_thread::sleep_for(1ns);
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
	
	int option = TRUE;               
	setsockopt(ServerSocket,         
		IPPROTO_TCP,          
		TCP_NODELAY,          
		(const char*)&option, 
		sizeof(option));      

	cout << "Start Server" << endl;
}

void ServerBase::InitHandler()
{
	// 패킷 내용 초기화
	// 패킷 처리 구조 초기화
}

void ServerBase::InitObsatacleInfo()
{
	// 바이너리 파일 읽기
	ifstream inFile("obstacles.bin", std::ios::in | std::ios::binary);

	if (!inFile) {
		std::cerr << "Failed to open obstacles.bin" << std::endl;
		return;
	}

	while (!inFile.eof()) {
		ObstacleInfo obstacle;
		inFile.read(reinterpret_cast<char*>(&obstacle), sizeof(obstacle));
		obstacles.push_back(obstacle);
	}
	inFile.close();

	Event event{ 9999, OverlappedType::RotateObs, chrono::system_clock::now() + DeltaTimeMilli };
	eventQueue.push(event);
}

void ServerBase::InitMapInfo()
{
	const char* FileNames[] = { "MapPosition1FF.bin", "MapPosition1F.bin", "MapPosition2F.bin" };

	for (int i = 0; i < 3; ++i)
	{
		ifstream inFile(FileNames[i], std::ios::in | std::ios::binary);

		if (!inFile) {
			std::cerr << "Failed to open " <<  FileNames[i] << std::endl;
			return;
		}

		while (!inFile.eof()) {
			TileInfo tile;
			inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
			if (i == 0)
			{
				tile.xScale *= 4;
				tile.yScale *= 4;
				tile.zScale *= 4;
			}
			tiles.push_back(tile);
		}

		inFile.close();
	}
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
	switch (overlappedEx->type)
	{
	case OverlappedType::MatchingStart:
		break;
	case OverlappedType::MatchingComplete:
		break;
	case OverlappedType::RotateObs:
	{
		unsigned short degree[66] = { 0, };
		int i = 0;
		for (auto& obs : obstacles)
		{
			if (obs.data.state != OBSTACLE_STATE::STOP)
			{
				obs.rotate += obs.deltaRotate;
				if (obs.rotate > 360)
					obs.rotate -= 360;

				degree[i] = obs.rotate * 100;
				++i;
			}
		}

		for (auto client : clients)
		{
			if (client.second->ID != -1)
				client.second->SendObstacleInfoPacket(degree, 66 * sizeof(short));
		}
		
		Event event{ 9999, OverlappedType::RotateObs, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);
		
		break;
	}
	case OverlappedType::Update:
	{
		auto client = clients[id];
		if (client->ID < 0) return;
		{
			{
				lock_guard<mutex> lock{ client->lock };
				client->velocity.y -= GRAVITY;
			}
			if (client->isMove)
			{
				lock_guard<mutex> lock{ client->lock };
				auto delta = client->direction * SPEED;
				client->velocity.x = delta.x;
				client->velocity.z = delta.z;
			}

			if (client->velocity.y < 0)
			{
				for (auto& tile : tiles)
				{
					if (client->position.y > 0)
					{
						if (tile.data.state == LayerState::L1Water ||
							tile.data.state == LayerState::L1Part1 ||
							tile.data.state == LayerState::L1Part2 ||
							tile.data.state == LayerState::L1Part3 ||
							tile.data.state == LayerState::L1Part4)
							continue;
					}
					if (client->collider.isCollisionAABB(tile.collider))
					{
						if (client->position.y + abs(client->velocity.y) < tile.collider.position->y) break;

						lock_guard<mutex> lock{ client->lock };
						client->position.y = tile.collider.position->y;
						client->velocity.y = 0;
						if (client->isJump)
						{
							client->isJump = false;
						}
						break;
					}
				}
			}

			{
				lock_guard<mutex> lock{ client->lock };
				client->position += client->velocity * DeltaTimefloat.count();
				if (client->position.y < -2000)
				{
					client->velocity = Vector3::Zero();
					client->position = Vector3(50, 100, 100);
				}
			}
		}

		client->SendPlayerInfoPacket(id, client->position, client->degree, client->isMove);

		for (auto cl : clients)
		{
			ClientException(cl, id);

			cl.second->SendPlayerInfoPacket(id, client->position, client->degree, client->isMove);
		}

		Event event{ id, OverlappedType::Update, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);

		break;
	}
	default:
		break;
	}
}

void ServerBase::Disconnect(int ID)
{
	cout << "DISCONNECT \n";
	// 연결끊기
	// 게임중, 로비 구별
	// 게임중이면 같은 그룹에게 전달
	// 로비면 그냥 끊기
	auto client = clients[ID];	

	lock_guard<mutex> lock{ clients[ID]->lock };

	// 매칭 이후 추가
	//if (client->RoomID > 0)
	//{
	//
	//}

	for (auto cl : clients)
	{
		ClientException(cl, ID);

		cl.second->SendRemovePlayerPacket(ID);
	}

	closesocket(clients[ID]->socket);
	clients[ID]->ID = -1;
}

void ServerBase::ProcessPacket(const int id, char* packet)
{
	switch (packet[1])
	{
	case ClientLogin:
	{
		clients[id]->SendServerLoginPacket(id);
		for (auto& client : clients)
		{
			ClientException(client, id);

			client.second->SendAddPlayerPacket(id, clients[id]->position);
			clients[id]->SendAddPlayerPacket(client.second->ID, client.second->position);
		}
		Event event{ id, OverlappedType::Update, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);
		break;
	}
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
		clients[id]->degree = p->degree;
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
	client->degree = packet->degree;
	switch (key)
	{
	case KeyType::MoveStart: // 이동
	{
		{
			lock_guard<mutex> lock{ client->lock };
			client->direction = dir;
			client->isMove = true;
		}
		break;
	}	
	case KeyType::MoveEnd:
	{
		{
			lock_guard<mutex> lock{ client->lock };
			client->direction = Vector3::Zero();
			client->velocity.x = 0;
			client->velocity.z = 0;
			client->isMove = false;
		}
		break;
	}
	case KeyType::Jump: // 점프 
	{
		if (!client->isJump)
		{
			client->isJump = true;
			client->velocity.y = JUMPVEL;
		}
		break;
	}
	case KeyType::Push: // 밀치기

		break;
	case KeyType::Boom: // 폭탄돌리기

		break;
	default:
		break;
	}
}

