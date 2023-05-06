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
    tempPlayerObj = CSceneMgr::GetInst()->AddNetworkGameObject(false, Vec3::Zero);
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return;
    }

    IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

    socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN addr;
    ZeroMemory(&addr, sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTNUM);
    addr.sin_addr.s_addr = inet_addr(SERVERIP);

    //socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
    //SOCKADDR_IN addr;
    //memset(&addr, 0, sizeof(addr));
    //addr.sin_family = AF_INET;
    //addr.sin_port = htons(PORTNUM);
    //inet_pton(AF_INET, SERVERIP, &addr.sin_addr);

    //if (WSAConnect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), 0, 0, 0, 0))
    //{
    //    std::cout << WSAGetLastError() << std::endl;
    //}
    if (WSAConnect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), NULL, NULL, NULL, NULL))
    {
        std::cout << WSAGetLastError() << " CONNECT " << std::endl;
    }
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), IOCPHandle, NULL, NULL);

    SendClientLoginPacket();
    DoRecv();

    std::thread* networkThread = new std::thread{ &NetworkMgr::NetworkWorkerThread, this };
#endif // NETWORK
}

void NetworkMgr::NetworkWorkerThread()
{
    while (true)
    {
        DWORD numBytes;
        ULONG_PTR ID;
        WSAOVERLAPPED* over = nullptr;
        BOOL ret = GetQueuedCompletionStatus(IOCPHandle, &numBytes, &ID, &over, INFINITE);
        OverlappedEx* overlappedEx = reinterpret_cast<OverlappedEx*>(over);
        const int id = static_cast<int>(ID);

        if (FALSE == ret)
        {
            std::cout << "Error" << std::endl;
        }

        switch (overlappedEx->type)
        {
        case OverlappedType::Recv:
            ProcessPacket(overlappedEx->sendBuf);
            break;
        case OverlappedType::Send:
            delete overlappedEx;
            break;
        }
    }
}

void NetworkMgr::DoSend(void* packet)
{
#if LOCALPLAY
    return;
#else
    OverlappedEx* overlappedEx = new OverlappedEx{ reinterpret_cast<char*>(packet) };
    if (WSASend(socket, &overlappedEx->wsaBuf, 1, 0, 0, &overlappedEx->overlapped, 0))
    {
        std::cout << WSAGetLastError() << " SEND " << std::endl;
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
    WSARecv(socket, &recv.wsaBuf, 1, NULL, &recv_flag, &recv.overlapped, 0);
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

    DoRecv();
}

void NetworkMgr::ProcessPacket(char* packet)
{
    switch ((unsigned char)packet[1])
    {
    case ServerLogin:
    {
        std::cout << "RECV LOGIN \n";
        ServerLoginPacket* p = reinterpret_cast<ServerLoginPacket*>(packet);
        tempPlayerObj->GetScript<CPlayerScript>()->SetPlayable(true);
        tempPlayerObj->GetScript<CPlayerScript>()->SetPlayerPos(Vec3(p->x, p->y, p->z));
        networkObjects[p->id] = tempPlayerObj;
        break;
    }
    case ServerAddPlayer:
    {
        std::cout << "RECV ADDPLAYER \n";
        ServerAddPlayerPacket* p = reinterpret_cast<ServerAddPlayerPacket*>(packet);
        if (networkObjects.find(p->id) != networkObjects.end())
        {
            networkObjects[p->id]->GetScript<CPlayerScript>()->SetPlayerPos(Vec3(p->x, p->y, p->z));
        }
        else
        {
            auto obj = CSceneMgr::GetInst()->AddNetworkGameObject(false, Vec3(p->x, p->y, p->z));
            networkObjects[p->id] = obj;
        }
        break;
    }
    case ServerRemovePlayer:
    {
        std::cout << "RECV REMOVEPLAYER Cur count : " << networkObjects.size() << "\n";
        ServerRemovePlayerPacket* p = reinterpret_cast<ServerRemovePlayerPacket*>(packet);
        CSceneMgr::GetInst()->RemoveNetworkGameObject(networkObjects[p->id]);
        networkObjects.erase(p->id);
        std::cout << "After count : " << networkObjects.size() << "\n";
        break;
    }
    case ServerPlayerInfo:
    {
        ServerPlayerInfoPacket* p = reinterpret_cast<ServerPlayerInfoPacket*>(packet);
        if (networkObjects.find(p->id) != networkObjects.end())
        {
            networkObjects[p->id]->GetScript<CPlayerScript>()->SetPlayerPos(Vec3(p->xPos, p->yPos, p->zPos));
        }
        else
        {
            auto obj = CSceneMgr::GetInst()->AddNetworkGameObject(false, Vec3(p->xPos, p->yPos, p->zPos));
            networkObjects[p->id] = obj;
        }
        break;
    }
    } 
    DoRecv();
}

//void RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag)
//{
//    NetworkMgr::GetInst()->ProcessPacket(NetworkMgr::GetInst()->recv.wsaBuf.buf);
//}
//
//void SendCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag)
//{
//    delete over;
//}
