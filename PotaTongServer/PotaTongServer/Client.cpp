#include "pch.h"
#include "Client.h"

Client::Client() :
	ID(-1),
	RoomID(-1),
	prevRemainData(0)
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
	if (isAI) return;
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

void Client::SendObstacleInfoPacket(const unsigned short degree[], int size)
{
	ServerObstacleInfoPacket packet;
	packet.size = sizeof(ServerObstacleInfoPacket);
	packet.type = ServerObstacleInfo;
	memcpy(packet.degree, degree, size);

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

void Client::SendPlayerInfoPacket(const int id, const Vector3 pos, const float degree, const bool isMove)
{
	ServerPlayerInfoPacket packet;
	packet.size = sizeof(ServerPlayerInfoPacket);
	packet.type = ServerPlayerInfo;
	packet.id = id;
	packet.isMove = isMove;
	packet.xPos = pos.x;
	packet.yPos = pos.y;
	packet.zPos = pos.z;
	packet.degree = degree;

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
