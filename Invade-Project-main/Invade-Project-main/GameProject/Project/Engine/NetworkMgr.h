#pragma once
class NetworkMgr
{
	SINGLE(NetworkMgr)
public:
	SOCKET socket;
	OverlappedEx recv;

	int CurID;
	std::unordered_map<int, CGameObject*> networkObjects;
public:

	void Init();
	void Update();

	void DoSend(void* packet);
	void SendClientLoginPacket();
	void SendClientKeyInputPacket(const int key, Vec3 dir);

	void DoRecv();
	void ProcessPacket();
private:
};

void CALLBACK RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag);
void CALLBACK SendCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flag);
