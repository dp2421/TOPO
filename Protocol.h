#pragma once

constexpr float DT = 1 / 60;
constexpr float SPEED = 200.f;

constexpr int PORTNUM = 4000;
constexpr int BUFFERSIZE = 256;
constexpr int NAMESIZE = 20;
constexpr char SERVERIP[] = "127.0.0.1";

constexpr char ClientLogin = 100;
constexpr char ClientMatching = 101;
// ĳ���ٲٱ�? ���ٲٱ� ���
constexpr char ClientReady = 102;
constexpr char ClientKeyInput = 103;

//-------------------------------------------------------------------------------------

constexpr char ServerLogin = 201;
constexpr char ServerMatchingOK = 202;
constexpr char ServerGameStart = 203;

constexpr char ServerAddPlayerInfo = 204;
constexpr char ServerPlayerInfo = 205;
constexpr char ServerObstacleInfo = 206;
constexpr char ServerGameTimer = 207;
constexpr char ServerGameEnd = 208;

constexpr char ServerGameResult = 208;

#pragma pack (push, 1)

struct ClientLoginPacket
{
	unsigned char size;
	char	type;
	char	name[NAMESIZE];
};

struct ClientMatchingPacket
{
	unsigned char size;
	char	type;
	char	gameMode;
};

struct ClientReadyPacket
{
	unsigned char size;
	char	type;
};

struct ClientKeyInputPacket
{
	unsigned char size;
	char	type;
	int		key;
	// �÷��̾ �ٶ󺸰� �ִ� ���� �޾ƿ�
	float	x, y, z;
};

//-------------------------------------------------------------------------------------------------

struct ServerLoginPacket
{
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
};

struct ServerMatchingOKPacket
{
	unsigned char size;
	char	type;
};

struct ServerGameStartPacket
{
	unsigned char size;
	char	type;
};

struct ServerAddPlayerPacket
{
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
};

struct ServerPlayerInfoPacket
{
	unsigned char size;
	char	type;
	int		id;
	float	xPos, yPos, zPos;
	float	xDir, yDir, zDir;
	// ���⵵ �־�� �ұ�? ���� ��������
};

struct ServerObstacleInfoPacket
{
	unsigned char size;
	char	type;
	//Ÿ�Կ� ���� ������ �޶��� �� ����
	//��ֹ��� ��ȣ �ο�?
	//NPC���� �������� ���� Ŭ����?
};

struct ServerGameTimerPacket
{
	unsigned char size;
	char	type;
	int		time;
};

struct ServerGameEndPacket
{
	unsigned char size;
	char	type;
	char	gameMode;
};

struct ServerGameResultPacket
{
	unsigned char size;
	char	type;
	int		id;
	// ��ŷ��? ���ھ? �Ѵ�?
};

#pragma pack (pop)