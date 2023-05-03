#include "pch.h"
#include "Client.h"

Client::Client() :
	ID(-1),
	RoomID(-1),
	prevRemainData(0)
{
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

void Client::SendPlayerInfoPacket(const int id, const Vector3 pos, const Vector3 dir)
{
	ServerPlayerInfoPacket packet;
	packet.size = sizeof(ServerPlayerInfoPacket);
	packet.type = ServerPlayerInfo;
	packet.id = id;
	packet.xPos = pos.x;
	packet.yPos = pos.y;
	packet.zPos = pos.z;
	packet.xDir = dir.x;
	packet.yDir = dir.y;
	packet.zDir = dir.z;

	SendPacket(&packet);
}
