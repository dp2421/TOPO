#include "pch.h"
#include "NetworkMgr.h"

NetworkMgr::NetworkMgr()
{

}

NetworkMgr::~NetworkMgr()
{
    closesocket(socket);
    WSACleanup();
}

void NetworkMgr::Init()
{
	WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return;
    }

    socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
    SOCKADDR_IN addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTNUM);
    inet_pton(AF_INET, SERVERIP, &addr.sin_addr);
    WSAConnect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), 0, 0, 0, 0);
}

void NetworkMgr::Update()
{
    SleepEx(1, true);
}

void NetworkMgr::DoSend(void* packet)
{
    OverlappedEx* overlappedEx = new OverlappedEx{ reinterpret_cast<char*>(packet) };
    WSASend(socket, &overlappedEx->wsaBuf, 1, 0, 0, &overlappedEx->overlapped, 0);
}

void NetworkMgr::SendClientLoginPacket()
{
    ClientLoginPacket packet;
    packet.size = sizeof(ClientLoginPacket);
    packet.type = ClientLogin;
    string name{ "p" };
    strcpy_s(packet.name, name.c_str());

    DoSend(&packet);
}

void NetworkMgr::SendClientKeyInputPacket(const int key, Vec3 dir)
{
    ClientKeyInputPacket packet;
    packet.size = sizeof(ClientKeyInputPacket);
    packet.type = ClientKeyInput;
    packet.key = key;
    packet.x = dir.x;
    packet.y = dir.y;
    packet.z = dir.z;

    DoSend(&packet);
}

void NetworkMgr::DoRecv()
{
    DWORD recv_flag = 0;
    memset(&recv.overlapped, 0, sizeof(recv.overlapped));
    WSARecv(socket, &recv.wsaBuf, 1, NULL, &recv_flag, &recv.overlapped, RecvCallback);
}

void NetworkMgr::ProcessPacket()
{
    switch (recv.wsaBuf.buf[0])
    {
    case ServerLogin:
    {

        break;
    }
    case ServerAddPlayerInfo:
    {
        break;
    }
    case ServerPlayerInfo:
    {
    
    }
    default:
        break;
    } 
}

void RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag)
{
    NetworkMgr::GetInst()->ProcessPacket();
    NetworkMgr::GetInst()->DoRecv();
}
