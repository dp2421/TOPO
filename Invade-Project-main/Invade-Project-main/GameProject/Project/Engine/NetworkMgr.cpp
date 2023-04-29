#include "pch.h"
#include "NetworkMgr.h"
#include "SceneMgr.h"
#include "GameObject.h"
#include "Transform.h"


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
#if LOCALPLAY 
    return;
#else
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
    if (WSAConnect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), 0, 0, 0, 0))
    {
        std::cout << WSAGetLastError() << std::endl;
    }
    SendClientLoginPacket();
    DoRecv();
#endif // NETWORK
}

void NetworkMgr::Update()
{
#if LOCALPLAY
    return;
#else
    SleepEx(1, true);
#endif // NETWORK
}

void NetworkMgr::DoSend(void* packet)
{
#if LOCALPLAY
    return;
#else
    OverlappedEx* overlappedEx = new OverlappedEx{ reinterpret_cast<char*>(packet) };
    if (WSASend(socket, &overlappedEx->wsaBuf, 1, 0, 0, &overlappedEx->overlapped, SendCallback))
    {
        std::cout << WSAGetLastError() << std::endl;
    }
    else
    {
        switch (reinterpret_cast<char*>(packet)[1])
        {
        case ClientLogin:
            std::cout << "Send Login" << std::endl;
            break;
        case ClientKeyInput:
            std::cout << "Send KeyInput" << std::endl;
            break;
        }
    }
#endif
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

void NetworkMgr::SendClientMovePacket(Vec3 dir)
{
    ClientMovePacket packet;
    packet.size = sizeof(ClientMovePacket);
    packet.type = ClientMove;
    packet.x = dir.x;
    packet.y = dir.y;
    packet.z = dir.z;

    DoSend(&packet);
}


void NetworkMgr::DoRecv()
{
#if LOCALPLAY
    return;
#else
    DWORD recv_flag = 0;
    memset(&recv.overlapped, 0, sizeof(recv.overlapped));
    WSARecv(socket, &recv.wsaBuf, 1, NULL, &recv_flag, &recv.overlapped, RecvCallback);
#endif
}

void NetworkMgr::ProcessPacket()
{
    switch ((unsigned char)recv.wsaBuf.buf[1])
    {
    case ServerLogin:
    {
        ServerLoginPacket* packet = reinterpret_cast<ServerLoginPacket*>(recv.wsaBuf.buf);
        auto obj = CSceneMgr::GetInst()->AddNetworkGameObject(true, Vec3(packet->x, packet->y, packet->z));
        networkObjects[packet->id] = obj;
        break;
    }
    case ServerAddPlayer:
    {
        ServerAddPlayerPacket* packet = reinterpret_cast<ServerAddPlayerPacket*>(recv.wsaBuf.buf);
        auto obj = CSceneMgr::GetInst()->AddNetworkGameObject(false, Vec3(packet->x, packet->y, packet->z));
        networkObjects[packet->id] = obj;
        break;
    }
    case ServerPlayerInfo:
    {
        ServerPlayerInfoPacket* packet = reinterpret_cast<ServerPlayerInfoPacket*>(recv.wsaBuf.buf);
        networkObjects[packet->id]->Transform()->SetLocalPos(Vec3(packet->xPos, packet->yPos, packet->zPos));
        break;
    }
    } 
}

void RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag)
{
    NetworkMgr::GetInst()->ProcessPacket();
    NetworkMgr::GetInst()->DoRecv();
}

void SendCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag)
{
    delete over;
}
