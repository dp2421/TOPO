#pragma once
//vector를 통한 recv 버퍼 메모리 관리

class Client
{
public:
	int ID;
	int RoomID = -1;

	SOCKET socket;
	int prevRemainData;

	Vector3 position;
	Vector3 direction;
	Vector3 velocity;
public:
	Client();
	~Client();

	void SendPacket(void* packet);
	void RecvPacket();

	void SendServerLoginPacket(const int id);
	void SendAddPlayerPacket(const int id, const Vector3 pos);
	void SendPlayerInfoPacket(const int id, const Vector3 pos, const Vector3 dir);
private:
	OverlappedEx recv;
};

