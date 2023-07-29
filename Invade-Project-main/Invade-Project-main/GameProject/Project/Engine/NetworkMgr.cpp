#include "pch.h"
#include "NetworkMgr.h"
#include "RenderMgr.h"
#include "SceneMgr.h"
#include "GameObject.h"
#include "Transform.h"
#include "PlayerScript.h"
#include "ObstacleScript.h"
#include "UIScript.h"
#include "ItemScript.h"
#include "Camera.h"


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
    //tempPlayerObj = CSceneMgr::GetInst()->AddNetworkGameObject(false, Vec3::Zero);
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
            AssemblyPacket(numBytes, overlappedEx);
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
            break;
        case ClientKeyInput:
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

void NetworkMgr::SendClientMatchingPacket(MapType type)
{
    std::cout << "Send Matching\n";
    ClientMatchingPacket packet;
    packet.size = sizeof(ClientMatchingPacket);
    packet.type = ClientMatching;
    packet.gameMode = static_cast<unsigned char>(type);

    DoSend(&packet);
}

void NetworkMgr::SendClientKeyInputPacket(KeyType key, Vec3 dir, float degree)
{
    ClientKeyInputPacket packet;
    packet.size = sizeof(ClientKeyInputPacket);
    packet.type = ClientKeyInput;
    packet.key = key;
    packet.x = dir.x;
    packet.y = dir.y;
    packet.z = dir.z;
    packet.degree = degree;

    DoSend(&packet);
}

void NetworkMgr::SendClientMovePacket(Vec3 dir, float degree)
{
    ClientMovePacket packet;
    packet.size = sizeof(ClientMovePacket);
    packet.type = ClientMove;
    packet.x = dir.x;
    packet.y = dir.y;
    packet.z = dir.z;
    packet.degree = degree;

    DoSend(&packet);
}

void NetworkMgr::SendClientReadyPacket()
{
    std::cout << "Send Ready \n";
    ClientReadyPacket packet;
    packet.size = sizeof(ClientReadyPacket);
    packet.type = ClientReady;

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

void NetworkMgr::AssemblyPacket(int recvData, OverlappedEx* over)
{
    int remain_data = recvData + prevRemainData;
    char* p = over->wsaBuf.buf;
    while (remain_data > 0)
    {
        int packet_size = reinterpret_cast<PACKETSIZE*>(p)[0];
        if (packet_size <= remain_data) 
        {
            ProcessPacket(p);
            p = p + packet_size;
            remain_data = remain_data - packet_size;
        }
        else break;
    }
    prevRemainData = remain_data;

    if (remain_data > 0)
    {
        memcpy(over->sendBuf, p, remain_data);
    }

    DoRecv();
}

void NetworkMgr::ProcessPacket(char* packet)
{
    switch ((unsigned char)packet[sizeof(PACKETSIZE)])
    {
    case ServerLogin:
    {
        std::cout << "RECV LOGIN \n";
        ServerLoginPacket* p = reinterpret_cast<ServerLoginPacket*>(packet);
        CurID = p->id;
        std::cout << p->id << " Player ID\n";
        break;
    }
    case ServerMatchingOK:
    {
        ServerMatchingOKPacket* p = reinterpret_cast<ServerMatchingOKPacket*>(packet);
        CRenderMgr::GetInst()->SetMatchComplete(true, p->gameMode);
        std::cout << "Matching Complete \n";
        // 어떤 모드가 매칭됐는지, 매칭자체가 됐는지 

        // Scene 전환 코드 추가 지점
        break;
    }
    case ServerGameStart:
    {
        ServerGameStartPacket* p = reinterpret_cast<ServerGameStartPacket*>(packet);
        //for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
        //{
        //    if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::NUMBER)
        //    {
        //        obj->GetScript<CUIScript>()->SetCountDown(p->count);
        //    }
        //}
        // p->count << 시작까지 남은 초
        
        //CRenderMgr::GetInst()->GetCamera(1)->SetStartCnt((int)p->count);
        std::cout << "Start Count " << (int)p->count << std::endl;

        break;
    }
    case ServerStartTime:
    {
        ServerStartTimePacket* p = reinterpret_cast<ServerStartTimePacket*>(packet);
        startTime = p->startTime;

        break;
    }
    case ServerGameEnd:
    {
        ServerGameEndPacket* p = reinterpret_cast<ServerGameEndPacket*>(packet);

        CRenderMgr::GetInst()->SetFever(p->isFever);
        // p->isFever << 피버모드냐 아니냐 아니라면 Result도 같이 갈듯
        
        break;
    }
    case ServerGameResult:
    {
        ServerGameResultPacket* p = reinterpret_cast<ServerGameResultPacket*>(packet);
        
        // p->id 등수 배열 0부터 1등 
                
        //1등 테스트. 일단 플레이어 1등에 앉혀놓는거 확인

        networkObjects[CurID]->GetScript<CPlayerScript>()->SetChangeAward(true);
        networkObjects[CurID]->GetScript<CPlayerScript>()->startAwardScene(p->id[0]);
       
        break;
    }
    case ServerAddPlayer:
    {
        std::cout << "RECV ADDPLAYER \n";
        ServerAddPlayerPacket* p = reinterpret_cast<ServerAddPlayerPacket*>(packet);
        if (networkObjects.find(p->id) != networkObjects.end())
        {
            break;
        }
        else
        {
            auto obj = CSceneMgr::GetInst()->AddNetworkGameObject(false, Vec3(p->x, p->y, p->z));
            if (CurID == p->id) obj->GetScript<CPlayerScript>()->SetPlayable(true);
            networkObjects[p->id] = obj;
        }
        break;
    }
    case ServerRemovePlayer:
    {
        std::cout << "RECV REMOVEPLAYER Cur count : " << networkObjects.size() << "\n";
        //ServerRemovePlayerPacket* p = reinterpret_cast<ServerRemovePlayerPacket*>(packet);
        //CSceneMgr::GetInst()->RemoveNetworkGameObject(networkObjects[p->id]);
        //networkObjects.erase(p->id);
        //std::cout << "After count : " << networkObjects.size() << "\n";
        break;
    }
    case ServerPlayerInfo:
    {
        ServerPlayerInfoPacket* p = reinterpret_cast<ServerPlayerInfoPacket*>(packet);
        if (networkObjects.find(p->id) != networkObjects.end())
        {
            networkObjects[p->id]->GetScript<CPlayerScript>()->SetPlayerPos(Vec3(p->xPos, p->yPos, p->zPos), p->degree, p->isMove, p->isGoal);

            if (CurID == p->id) networkObjects[p->id]->GetScript<CPlayerScript>()->LetParticle(Vec3(p->xPos, p->yPos, p->zPos), PARTICLE_TYPE::COLLPARICLE, p->isColl);
        }
        else
        {
            break;
        }
        break;
    }
    case ServerObstacleInfo:
    {
        ServerObstacleInfoPacket* p = reinterpret_cast<ServerObstacleInfoPacket*>(packet);
        for (int i = 0; i < OBSTACLENUM; ++i)
        {
            CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Obstacle")->GetParentObj()[i]->GetScript<CObstacleScript>()->Rotate(((float)p->degree[i])/100);
        }
        break;
    }
    case ServerObstacleRPS:
    {
        ServerObstacleRPSPacket* p = reinterpret_cast<ServerObstacleRPSPacket*>(packet);
        for (int i = 0; i < OBSTACLENUM; ++i)
        {
            CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Obstacle")->GetParentObj()[i]->GetScript<CObstacleScript>()->SetSpeed(((float)p->angularVelocity[i]) / 100);
        }
        break;
    }
    case ServerSingleObstacleInfo:
    {
        ServerSingleObstacleInfoPacket* p = reinterpret_cast<ServerSingleObstacleInfoPacket*>(packet);
        CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Obstacle")->GetParentObj()[p->id]->GetScript<CObstacleScript>()->Rotate(((float)p->degree) / 100);
        break;
    }
    case ServerMeteoInfo:
    {
        break;
    }
    case ServerEnterCoin:
    {
        ServerEnterCoinPacket* p = reinterpret_cast<ServerEnterCoinPacket*>(packet);
        
        // p->id 먹은 플레이어 ID;
        CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Coin")->GetParentObj()[p->coinIndex]->GetScript<CItemScript>()->removeCoin(p->coinIndex);
        break;
    }
    case ServerPushed:
    {
        ServerPushedPacket* p = reinterpret_cast<ServerPushedPacket*>(packet);
        
        // p->id 밀쳐진 플레이어 

        break;
    }
    case ServerPushCoolTime:
    {
        ServerPushCoolTimePacket* p = reinterpret_cast<ServerPushCoolTimePacket*>(packet);

        if (networkObjects.find(CurID) != networkObjects.end())
        {
            networkObjects[CurID]->GetScript<CPlayerScript>()->pushTime = p->effectTime;
        }

        break;
    }
    default:
    {
        //std::cout << "TYPE : " << (int)(unsigned char)packet[sizeof(PACKETSIZE)] << " \n";
    }
    } 
}