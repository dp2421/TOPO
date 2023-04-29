#pragma once

//constexpr auto DT = chrono::round<chrono::milliseconds>(chrono::duration<float>(1.f / 60.f));
constexpr float SPEED = 200.f;

constexpr int PORTNUM = 20150;
constexpr int BUFFERSIZE = 256;
constexpr int NAMESIZE = 20;
//constexpr char SERVERIP[] = "210.117.115.67";
constexpr char SERVERIP[] = "127.0.0.1";

constexpr unsigned char ClientLogin = 100;
constexpr unsigned char ClientMatching = 101;
// ĳ���ٲٱ�? ���ٲٱ� ���
constexpr unsigned char ClientReady = 102;
constexpr unsigned char ClientKeyInput = 103;
constexpr unsigned char ClientMove = 104;

enum class KeyType : int
{
	MoveStart,
	MoveEnd,
	Jump,
	Push,
	Boom,
};

//-------------------------------------------------------------------------------------

constexpr unsigned char ServerLogin = 201;
constexpr char ServerMatchingOK = 202;
constexpr char ServerGameStart = 203;

constexpr unsigned char ServerAddPlayer = 204;
constexpr unsigned char ServerPlayerInfo = 205;
constexpr unsigned char ServerObstacleInfo = 206;
constexpr unsigned char ServerGameTimer = 207;
constexpr unsigned char ServerGameEnd = 208;

constexpr unsigned char ServerGameResult = 208;

#pragma pack (push, 1)

struct ClientLoginPacket
{
	unsigned char size;
	unsigned char	type;
	char	name[NAMESIZE];
};

struct ClientMatchingPacket
{
	unsigned char size;
	unsigned char	type;
	char	gameMode;
};

struct ClientReadyPacket
{
	unsigned char size;
	unsigned char	type;
};

struct ClientKeyInputPacket
{
	unsigned char	size;
	unsigned char	type;
	KeyType			key;
	// �÷��̾ �ٶ󺸰� �ִ� ���� �޾ƿ�
	float			x, y, z;
};

struct ClientMovePacket
{
	unsigned char size;
	unsigned char type;
	float x, y, z;
};

//-------------------------------------------------------------------------------------------------

struct ServerLoginPacket
{
	unsigned char size;
	unsigned char	type;
	int		id;
	float	x, y, z;
};

struct ServerMatchingOKPacket
{
	unsigned char size;
	unsigned char	type;
};

struct ServerGameStartPacket
{
	unsigned char size;
	unsigned char	type;
};

struct ServerAddPlayerPacket
{
	unsigned char size;
	unsigned char	type;
	int		id;
	float	x, y, z;
};

struct ServerPlayerInfoPacket
{
	unsigned char size;
	unsigned char	type;
	int		id;
	float	xPos, yPos, zPos;
	float	xDir, yDir, zDir;
	// ���⵵ �־�� �ұ�? ���� ��������
};

struct ServerObstacleInfoPacket
{
	unsigned char size;
	unsigned char	type;
	//Ÿ�Կ� ���� ������ �޶��� �� ����
	//��ֹ��� ��ȣ �ο�?
	//NPC���� �������� ���� Ŭ����?
};

struct ServerGameTimerPacket
{
	unsigned char size;
	unsigned char	type;
	int		time;
};

struct ServerGameEndPacket
{
	unsigned char size;
	unsigned char	type;
	char	gameMode;
};

struct ServerGameResultPacket
{
	unsigned char size;
	unsigned char	type;
	int		id;
	// ��ŷ��? ���ھ? �Ѵ�?
};

#pragma pack (pop)