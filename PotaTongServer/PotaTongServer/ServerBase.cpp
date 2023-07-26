#include "pch.h"
#include "ServerBase.h"

ServerBase::ServerBase()
{
	InitServer();

	matchingManager = new MatchingManager(IOCPHandle);
	InitHandler();
	InitObsatacleInfo();
	InitMapInfo();
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
			case OverlappedType::GameStartCount:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::GameEnd:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::RotateObs:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::SendRotateInfo:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::Update:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				// 시간 보간
				//auto diff = chrono::duration<float>((currentTime - event.excuteTime) / DeltaTimeMilli);
				//memcpy(overlappedEx->sendBuf, &diff, sizeof(diff));

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::UpdateAI:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::MatchingRacingComplete:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			case OverlappedType::MatchingObstacleComplete:
			{
				OverlappedEx* overlappedEx = new OverlappedEx;
				overlappedEx->type = event.eventType;

				PostQueuedCompletionStatus(IOCPHandle, 1, event.objID, &overlappedEx->overlapped);
				event.objID = -1;
				break;
			}
			default:
			{
				cout << (int)event.eventType << " EVENT TYPE ERROR!" << endl;
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
		bool isFever = false;
		if (isFeverByRoomID.find(id) != isFeverByRoomID.end())
			isFever = isFeverByRoomID[id];

		if (isFever)
		{
			int count = 0;
			for (auto cl : clients)
			{
				if (cl.second->RoomID == id)
				{
					cl.second->position = PlayerStartPos;
					cl.second->position.x += PlayerStartDistance * count;
					count++;
				}
			}
		}

		cout << "Start Count RoomID : " << id << endl;
		for (auto cl : clients)
		{
			if (cl.second->isAI) continue;
			if (cl.second->RoomID == id)
			{
				cl.second->SendGameStartPacket(startCountByRoomID[id] - 1);
			}
		}

		startCountByRoomID[id] -= 1;
		if (startCountByRoomID[id] == 0 && !isFever)
		{
			for (auto cl : clients)
			{
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
				}
			}
			Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(GameTime) };
			eventQueue.push(event);
		}	
		else if (startCountByRoomID[id] == 0)
		{
			Event event{ id, OverlappedType::GameEnd, chrono::system_clock::now() + chrono::seconds(GameTime) };
			eventQueue.push(event);
		}
		else
		{
			Event event{ id, OverlappedType::GameStartCount, chrono::system_clock::now() + 1s };
			eventQueue.push(event);
		}

		cout << startCountByRoomID[id] << " STARTCOUNT \n";

		break;
	}
	case OverlappedType::GameEnd:
	{
		startCountByRoomID[id] = 3;
		cout << startCountByRoomID[id] << " GameEND reamin Count\n";
		bool isFever = false;
		if (isFeverByRoomID.find(id) != isFeverByRoomID.end())
			isFever = isFeverByRoomID[id];

		int count = 0;
		unsigned char rank[] { -1,-1,-1 };
		int score[]{ -1,-1,-1 };
		std::unordered_map<unsigned char, int> scoreByID;
		if (isFever)
		{
			for (auto cl : clients)
			{
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
			if (cl.second->RoomID == id)
			{
				if (!isFever)
					cl.second->SendGameEndPacket(true);
				else
				{
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
						cl.second->SendGameResultPacket(rank, 3);
					}
				}

				lock_guard<mutex> lock{ cl.second->lock };
				cl.second->isGoal = false;
				cl.second->isMove = false;
				cl.second->isJump = false;
				cl.second->velocity = Vector3::Zero();
				cl.second->direction = Vector3::Zero();
			}
		}

		Event event{ id, OverlappedType::GameStartCount, chrono::system_clock::now() + 4s };
		eventQueue.push(event);

		isFeverByRoomID[id] = true;

		break;
	}
	case OverlappedType::MatchingRacingStart:
	{

		break;
	}
	case OverlappedType::MatchingObstacleStart:
	{

		break;
	}
	case OverlappedType::MatchingRacingComplete:
	{
		int connectClient = matchingManager->CompleteMatching(id, MapType::Racing);
		remainingUnReadyClientNumByRoomID[id] = connectClient;
		InitAI(id, MapType::Racing, connectClient);
		break;
	}
	case OverlappedType::MatchingObstacleComplete:
	{

		matchingManager->CompleteMatching(id, MapType::Obstacle);
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

		Event event{ 9999, OverlappedType::RotateObs, chrono::system_clock::now() + DeltaTimeMilli };
		eventQueue.push(event);

		break;
	}
	case OverlappedType::SendRotateInfo:
	{
		unsigned short degree[66] = { 0, };
		int i = 0;
		for (auto& obs : obstacles)
		{
			if (obs.data.state != OBSTACLE_STATE::STOP)
			{
				degree[i] = obs.rotate * 100;
				++i;
			}
		}

		for (auto client : clients)
		{
			if (client.second->RoomID == -1) continue;
			if (client.second->mapType != MapType::Racing) continue;

			if (client.second->ID != -1 && !client.second->isAI)
				client.second->SendObstacleInfoPacket(degree, 66 * sizeof(short));
		}

		Event event{ 9999, OverlappedType::SendRotateInfo, chrono::system_clock::now() + 1s };
		eventQueue.push(event);

		break;
	}
	case OverlappedType::Update:
	{
		auto client = clients[id];

		if (client->ID < 0)
		{
			break;
		}

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
						if (client->position.y + abs(client->velocity.y) < tile.collider.position->y) break;

						//if(tile.data.state == LayerState::L1Water)
						//	cout << "Tile : " << tile.data.state << endl;

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

			// 장애물 충돌체크
			for (auto& obs : obstacles)
			{
				//break; /////////////////////////////////////////////////////////////////////////////////////////// 장애물 충돌 임시 중단
				if (client->velocity.x != 0 || client->velocity.z != 0)
				{
					if (obs.data.state == OBSTACLE_STATE::SPIN || obs.data.state == OBSTACLE_STATE::STOP)
					{
						if (client->collider.isCollisionAABB(obs.collider[0]))
						{
							client->isColl = true;
							break;
							// BoundingBox Side 별 거리 계산 후 가장 가까운 방향의 Normal Vector 세팅 후 반환
							//auto boxcenter = obs.collider[0].getBoundingbox().Center;
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

							else if (XMVectorGetY(faceNormal) < 0.0f)
							{
								lock_guard<mutex> lock{ client->lock };
								if (client->position.y > 0)
								{
									client->direction.y = 0;
									client->isJump = false;
								}
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
							break;
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

					if (client->collider.isCollisionOBB(obs.collider[1]))
					{
						client->isColl = true;
						break;
						//std::cout << " COLL SPIN " << endl;
						auto originMat = DirectX::XMMatrixTranslation(client->position.x, client->position.y, client->position.z);

						Vector3 vec = *obs.collider[1].position;

						auto transMat = DirectX::XMMatrixTranslation(-vec.x, -vec.y, -vec.z);
						auto inverseTransMat = DirectX::XMMatrixTranslation(vec.x, vec.y, vec.z);
						auto rotateMat = DirectX::XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(-obs.rotate));
						auto inverseRotateMat = DirectX::XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(obs.rotate));

						auto calculMat = transMat * rotateMat * originMat;
						auto calculPos = Vector3(calculMat.r[3].m128_f32[0], calculMat.r[3].m128_f32[1], calculMat.r[3].m128_f32[2]);
						//cout << calculPos.x << " " << calculPos.y << " " << calculPos.z << endl;

						if (calculPos.x > 0 && calculPos.z < 0)
						{
							calculMat.r[3].m128_f32[2] = -(obs.collider[1].size.z + PlayerCollider.z);
						}
						else if (calculPos.x < 0 && calculPos.z > 0)
						{
							calculMat.r[3].m128_f32[2] = (obs.collider[1].size.z + PlayerCollider.z);
						}

						auto resultMat = calculMat * inverseRotateMat * inverseTransMat;
						auto resultPos = Vector3(resultMat.r[3].m128_f32[0], resultMat.r[3].m128_f32[1], resultMat.r[3].m128_f32[2]);

						{
							lock_guard<mutex> lock{ client->lock };
							client->position = resultPos;
						}
					}
				}

				if (obs.data.state == OBSTACLE_STATE::PENDULUM)
				{
					//auto transMat = XMMatrixTranslation(0, -150, 0);
					//auto rotMat = XMMatrixRotationRollPitchYaw(0, 0, sinf(XMConvertToRadians(obs.rotate)));
					//auto invTransMat = XMMatrixTranslation(0, 150, 0);
					//
					//auto resultMat = rotMat * transMat;
					//
					//obs.collider[0].orientation = XMQuaternionRotationMatrix(resultMat);

					obs.collider[0].orientation =
						DirectX::XMQuaternionRotationRollPitchYaw
						(
							0,
							0,
							sinf(XMConvertToRadians(obs.rotate))
						);

					if (client->collider.isCollisionOBB(obs.collider[0]))
					{
						client->isColl = true;
						break;
						XMVECTOR vec;
						float angle;
						XMFLOAT4 vec4;
						XMQuaternionToAxisAngle(&vec, &angle, obs.collider[0].orientation);
						XMStoreFloat4(&vec4, vec);
						//cout << "Coll PENDULUM " << vec4.x << " " << vec4.y << " " << vec4.z << " " << vec4.w << endl;
					}
				}
				client->isColl = false;
			}

			{
				lock_guard<mutex> lock{ client->lock };
				if(startCountByRoomID[client->RoomID] == 0)
					client->position += client->velocity * DeltaTimefloat.count();
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
					client->score += 120000 - chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() -  startTimePointByRoomID[client->RoomID]).count();
				}
			}
		}
		else if (client->mapType == MapType::Obstacle)
		{

		}
		else if (client->mapType == MapType::Meteo)
		{

		}
		else if (client->mapType == MapType::Bomb)
		{

		}

		if (!client->isAI)
			client->SendPlayerInfoPacket(id, client->position, client->degree, client->isMove, client->isColl, client->isGoal);

		for (auto cl : clients)
		{
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
	case ClientReady:
	{
		cout << "Cleint READY\n";
		switch (clients[id]->mapType)
		{
		case MapType::Lobby:
		{

		}
		case MapType::Racing:
		{
			cout << "Send Info  \n";
			unsigned short RPS[66] = { 0, };
			unsigned short degree[66] = { 0, };
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
			clients[id]->SendObstacleRPSPacket(RPS, 66 * sizeof(short));
			clients[id]->SendObstacleInfoPacket(degree, 66 * sizeof(short));

			Event event{ id, OverlappedType::Update, chrono::system_clock::now() + DeltaTimeMilli };
			eventQueue.push(event);


			int readyCount = -1;
			{
				lock_guard<mutex> lock{ this->lock };
				remainingUnReadyClientNumByRoomID[clients[id]->RoomID] -= 1;
				readyCount = remainingUnReadyClientNumByRoomID[clients[id]->RoomID];
			}
			if (readyCount == 0)
			{
				startCountByRoomID[clients[id]->RoomID] = 3;
				Event event{ clients[id]->RoomID, OverlappedType::GameStartCount, chrono::system_clock::now() + 1s };
				eventQueue.push(event);
			}

			break;
		}
		case MapType::Result:
		{
			break;
		}
		// Obstacle
		default:
		{

			break;
		}
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
			client->isJump = true;
			client->velocity.y = JUMPVEL;
		}
		break;
	}
	case KeyType::Push: // 밀치기
		for (auto& cl : clients)
		{
			ClientException(cl, id);
			if (cl.second->mapType != client->mapType) continue;
			if (cl.second->RoomID == -1) continue;

			if (Vector3::Distance(client->position, cl.second->position) > PUSHDISTANCE) continue;
			{

			}
		}
		break;
	case KeyType::Boom: // 폭탄돌리기

		break;
	default:
		break;
	}
}

