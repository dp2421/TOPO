#include "pch.h"
#include "Client.h"

Client::Client() :
	ID(-1),
	RoomID(-1),
	prevRemainData(0),
	mapType(MapType::Lobby)
{
	collider.position = &this->position;
	collider.offset = PlayerOffset;
	collider.size = PlayerCollider;
}

Client::~Client()
{
}

void Client::SendPacket(void* packet)	
{
	OverlappedEx* overlappedEx = new OverlappedEx{ reinterpret_cast<char*>(packet) };
	WSASend(socket, &overlappedEx->wsaBuf, 1, 0, 0, &overlappedEx->overlapped, 0);
}

void Client::RecvPacket()
{
	DWORD recv_flag = 0;
	memset(&recv.overlapped, 0, sizeof(recv.overlapped));
	recv.wsaBuf.len = BUFFERSIZE - prevRemainData;
	recv.wsaBuf.buf = recv.sendBuf + prevRemainData;
	WSARecv(socket, &recv.wsaBuf, 1, 0, &recv_flag,	&recv.overlapped, 0);
}

void Client::SendServerLoginPacket(const int id)
{
	ServerLoginPacket packet;
	packet.size = sizeof(ServerLoginPacket);
	packet.type = ServerLogin;
	packet.id = id;
	packet.x = position.x;
	packet.y = position.y;
	packet.z = position.z;

	SendPacket(&packet);
}

void Client::SendMatchingOKPacket(const MapType gamemode)
{
	cout << "Send Matching OK\n";
	ServerMatchingOKPacket packet;
	packet.size = sizeof(ServerMatchingOKPacket);
	packet.type = ServerMatchingOK;
	packet.gameMode = static_cast<unsigned char>(gamemode);

	SendPacket(&packet);
}

void Client::SendGameStartPacket(const int count)
{
	ServerGameStartPacket packet;
	packet.size = sizeof(ServerGameStartPacket);
	packet.type = ServerGameStart;
	packet.count = count;

	SendPacket(&packet);
}

void Client::SendGameEndPacket(const bool isFever)
{
	ServerGameEndPacket packet;
	packet.size = sizeof(ServerGameEndPacket);
	packet.type = ServerGameEnd;
	packet.isFever = isFever;

	SendPacket(&packet);
}

void Client::SendGameResultPacket(const unsigned char id[], const int size)
{
	ServerGameResultPacket packet;
	packet.size = sizeof(ServerGameResultPacket);
	packet.type = ServerGameResult;
	memcpy(packet.id, id, size);

	SendPacket(&packet);
}

void Client::SendAddPlayerPacket(const int id, const Vector3 vec)
{
	ServerAddPlayerPacket packet;
	packet.size = sizeof(ServerAddPlayerPacket);
	packet.type = ServerAddPlayer;
	packet.id = id;
	packet.x = vec.x;
	packet.y = vec.y;
	packet.z = vec.z;

	SendPacket(&packet);
}

void Client::SendRemovePlayerPacket(const int id)
{
	ServerRemovePlayerPacket packet;
	packet.size = sizeof(ServerRemovePlayerPacket);
	packet.type = ServerRemovePlayer;
	packet.id = id;

	SendPacket(&packet);
}

void Client::SendPlayerInfoPacket(
	const int id,
	const Vector3 pos,
	const float degree,
	const bool isMove,
	const bool isColl,
	const bool isGoal
)
{
	ServerPlayerInfoPacket packet;
	packet.size = sizeof(ServerPlayerInfoPacket);
	packet.type = ServerPlayerInfo;
	packet.id = id;
	packet.isMove = isMove;
	packet.isColl = isColl;
	packet.isGoal = isGoal;
	packet.xPos = pos.x;
	packet.yPos = pos.y;
	packet.zPos = pos.z;
	packet.degree = degree;

	SendPacket(&packet);
}

void Client::SendObstacleInfoPacket(const unsigned short degree[], int size)
{
	ServerObstacleInfoPacket packet;
	packet.size = sizeof(ServerObstacleInfoPacket);
	packet.type = ServerObstacleInfo;
	memcpy(packet.degree, degree, size);

	SendPacket(&packet);
}

void Client::SendSingleObstacleInfoPacket(const unsigned char id, const unsigned short degree)
{
	ServerSingleObstacleInfoPacket packet;
	packet.size = sizeof(ServerSingleObstacleInfoPacket);
	packet.id = id;
	packet.type = ServerSingleObstacleInfo;

	SendPacket(&packet);
}

void Client::SendObstacleRPSPacket(const unsigned short angularVelocity[], int size)
{
	ServerObstacleRPSPacket packet;
	packet.size = sizeof(ServerObstacleRPSPacket);
	packet.type = ServerObstacleRPS;
	memcpy(packet.angularVelocity, angularVelocity, size);

	SendPacket(&packet);
}

void Client::SendMeteoPacket(const unsigned char target, unsigned short time)
{
	ServerMeteoInfoPacket packet;
	packet.size = sizeof(ServerMeteoInfoPacket);
	packet.type = ServerMeteoInfo;
	packet.time = time;

	SendPacket(&packet);
}

void Client::SendEnterCoinPacket(const int id)
{
	ServerEnterCoinPacket packet;
	packet.size = sizeof(ServerEnterCoin);
	packet.id = id;

	SendPacket(&packet);
}