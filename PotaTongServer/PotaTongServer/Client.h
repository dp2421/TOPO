#pragma once
//vector를 통한 recv 버퍼 메모리 관리

class Client
{
public:
	int ID;
	int RoomID;

	mutex lock;

	SOCKET socket;
	int prevRemainData;

	bool isMove = false;
	bool isJump = false;

	bool isAI = false;
	float remainChangeAIVelocity = 0;

	float degree;
	Vector3 position;
	Vector3 direction;
	Vector3 velocity;

	Collider collider;
public:
	Client();
	~Client();

	void SendPacket(void* packet);
	void RecvPacket();

	void SendServerLoginPacket(const int id);
	void SendAddPlayerPacket(const int id, const Vector3 pos);
	void SendRemovePlayerPacket(const int id);
	void SendPlayerInfoPacket(const int id, const Vector3 pos, const float degree, const bool isMove);
	void SendObstacleInfoPacket(const unsigned short degree[], int size);
	void SendSingleObstacleInfoPacket(const unsigned char id, const unsigned short degree);
	void SendObstacleRPSPacket(const unsigned short angularVelocity[], int size);
private:
	OverlappedEx recv;
};

