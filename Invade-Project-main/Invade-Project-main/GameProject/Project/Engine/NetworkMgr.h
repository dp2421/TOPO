#pragma once
class NetworkMgr
{
	SINGLE(NetworkMgr)
public:
	HANDLE IOCPHandle;
	SOCKET socket;
	OverlappedEx recv;

	int CurID;
	std::unordered_map<int, CGameObject*> networkObjects;
	std::unordered_map<int, std::mutex> mutexList;

	int prevRemainData = 0;
	vector <std::thread> workerThreads;

	CGameObject* tempPlayerObj;
public:

	void Init();
	void NetworkWorkerThread();

	void DoSend(void* packet);
	void SendClientLoginPacket();
	void SendClientKeyInputPacket(KeyType key, Vec3 dir, float degree);
	void SendClientMovePacket(Vec3 dir, float degree);

	void DoRecv();
	void AssemblyPacket(int recvData);
	void ProcessPacket(char* packet);
private:
};