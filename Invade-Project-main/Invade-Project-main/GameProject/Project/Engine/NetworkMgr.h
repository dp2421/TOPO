#pragma once
class NetworkMgr
{
	SINGLE(NetworkMgr)
public:
	SOCKET socket;
	OverlappedEx recv;
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
