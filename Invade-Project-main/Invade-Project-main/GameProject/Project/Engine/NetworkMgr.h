#pragma once
class NetworkMgr
{
	SINGLE(NetworkMgr)
public:
	HANDLE IOCPHandle;
	SOCKET socket;
	OverlappedEx recv;

	int CurID = -1;
	std::unordered_map<int, CGameObject*> networkObjects;
	std::unordered_map<int, std::mutex> mutexList;

	std::chrono::system_clock::time_point startTime;
	int StartCntDown;

	int prevRemainData = 0;
	vector <std::thread> workerThreads;
	CGameObject* tempPlayerObj;

	int rankPLID[3];

public:

	void Init();
	void NetworkWorkerThread();

	void DoSend(void* packet);
	void SendClientLoginPacket();
	void SendClientMatchingPacket(MapType type);
	void SendClientKeyInputPacket(KeyType key, Vec3 dir, float degree);
	void SendClientMovePacket(Vec3 dir, float degree);
	void SendClientReadyPacket();

	void DoRecv();
	void AssemblyPacket(int recvData, OverlappedEx* over);
	void ProcessPacket(char* packet);

	void setRankPLID(int rk1, int rk2, int rk3) {
		rankPLID[0] = rk1;
		rankPLID[1] = rk2;
		rankPLID[2] = rk3;
	}

private:
};