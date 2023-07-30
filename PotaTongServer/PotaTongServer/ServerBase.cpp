#include "pch.h"
#include "ServerBase.h"

ServerBase::ServerBase()
{
	InitServer();

	matchingManager = new MatchingManager(IOCPHandle);
	InitHandler();
	InitObsatacleInfo();
	InitMapInfo();
	InitItemInfo();
	InitMeteoInfo();
	InitJumpMapInfo();
	InitNaviInfo();
	//InitAI();
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
				std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
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
			delete overlappedEx;
			break;
		}
	}
}

void ServerBase::EventThread()
{
	Event rotateObs{ 9999, OverlappedType::RotateObs, chrono::system_clock::now() + DeltaTimeMilli };
	eventQueue.push(rotateObs);
	Event sendRotateInfo{ 9999, OverlappedType::SendRotateInfo, chrono::system_clock::now() + DeltaTimeMilli };
	eventQueue.push(sendRotateInfo);

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
			default:
			{
				// 시간 보간
				//auto diff = chrono::duration<float>((currentTime - event.excuteTime) / DeltaTimeMilli);
				//memcpy(overlappedEx->sendBuf, &diff, sizeof(diff));

				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
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
	ServerSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORTNUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	::bind(ServerSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(ServerSocket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	int option = TRUE;
	setsockopt(ServerSocket,
		IPPROTO_TCP,
		TCP_NODELAY,
		(const char*)&option,
		sizeof(option));

	IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(ServerSocket), IOCPHandle, 9999, 0);
	ClientSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
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

void ServerBase::InitObsatacleInfo()
{
	// 바이너리 파일 읽기
	ifstream inFile("obstacles.bin", std::ios::in | std::ios::binary);

	if (!inFile) {
		std::cerr << "Failed to open obstacles.bin" << std::endl;
		return;
	}

	int i = 1;
	while (!inFile.eof()) {
		ObstacleInfo obstacle;
		inFile.read(reinterpret_cast<char*>(&obstacle), sizeof(obstacle));
		obstacles.emplace_back(obstacle, i);
		if (obstacle.state != OBSTACLE_STATE::STOP) ++i;
	}
	obstacles.pop_back();
	inFile.close();
}

void ServerBase::InitMapInfo()
{
	const char* FileNames = { "FinalRacingPos.bin" };

	ifstream inFile(FileNames, std::ios::in | std::ios::binary);

	if (!inFile) {
		std::cerr << "Failed to open " << FileNames << std::endl;
		return;
	}

	while (!inFile.eof()) {
		TileInfo tile;
		inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
		tiles.push_back(tile);
	}
	tiles.pop_back();

	inFile.close();
}

void ServerBase::InitNaviInfo()
{
	{
		const char* FileNames = { "move2F.bin" };

		ifstream inFile(FileNames, std::ios::in | std::ios::binary);

		if (!inFile) {
			std::cerr << "Failed to open " << FileNames << std::endl;
			return;
		}

		while (!inFile.eof()) {
			TileInfo tile;
			inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
			navi2F.push_back(tile);
		}
		navi2F.pop_back();

		inFile.close();
	}
	{
		const char* FileNames = { "move1F.bin" };

		ifstream inFile(FileNames, std::ios::in | std::ios::binary);

		if (!inFile) {
			std::cerr << "Failed to open " << FileNames << std::endl;
			return;
		}

		while (!inFile.eof()) {
			TileInfo tile;
			inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
			navi1F.push_back(tile);
		}
		navi1F.pop_back();

		inFile.close();
	}
}

void ServerBase::InitItemInfo()
{
	const char* FileNames = { "RacingMapItemSV.bin" };

	ifstream inFile(FileNames, std::ios::in | std::ios::binary);

	if (!inFile) {
		std::cerr << "Failed to open " << FileNames << std::endl;
		return;
	}

	while (!inFile.eof()) {
		TileInfo tile;
		inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
		if (tile.state == LayerState::LCoin)
		{
			coins.push_back(tile);
		}
		else if (tile.state == LayerState::L1Sujum)
		{
			superJump.push_back(tile);
		}
	}
	coins.pop_back();

	inFile.close();
}

void ServerBase::InitMeteoInfo()
{
	const char* FileNames[] = { "LMetorCenterSV.bin", "LMetorGrassSV.bin", "LMetorStoneSV.bin", "LMetorWaterSV.bin", "LMetorWoodSV.bin" };

	for (int i = 0; i < 5; ++i)
	{
		ifstream inFile(FileNames[i], std::ios::in | std::ios::binary);

		if (!inFile) {
			std::cerr << "Failed to open " << FileNames[i] << std::endl;
			return;
		}

		while (!inFile.eof()) {
			MetorTile tile;
			inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
			meteoTiles.push_back(tile);
		}

		inFile.close();
	}
}

void ServerBase::InitJumpMapInfo()
{
	{
		const char* FileNames = { "JumpMap.bin" };

		ifstream inFile(FileNames, std::ios::in | std::ios::binary);

		if (!inFile) {
			std::cerr << "Failed to open " << FileNames << std::endl;
			return;
		}

		while (!inFile.eof()) {
			TileInfo tile;
			inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
			jumpMapTiles.push_back(tile);
		}
		jumpMapTiles.pop_back();

		inFile.close();
	}

	{
		const char* FileNames = { "Jumpobstacles.bin" };

		ifstream inFile(FileNames, std::ios::in | std::ios::binary);

		if (!inFile) {
			std::cerr << "Failed to open " << FileNames << std::endl;
			return;
		}

		while (!inFile.eof()) {
			JumpObstacleObject obs;
			inFile.read(reinterpret_cast<char*>(&obs), sizeof(obs));
			jumpMapObstacle.push_back(obs);
		}
		jumpMapObstacle.pop_back();

		inFile.close();
	}
}

void ServerBase::InitAI(int roomID, MapType mapType, int AINum)
{
	for (int i = AINum; i < RacingMAX; ++i)
	{
		int newID = clientID++;

		clients[newID] = new Client;
		clients[newID]->ID = newID;
		clients[newID]->isAI = true;
		clients[newID]->RoomID = roomID;
		clients[newID]->mapType = mapType;
		clients[newID]->position = PlayerStartPos;
		clients[newID]->position.x += (PlayerStartDistance * i);

		Event update{ clients[newID]->ID, OverlappedType::Update, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(update);
	}
}

void ServerBase::Accept()
{
	int newID = clientID++;

	clients[newID] = new Client;
	clients[newID]->ID = newID;
	clients[newID]->socket = ClientSocket;
	clients[newID]->position = Vector3::Zero();
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(ClientSocket), IOCPHandle, newID, 0);
	clients[newID]->RecvPacket();

	ClientSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
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
		int packet_size = (PACKETSIZE)p[0];
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
	case OverlappedType::GameStartCount:
	{
		GameStartCount(id);
		break;
	}
	case OverlappedType::GameEnd:
	{
		GameEnd(id);
		break;
	}
	case OverlappedType::MatchingRacingComplete:
	{
		isFeverByRoomID[id] = false;
		if (isCoinActiveByRoomID.find(id) == isCoinActiveByRoomID.end())
		{
			isCoinActiveByRoomID[id] = new bool[2];
			isCoinActiveByRoomID[id][0] = false;
			isCoinActiveByRoomID[id][1] = false;
		}

		int connectClient = matchingManager->CompleteMatching(id, MapType::Racing);
		remainingUnReadyClientNumByRoomID[id] = connectClient;
		InitAI(id, MapType::Racing, connectClient);
		startCountByRoomID[id] = 3;
		break;
	}
	case OverlappedType::MatchingObstacleComplete:
	{
		isFeverByRoomID[id] = false;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> x(2, 3);

		auto mapType = static_cast<MapType>(x(gen));
		int connectClient = matchingManager->CompleteMatching(id, mapType);
		remainingUnReadyClientNumByRoomID[id] = connectClient;
		startCountByRoomID[id] = 3;
		if (mapType == MapType::Meteo)
		{
			for(int i = 0; i < 5; ++i)
				isGroundByRoomID[id].push_back(true);
		}
		break;
	}
	case OverlappedType::RotateObs:
	{
		for (auto& obs : obstacles)
		{
			if (obs.data.state != OBSTACLE_STATE::STOP)
			{
				obs.rotate += obs.deltaRotate;
				if (obs.rotate > 360)
					obs.rotate -= 360;
			}
		}

		for (auto& obs : jumpMapObstacle)
		{
			obs.rotate += obs.deltaRotate;
			if (obs.rotate > 360)
				obs.rotate -= 360;
		}

		Event event{ 9999, OverlappedType::RotateObs, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);

		break;
	}
	case OverlappedType::SendRotateInfo:
	{
		unsigned short degree[OBSTACLENUM] = { 0, };
		int i = 0;
		for (auto& obs : obstacles)
		{
			if (obs.data.state != OBSTACLE_STATE::STOP)
			{
				degree[i] = obs.rotate * 100;
				++i;
			}
		}

		unsigned short jumpDegree = 0;
		for (auto& obs : jumpMapObstacle)
		{
			jumpDegree = obs.rotate * 100;
		}

		for (auto client : clients)
		{
			if (client.second->RoomID == -1) continue;
			if (client.second->mapType == MapType::Racing)
			{
				if (client.second->ID != -1 && !client.second->isAI)
					client.second->SendObstacleInfoPacket(degree, OBSTACLENUM * sizeof(short));
			}
			else if (client.second->mapType == MapType::Jump)
			{
				if (client.second->ID != -1 && !client.second->isAI)
					client.second->SendJumpObstacleInfoPacket(jumpDegree);
			}
		}

		Event event{ 9999, OverlappedType::SendRotateInfo, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);

		break;
	}
	case OverlappedType::Update:
	{
		auto client = clients[id];

		if (client->ID == -1 || client->RoomID == -1)
		{
			break;
		}

		client->velocity.x = 0;
		client->velocity.z = 0;
		{
			lock_guard<mutex> lock{ client->lock };
			client->velocity.y -= GRAVITY * (isFeverByRoomID[client->ID] ? 3 : 1);
		}
		if (client->isMove && !client->isPushed)
		{
			lock_guard<mutex> lock{ client->lock };
			auto delta = client->direction * SPEED * (isFeverByRoomID[client->ID] ? FeverModeMulti : 1);
			client->velocity.x = delta.x;
			client->velocity.z = delta.z;
		}
		else if (client->isPushed)
		{
			lock_guard<mutex> lock{ client->lock };
			auto delta = client->direction * SPEED / 3 * (isFeverByRoomID[client->ID] ? FeverModeMulti : 1);
			client->velocity.x = delta.x;
			client->velocity.z = delta.z;
		}

		if (client->mapType == MapType::Racing)
		{
			// 바닥 충돌체크
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
						if (client->position.y + abs(client->velocity.y) < tile.collider.position->y) continue;

						//if(tile.data.state == LayerState::L1Water)
						//	cout << "Tile : " << tile.data.state << endl;

						lock_guard<mutex> lock{ client->lock };
						client->position.y = tile.collider.position->y;
						client->velocity.y = 0;
						if (client->isJump)
						{
							client->isJump = false;
						}
					}
				}
			}

			client->isColl = false;
			// 장애물 충돌체크
			for (auto& obs : obstacles)
			{
				if (client->velocity.x != 0 || client->velocity.z != 0)
				{
					if (obs.data.state == OBSTACLE_STATE::SPIN || obs.data.state == OBSTACLE_STATE::STOP)
					{
						if (client->collider.isCollisionAABB(obs.collider[0]))
						{
							client->isColl = true;

							// BoundingBox Side 별 거리 계산 후 가장 가까운 방향의 Normal Vector 세팅 후 반환
							auto clientCenter = client->collider.getBoundingbox().Center;
							XMVECTOR center = XMLoadFloat3(&clientCenter);
							XMVECTOR faceNormal = client->collider.GetClosestFaceNormal(obs.collider[0].getBoundingbox(), center);

							if (XMVectorGetX(faceNormal) < 0.0f)
							{
								lock_guard<mutex> lock{ client->lock };
								if (client->position.x > obs.position.x)
									client->position.x = obs.position.x + obs.collider[0].size.x + (PlayerCollider.x);
								else
									client->position.x = obs.position.x - obs.collider[0].size.x - (PlayerCollider.x);
								client->direction.x = 0;
								client->velocity.x = 0;
							}
							else if (XMVectorGetZ(faceNormal) < 0.0f)
							{
								lock_guard<mutex> lock{ client->lock };
								if (client->position.z > obs.position.z)
									client->position.z = obs.position.z + obs.collider[0].size.z + (PlayerCollider.z + 0.1);
								else
									client->position.z = obs.position.z - obs.collider[0].size.z - (PlayerCollider.z + 0.1);

								client->direction.z = 0;
								client->velocity.z = 0;
							}
						}
					}
				}

				if (obs.data.state == OBSTACLE_STATE::SPIN)
				{
					obs.collider[1].orientation =
						DirectX::XMQuaternionRotationRollPitchYaw
						(
							0,
							XMConvertToRadians(obs.rotate),
							0
						);

					XMFLOAT4 rhsOrientation;

					XMStoreFloat4(&rhsOrientation, obs.collider[1].orientation);

					if (client->collider.isCollisionOBB(obs.collider[1]))
					{
						client->isColl = true;
					
						Vector3 collisionNormal = client->position - *obs.collider[1].position;
						collisionNormal.y = 0;
						collisionNormal.Normalize();
					
						float angularVelocity = 3; 
					
						Vector3 tangentialVelocity = Vector3::Cross(obs.collider[1].size, collisionNormal) * angularVelocity;
					
						Vector3 pushDirection = tangentialVelocity;
						pushDirection.y = 0;
						pushDirection.Normalize();
					
						float relativeVelocity = Vector3::Dot(client->velocity, pushDirection);
					
						auto someFactor = 50.0f;
						
						if (relativeVelocity <= 0)
						{
							float pushMagnitude = angularVelocity * someFactor;
					
							client->velocity = Vector3::Zero();
							client->velocity += pushDirection * pushMagnitude;
						}
						else
						{
							float pushMagnitude = angularVelocity * someFactor;
					
							client->velocity = Vector3::Zero();
							client->velocity -= pushDirection * pushMagnitude;
						}
					}
				}

				if (obs.data.state == OBSTACLE_STATE::PENDULUM)
				{
					obs.collider[0].orientation =
						DirectX::XMQuaternionRotationRollPitchYaw
						(
							0,
							0,
							sinf(XMConvertToRadians(obs.rotate))
						);

					if (client->collider.isCollisionOBB(obs.collider[0]))
					{
						//client->isColl = true;
						//
						//// Assuming the obstacle's velocity is accessible through obs.velocity.
						//XMVECTOR obstacleVelocity = XMLoadFloat3(&obs.velocity);
						//
						//// Calculate the direction of the obstacle's movement at the point of collision.
						//XMVECTOR movementDirection = XMVector3Cross(collisionNormal, obstacleVelocity);
						//movementDirection = XMVector3Normalize(movementDirection);
						//
						//// Determine the relative velocity between the player and the obstacle along the movement direction.
						//XMVECTOR relativeVelocity = client->velocity - obstacleVelocity;
						//
						//// Calculate the magnitude of the pushing force based on relative velocity, mass, or any other factors.
						//// You can adjust this value to achieve the desired gameplay effect.
						//float forceMagnitude = 100.0f;
						//
						//// Apply the pushing force to the player in the direction of the obstacle's movement.
						//client->velocity += movementDirection * forceMagnitude;
					}
				}
			}

			for (auto& super : superJump)
			{
				if (client->isSuperJump) break;
				if (client->RoomID == -1 || client->ID == -1) break;
				if (client->isAI) continue;

				if (client->collider.isCollisionAABB(super.collider))
				{
					cout << "CALL SUPER : " << client->ID << endl;
					lock_guard<mutex> lock{ client->lock };
					client->isSuperJump = true;
					client->isMove = false;
					client->isJump = false;
					client->velocity = Vector3::Zero();
					client->position = Vector3(super.data.xPos, super.data.yPos, super.data.zPos);
				}
			}

			{
				lock_guard<mutex> lock{ client->lock };
				if (startCountByRoomID[client->RoomID] == 0)
				{
					if (!client->isSuperJump)
					{
						client->position += client->velocity * DeltaTimefloat.count();
					}
					else
					{
						client->position += SUPERJUMP * DeltaTimefloat.count() * (isFeverByRoomID[client->ID] ? FeverModeMulti : 1);
						if (client->position.y > 1000.0f)
						{
							client->isSuperJump = false;
							client->velocity = Vector3::Zero();
						}
					}
				}

				if (client->position.y < -3000)
				{
					client->velocity.y = 0;
					client->position = Vector3{ 0,10,50 };
				}
				else if (client->position.z > 21500 && client->position.y > 0)
				{
					client->isGoal = true;
					client->isMove = false;
					client->isJump = false;
					client->velocity = Vector3::Zero();
					client->direction = Vector3::Zero();
					client->score += 120000 - chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTimePointByRoomID[client->RoomID]).count();
				}
			}

			int count = 0;
			for (auto& coin : coins)
			{
				if (client->RoomID == -1 || client->ID == -1) break;
				if (client->isCoin) break;
				if (isCoinActiveByRoomID[client->RoomID][count]) continue;

				if (client->collider.isCollisionAABB(coin.collider))
				{
					cout << "CLIENT ID : " << client->ID << endl;
					lock_guard<mutex> lock{ client->lock };
					client->isCoin = true;
					isCoinActiveByRoomID[client->RoomID][count] = true;
					for (auto& cl : clients)
					{
						if (cl.second->ID == -1) continue;
						if (cl.second->isAI) continue;
						if (cl.second->RoomID != client->RoomID) continue;
						if (cl.second->mapType != client->mapType) continue;

						cl.second->SendEnterCoinPacket(id, count);
					}
				}
				count++;
			}
		}
		else if (client->mapType == MapType::Jump)
		{
			client->isColl = false;
			// 장애물 충돌체크
			for (auto& obs : jumpMapObstacle)
			{
				if (client->velocity.x != 0 || client->velocity.z != 0)
				{
					if (obs.data.state == OBSTACLE_STATE::SPIN || obs.data.state == OBSTACLE_STATE::STOP)
					{
						if (client->collider.isCollisionAABB(obs.collider[0]))
						{
							client->isColl = true;

							// BoundingBox Side 별 거리 계산 후 가장 가까운 방향의 Normal Vector 세팅 후 반환
							auto clientCenter = client->collider.getBoundingbox().Center;
							XMVECTOR center = XMLoadFloat3(&clientCenter);
							XMVECTOR faceNormal = client->collider.GetClosestFaceNormal(obs.collider[0].getBoundingbox(), center);

							if (XMVectorGetX(faceNormal) < 0.0f)
							{
								lock_guard<mutex> lock{ client->lock };
								if (client->position.x > obs.position.x)
									client->position.x = obs.position.x + obs.collider[0].size.x + (PlayerCollider.x);
								else
									client->position.x = obs.position.x - obs.collider[0].size.x - (PlayerCollider.x);
								client->direction.x = 0;
								client->velocity.x = 0;
							}
							else if (XMVectorGetZ(faceNormal) < 0.0f)
							{
								lock_guard<mutex> lock{ client->lock };
								if (client->position.z > obs.position.z)
									client->position.z = obs.position.z + obs.collider[0].size.z + (PlayerCollider.z + 0.1);
								else
									client->position.z = obs.position.z - obs.collider[0].size.z - (PlayerCollider.z + 0.1);

								client->direction.z = 0;
								client->velocity.z = 0;
							}
						}
					}
				}

				if (obs.data.state == OBSTACLE_STATE::SPIN)
				{
					obs.collider[1].orientation =
						DirectX::XMQuaternionRotationRollPitchYaw
						(
							0,
							XMConvertToRadians(obs.rotate),
							0
						);

					XMFLOAT4 rhsOrientation;

					XMStoreFloat4(&rhsOrientation, obs.collider[1].orientation);

					if (client->collider.isCollisionOBB(obs.collider[1]))
					{
						client->isColl = true;

						Vector3 collisionNormal = client->position - *obs.collider[1].position;
						collisionNormal.y = 0;
						collisionNormal.Normalize();

						float angularVelocity = 3;

						Vector3 tangentialVelocity = Vector3::Cross(obs.collider[1].size, collisionNormal) * angularVelocity;

						Vector3 pushDirection = tangentialVelocity;
						pushDirection.y = 0;
						pushDirection.Normalize();

						float relativeVelocity = Vector3::Dot(client->velocity, pushDirection);

						auto someFactor = 100.0f;

						if (relativeVelocity <= 0)
						{
							float pushMagnitude = angularVelocity * someFactor;

							client->velocity = Vector3::Zero();
							client->velocity += pushDirection * pushMagnitude * (isFeverByRoomID[client->ID] ? FeverModeMulti : 1);
						}
						else
						{
							float pushMagnitude = angularVelocity * someFactor;

							client->velocity = Vector3::Zero();
							client->velocity -= pushDirection * pushMagnitude * (isFeverByRoomID[client->ID] ? FeverModeMulti : 1);
						}
					}
				}
				if (client->velocity.y < 0)
				{
					for (auto& tile : jumpMapTiles)
					{
						if (client->collider.isCollisionAABB(tile.collider))
						{
							if (client->position.y + abs(client->velocity.y) < tile.collider.position->y) break;

							//if(tile.data.state == LayerState::L1Water)
							//	cout << "Tile : " << tile.data.state << endl;

							lock_guard<mutex> lock{ client->lock };
							client->position.y = tile.collider.position->y;
							client->velocity.y = 0;
							if (client->isJump)
							{
								client->isJump = false;
								client->isGoal = true;
							}
							break;
						}
					}
				}
				{
					lock_guard<mutex> lock{ client->lock };
					if (startCountByRoomID[client->RoomID] == 0)
						client->position += client->velocity * DeltaTimefloat.count();
					if (client->position.y < -3000)
					{
						client->velocity.y = 0;
					}
				}
			}
		}
		else if (client->mapType == MapType::Meteo)
		{
			if (client->velocity.y < 0)
			{
				for (auto& tile : meteoTiles)
				{
					if (isGroundByRoomID[client->RoomID][static_cast<int>(tile.Mdata.state) - 6])
					{
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
			}
			{
				lock_guard<mutex> lock{ client->lock };
				if (startCountByRoomID[client->RoomID] == 0)
					client->position += client->velocity * DeltaTimefloat.count();
				if (client->position.y < -3000)
				{
					client->velocity.y = 0;
				}
			}
		}

		if (!client->isAI)
		{
			client->SendPlayerInfoPacket(id, client->position, client->degree, client->isMove, client->isColl, client->isGoal);
			//cout << "Player Pos X : " << client->position.x << " Z : " << client->position.z << endl;
		}

		for (auto cl : clients)
		{
			if (cl.second == nullptr)
				continue;
			ClientException(cl, id);
			if (cl.second->isAI) continue;
			if (cl.second->RoomID != client->RoomID) continue;
			if (cl.second->mapType != client->mapType) continue;

			cl.second->SendPlayerInfoPacket(id, client->position, client->degree, client->isMove, client->isColl, client->isGoal);
		}

		Event event{ id, OverlappedType::Update, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);

		break;
	}
	case OverlappedType::AddAI:
	{
		break;
	}
	case OverlappedType::UpdateAI:
	{
		if (id == -1) break;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> x(-1.0f, 1.0f);
		std::uniform_real_distribution<float> z(0.0f, 1.0f);
		XMVECTOR dir{ x(gen), 0, z(gen) };

		XMFLOAT3 normalDir;
		XMStoreFloat3(&normalDir, XMVector3Normalize(dir));

		{
			lock_guard<mutex> lock{ clients[id]->lock };
			clients[id]->isMove = true;
			clients[id]->direction = { normalDir.x, normalDir.y, normalDir.z };

			auto x = clients[id]->direction.x;
			auto z = clients[id]->direction.z;

			clients[id]->degree = XMConvertToDegrees(atan2(x, z)) - 180;
			if (clients[id]->degree < -360)
				clients[id]->degree += 360;
		}

		std::uniform_int_distribution<int> update(1, 10);
		Event event{ id, OverlappedType::UpdateAI, chrono::system_clock::now() + chrono::seconds(update(gen)) };
		eventQueue.push(event);
		break;
	}
	case OverlappedType::PushCoolTime:
	{
		clients[id]->isCanPush = true;
		break;
	}
	case OverlappedType::PushEnd:
	{
		clients[id]->isPushed = false;
		clients[id]->direction = Vector3::Zero();
		if (clients[id]->isAI)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> x(-1.0f, 1.0f);
			std::uniform_real_distribution<float> z(0.0f, 1.0f);
			XMVECTOR dir{ x(gen), 0, z(gen) };

			XMFLOAT3 normalDir;
			XMStoreFloat3(&normalDir, XMVector3Normalize(dir));

			{
				lock_guard<mutex> lock{ clients[id]->lock };
				clients[id]->isMove = true;
				clients[id]->direction = { normalDir.x, normalDir.y, normalDir.z };

				auto x = clients[id]->direction.x;
				auto z = clients[id]->direction.z;

				clients[id]->degree = XMConvertToDegrees(atan2(x, z)) - 180;
				if (clients[id]->degree < -360)
					clients[id]->degree += 360;
			}
		}
		break;
	}
	case OverlappedType::Meteo:
	{
		auto targetTime = chrono::system_clock::now() + 12s;
		for (auto cl : clients)
		{
			if (cl.second->isAI) continue;
			if (cl.second->RoomID != id) continue;

			int target;
			while (true)
			{
				srand(time(NULL));
				auto r = rand() % 6;
				if(isGroundByRoomID[id][r])
				{
					target = r;
					isGroundByRoomID[id][r] = false;
				}
			}
			cl.second->SendMeteoPacket(target, targetTime);
		}

		if (startCountByRoomID[id] == 0)
		{
			Event event{ id, OverlappedType::Meteo, targetTime };
			eventQueue.push(event);
		}
		break;
	}
	case OverlappedType::JumpMap:
	{

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
	if (client->ID == -1) return;

	lock_guard<mutex> lock{ clients[ID]->lock };

	// 매칭 이후 추가
	//if (client->RoomID > 0)
	//{
	//
	//}

	bool isInPlayer = false;

	for (auto cl : clients)
	{
		ClientException(cl, ID);
		if (cl.second->RoomID != client->RoomID) continue;
		if (cl.second->mapType != client->mapType) continue;
		if (!cl.second->isAI)
		{
			cl.second->SendRemovePlayerPacket(ID);
			isInPlayer = true;
		}
	}

	if (false == isInPlayer)
	{
		for (auto cl : clients)
		{
			ClientException(cl, ID);
			if (cl.second->RoomID == client->RoomID)
			{
				lock_guard<mutex> lock{ cl.second->lock };
				client->ID = -1;
				closesocket(cl.second->socket);
			}
		}
	}

	client->ID = -1;
	closesocket(clients[ID]->socket);
}

void ServerBase::ProcessPacket(const int id, char* packet)
{
	switch ((unsigned char)packet[sizeof(PACKETSIZE)])
	{
	case ClientLogin:
	{
		clients[id]->SendServerLoginPacket(id);
		break;
	}
	case ClientMatching:
	{
		auto p = reinterpret_cast<ClientMatchingPacket*>(packet);
		{
			lock_guard<mutex> lock{ clients[id]->lock };
			clients[id]->mapType = static_cast<MapType>(p->gameMode);
			matchingManager->DoMatching(clients[id], eventQueue);
		}
		break;
	}
	case ::ClientReady:
	{
		ClientReady(id);
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
	}
}

void ServerBase::ProcessInput(const int id, ClientKeyInputPacket* packet)
{
	auto client = clients[id];
	if (client->mapType == MapType::Lobby || client->RoomID == -1) return;
	if (startCountByRoomID[client->RoomID] != 0) return;
	if (client->isGoal) return;
	if (client->isPushed) return;
	if (client->isSuperJump) return;

	auto key = packet->key;
	Vector3 dir = Vector3(packet->x, packet->y, packet->z);
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
			lock_guard<mutex> lock{ client->lock };
			client->isJump = true;
			client->velocity.y = JUMPVEL * (isFeverByRoomID[client->ID] ? FeverModeMulti : 1);
		}
		break;
	}
	case KeyType::Push: // 밀치기
	{
		if (false == client->isCanPush) break;

		float nearestDistance = PUSHDISTANCE + 1;
		Client* target = NULL;
		for (auto& cl : clients)
		{
			ClientException(cl, id);
			if (cl.second->mapType != client->mapType) continue;
			if (cl.second->RoomID != client->RoomID) continue;
			if (Vector3::Distance(client->position, cl.second->position) > PUSHDISTANCE) continue;

			auto distance = Vector3::Distance(client->position, cl.second->position);
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				target = cl.second;
			}
		}

		if (target != NULL)
		{
			lock_guard<mutex> lock{ client->lock };
			target->isPushed = true;
			client->isCanPush = false;

			auto dirvec = target->position - client->position;
			XMVECTOR vec{ dirvec.x, 0, dirvec.z };
			XMFLOAT3 normalDir;
			XMStoreFloat3(&normalDir, XMVector3Normalize(vec));

			target->direction = { normalDir.x, 0, normalDir.z };

			auto timepoint = chrono::system_clock::now();
			auto pushedTime = timepoint + PushTime;
			cout << "PUSH TIME" << (pushedTime - timepoint).count() << endl;
			for (auto& cl : clients)
			{
				if (cl.second->isAI) continue;
				if (cl.second->ID < 0) continue;
				if (cl.second->mapType != client->mapType) continue;
				if (cl.second->RoomID != client->RoomID) continue;

				cl.second->SendPushedPacket(target->ID, pushedTime);
			}

			auto useTime = chrono::system_clock::now() + 5s;

			client->SendPushCoolTimePacket(useTime);

			Event coolTime{ client->ID, OverlappedType::PushCoolTime, useTime };
			eventQueue.push(coolTime);
			Event effectTime{ target->ID, OverlappedType::PushEnd, pushedTime };
			eventQueue.push(effectTime);
		}

		break;
	}
	case KeyType::Boom: // 폭탄돌리기

		break;
	default:
		break;
	}
}

void ServerBase::ClientReady(const int id)
{
	cout << "Cleint READY\n";
	switch (clients[id]->mapType)
	{
	case MapType::Lobby:
	{

	}
	case MapType::Racing:
	{
		unsigned short RPS[OBSTACLENUM] = { 0, };
		unsigned short degree[OBSTACLENUM] = { 0, };
		int i = 0;
		for (auto& obs : obstacles)
		{
			if (obs.data.state != OBSTACLE_STATE::STOP)
			{
				RPS[i] = obs.angularVelocity * 100;
				degree[i] = obs.rotate * 100;
				++i;
			}
		}
		clients[id]->SendObstacleRPSPacket(RPS, OBSTACLENUM * sizeof(short));
		clients[id]->SendObstacleInfoPacket(degree, OBSTACLENUM * sizeof(short));

		Event event{ id, OverlappedType::Update, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);

		break;
	}
	case MapType::Result:
	{
		break;
	}
	default: // Obstacle
	{
		Event event{ id, OverlappedType::Update, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);
		break;
	}
	}

	int readyCount = -1;
	{
		lock_guard<mutex> lock{ this->lock };
		remainingUnReadyClientNumByRoomID[clients[id]->RoomID] -= 1;
		readyCount = remainingUnReadyClientNumByRoomID[clients[id]->RoomID];
	}

	if (readyCount == 0)
	{
		Event event{ clients[id]->RoomID, OverlappedType::GameStartCount, chrono::system_clock::now() + 1s };
		eventQueue.push(event);
	}

	if (!clients[id]->isAI)
	{
		clients[id]->SendAddPlayerPacket(id, PlayerStartPos);

		for (auto& client : clients)
		{
			ClientException(client, id);
			if (client.second->mapType != clients[id]->mapType) continue;
			if (client.second->RoomID != clients[id]->RoomID) continue;

			//if (!client.second->isAI)
			//	client.second->SendAddPlayerPacket(id, clients[id]->position);

			clients[id]->SendAddPlayerPacket(client.second->ID, client.second->position);
		}
	}
}

void ServerBase::GameStartCount(const int id)
{
	bool isFever = false;
	if (isFeverByRoomID.find(id) != isFeverByRoomID.end())
		isFever = isFeverByRoomID[id];

	MapType curMode = MapType::Lobby;

	for (auto cl : clients)
	{
		if (cl.second->ID == -1) continue;
		if (cl.second->isAI) continue;
		if (cl.second->RoomID == id)
		{
			curMode = cl.second->mapType;
			cl.second->SendGameStartPacket(startCountByRoomID[id]);
		}
	}

	if (startCountByRoomID[id] > 0 && curMode != MapType::Lobby)
	{
		Event event{ id, OverlappedType::GameStartCount, chrono::system_clock::now() + 1s };
		eventQueue.push(event);
		startCountByRoomID[id] -= 1;
		cout << "StartCount : " << startCountByRoomID[id] << ", RoomID : " << id << endl;
	}
	else return;

	if (curMode == MapType::Racing)
	{
		RacingStartCount(id, isFever);
	}
	else if (curMode == MapType::Meteo)
	{
		MeteoStartCount(id, isFever);
	}
	else if (curMode == MapType::Jump)
	{
		JumpStartCount(id, isFever);
	}
}

void ServerBase::GameEnd(const int id)
{
	startCountByRoomID[id] = 3;
	bool isFever = false;
	if (isFeverByRoomID.find(id) != isFeverByRoomID.end())
		isFever = isFeverByRoomID[id];

	int count = 0;
	int rank[]{ -1,-1,-1 };
	int score[]{ -1,-1,-1 };
	std::unordered_map<unsigned char, int> scoreByID;
	if (isFever)
	{
		for (auto cl : clients)
		{
			if (cl.second->score == 0) continue;
			if (cl.second->RoomID == id)
			{
				if (score[0] < cl.second->score)
				{
					if (score[0] == -1)
					{
						score[0] = cl.second->score;
						rank[0] = cl.second->ID;
						continue;
					}
					else
					{
						score[2] = score[1];
						score[1] = score[0];
						score[0] = cl.second->score;

						rank[2] = rank[1];
						rank[1] = rank[0];
						rank[0] = cl.second->ID;
					}
				}
				else if (score[1] < cl.second->score)
				{
					if (score[1] == -1)
					{
						score[1] = cl.second->score;
						rank[1] = cl.second->ID;
						continue;
					}
					else
					{
						score[2] = score[1];
						score[1] = cl.second->score;

						rank[2] = rank[1];
						rank[1] = cl.second->ID;
					}
				}
				else if (score[2] < cl.second->score)
				{
					score[2] = cl.second->score;
					rank[2] = cl.second->ID;
				}
			}
		}
	}

	for (auto cl : clients)
	{
		if (cl.second->ID == -1) continue;
		if (cl.second->RoomID == id)
		{
			if (!isFever)
			{
				cl.second->SendGameEndPacket(true);
			}
			else
			{
				cl.second->mapType = MapType::Lobby;
				cl.second->RoomID = -1;
				cl.second->score = 0;

				if (cl.second->isAI)
				{
					lock_guard<mutex> lock{ cl.second->lock };
					cl.second->ID = -1;
					closesocket(cl.second->socket);
				}
				else
				{
					cl.second->SendGameResultPacket(rank, 3 * sizeof(int));
				}
			}

			lock_guard<mutex> lock{ cl.second->lock };
			cl.second->ClearBoolean();
			cl.second->velocity = Vector3::Zero();
			cl.second->direction = Vector3::Zero();
		}
	}

	Event event{ id, OverlappedType::GameStartCount, chrono::system_clock::now() + 4s };
	eventQueue.push(event);

	isFeverByRoomID[id] = true;

}

void ServerBase::RacingStartCount(const int id, const bool isFever)
{
	int count = 0;
	for (auto cl : clients)
	{
		if (cl.second->RoomID == id)
		{
			lock_guard<mutex> lock{ cl.second->lock };
			cl.second->position = PlayerStartPos;
			cl.second->position.x += PlayerStartDistance * count;
			count++;
		}
	}

	if (startCountByRoomID[id] == 0 && !isFever)
	{
		auto startTime = chrono::system_clock::now();
		startTimePointByRoomID[id] = startTime;
		for (auto cl : clients)
		{
			if (cl.second->ID == -1) continue;
			if (cl.second->RoomID == id)
			{
				if (cl.second->isAI)
				{
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> urd(-1.0f, 1.0f);
					XMVECTOR dir{ urd(gen), 0, urd(gen) };

					XMFLOAT3 normalDir;
					XMStoreFloat3(&normalDir, XMVector3Normalize(dir));

					cl.second->direction = { normalDir.x, 0, normalDir.z };

					std::uniform_int_distribution<int> time(1, 10);
					Event event{ cl.second->ID, OverlappedType::UpdateAI, chrono::system_clock::now() + chrono::seconds(time(gen)) };
					eventQueue.push(event);
				}
				else
				{
					cl.second->SendStartTimePacket(startTime);
				}
			}
		}
		isCoinActiveByRoomID[id][0] = false;
		isCoinActiveByRoomID[id][1] = false;
		Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(GameTime) };
		eventQueue.push(event);
	}
	else if (startCountByRoomID[id] == 0)
	{
		auto startTime = chrono::system_clock::now();
		startTimePointByRoomID[id] = startTime;
		for (auto& cl : clients)
		{
			if (cl.second->ID == -1) continue;
			if (cl.second->RoomID == id)
			{
				if (!cl.second->isAI)
				{
					cl.second->SendStartTimePacket(startTime);
				}
			}
		}
		isCoinActiveByRoomID[id][0] = false;
		isCoinActiveByRoomID[id][1] = false;
		Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(GameTime) };
		eventQueue.push(event);
	}
}

void ServerBase::MeteoStartCount(const int id, const bool isFever)
{
	int count = 0;
	for (auto cl : clients)
	{
		if (cl.second->RoomID == id)
		{
			lock_guard<mutex> lock{ cl.second->lock };
			cl.second->position = MeteoStartPos;
			cl.second->position += MeteoStartDistance * count;
			count++;
		}
	}

	if (startCountByRoomID[id] == 0 && !isFever)
	{
		auto startTime = chrono::system_clock::now();
		startTimePointByRoomID[id] = startTime;
		for (auto cl : clients)
		{
			if (cl.second->ID == -1) continue;
			if (cl.second->RoomID == id)
			{
				if (cl.second->isAI)
				{
				}
				else
				{
					cl.second->SendStartTimePacket(startTime);
					Event event{ id, OverlappedType::Meteo, startTime };
					eventQueue.push(event);
				}
			}
		}
		Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(SurviveTime) };
		eventQueue.push(event);
	}
	else if (startCountByRoomID[id] == 0)
	{
		auto startTime = chrono::system_clock::now();
		startTimePointByRoomID[id] = startTime;
		for (auto& cl : clients)
		{
			if (cl.second->ID == -1) continue;
			if (cl.second->RoomID == id)
			{
				if (!cl.second->isAI)
				{
					cl.second->SendStartTimePacket(startTime);
					Event event{ id, OverlappedType::Meteo, startTime };
					eventQueue.push(event);
				}
			}
		}
		Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(SurviveTime) };
		eventQueue.push(event);
	}
}

void ServerBase::JumpStartCount(const int id, const bool isFever)
{
	int count = 0;
	for (auto cl : clients)
	{
		if (cl.second->RoomID == id)
		{
			lock_guard<mutex> lock{ cl.second->lock };
			cl.second->position = JumpStartPos;
			cl.second->position += JumpStartDistance * count;
			count++;
		}
	}

	if (startCountByRoomID[id] == 0 && !isFever)
	{
		auto startTime = chrono::system_clock::now();
		startTimePointByRoomID[id] = startTime;
		for (auto cl : clients)
		{
			if (cl.second->ID == -1) continue;
			if (cl.second->RoomID == id)
			{
				if (cl.second->isAI)
				{
				}
				else
				{
					cl.second->SendStartTimePacket(startTime);
				}
			}
		}
		Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(SurviveTime) };
		eventQueue.push(event);
	}
	else if (startCountByRoomID[id] == 0)
	{
		auto startTime = chrono::system_clock::now();
		startTimePointByRoomID[id] = startTime;
		for (auto& cl : clients)
		{
			if (cl.second->ID == -1) continue;
			if (cl.second->RoomID == id)
			{
				if (!cl.second->isAI)
				{
					cl.second->SendStartTimePacket(startTime);
				}
			}
		}
		Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(SurviveTime) };
		eventQueue.push(event);
	}
}

