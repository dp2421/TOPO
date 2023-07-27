#pragma once
//vector�� ���� recv ���� �޸� ����

class Client
{
public:
	int ID;
	int RoomID;

	mutex lock;

	SOCKET socket;
	int prevRemainData;

	int score = 0;

	bool isMove = false;
	bool isJump = false;
	bool isColl = false;
	bool isGoal = false;
	bool isCoin = false;
	bool isPushed = true;

	bool isAI = false;
	float remainChangeAIVelocity = 0;
	MapType mapType;

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
	void SendMatchingOKPacket(const MapType gamemode);
	void SendGameStartPacket(const int count);
	void SendGameEndPacket(const bool isFever);
	void SendGameResultPacket(const unsigned char id[], const int size);
	void SendAddPlayerPacket(const int id, const Vector3 pos);
	void SendRemovePlayerPacket(const int id);
	void SendPlayerInfoPacket(
		const int id,
		const Vector3 pos,
		const float degree,
		const bool isMove,
		const bool isColl,
		const bool isGoal
	);
	void SendObstacleInfoPacket(const unsigned short degree[], int size);
	void SendSingleObstacleInfoPacket(const unsigned char id, const unsigned short degree);
	void SendObstacleRPSPacket(const unsigned short angularVelocity[], int size);
	void SendMeteoPacket(const unsigned char target, unsigned short time);
	void SendEnterCoinPacket(const int id, const int coinIndex);
private:
	OverlappedEx recv;
};

