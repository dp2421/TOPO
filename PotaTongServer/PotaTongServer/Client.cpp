#include "pch.h"
#include "Client.h"

Client::Client()
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
	packet.type = ClientLogin;
	packet.id = id;

	SendPacket(&packet);
}
