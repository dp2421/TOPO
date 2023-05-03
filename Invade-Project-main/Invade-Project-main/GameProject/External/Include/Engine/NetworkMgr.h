#pragma once
class NetworkMgr
{
	SINGLE(NetworkMgr)
public:
	SOCKET socket;
	OverlappedEx recv;

	int CurID;
	std::unordered_map<int, CGameObject*> networkObjects;

	int prevRemainData = 0;
public:

	void Init();
	void Update();

	void DoSend(void* packet);
	void SendClientLoginPacket();
	void SendClientKeyInputPacket(KeyType key, Vec3 dir);
	void SendClientMovePacket(Vec3 dir);

	void DoRecv();
	void AssemblyPacket(int recvData);
	void ProcessPacket(char* packet);
private:
};

void CALLBACK RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag);
void CALLBACK SendCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag);
