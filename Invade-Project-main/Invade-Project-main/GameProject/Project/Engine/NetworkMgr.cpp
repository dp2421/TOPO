#include "pch.h"
#include "NetworkMgr.h"
#include "SceneMgr.h"
#include "GameObject.h"
#include "Transform.h"
#include "PlayerScript.h"


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

void NetworkMgr::SendClientKeyInputPacket(KeyType key, Vec3 dir)
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
    recv.wsaBuf.len = BUFFERSIZE - prevRemainData;
    recv.wsaBuf.buf = recv.sendBuf + prevRemainData;
    WSARecv(socket, &recv.wsaBuf, 1, NULL, &recv_flag, &recv.overlapped, RecvCallback);
#endif
}

void NetworkMgr::AssemblyPacket(int recvData)
{
    int remain_data = recvData + prevRemainData;
    char* p = recv.wsaBuf.buf;
    while (remain_data > 0) 
    {
        int packet_size = (unsigned char)p[0];
        if (packet_size <= remain_data) 
        {
            ProcessPacket(p);
            p = p + packet_size;
            remain_data = remain_data - packet_size;
        }
        else break;
    }
    prevRemainData = remain_data;
    if (remain_data > 0) {
        memcpy(recv.wsaBuf.buf, p, remain_data);
    }
}

void NetworkMgr::ProcessPacket(char* packet)
{
    switch ((unsigned char)packet[1])
    {
    case ServerLogin:
    {
        ServerLoginPacket* p = reinterpret_cast<ServerLoginPacket*>(packet);
        auto obj = CSceneMgr::GetInst()->AddNetworkGameObject(true, Vec3(p->x, p->y, p->z));
        networkObjects[p->id] = obj;
        break;
    }
    case ServerAddPlayer:
    {
        ServerAddPlayerPacket* p = reinterpret_cast<ServerAddPlayerPacket*>(packet);
        auto obj = CSceneMgr::GetInst()->AddNetworkGameObject(false, Vec3(p->x, p->y, p->z));
        networkObjects[p->id] = obj;
        break;
    }
    case ServerPlayerInfo:
    {
        ServerPlayerInfoPacket* p = reinterpret_cast<ServerPlayerInfoPacket*>(packet);
        if (networkObjects.find(p->id) != networkObjects.end())
            networkObjects[p->id]->GetScript<CPlayerScript>()->SetPlayerPos(Vec3(p->xPos, p->yPos, p->zPos));
        break;
    }
    } 
}

void RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag)
{
    //NetworkMgr::GetInst()->AssemblyPacket(numBytes);
    NetworkMgr::GetInst()->ProcessPacket(NetworkMgr::GetInst()->recv.wsaBuf.buf);
    NetworkMgr::GetInst()->DoRecv();
}

void SendCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag)
{
    delete over;
}
