#pragma once

constexpr int PORTNUM = 4000;
constexpr int BUFFERSIZE = 256;
constexpr int NAMESIZE = 20;

constexpr char ClientLogin = 0;
constexpr char ClientMatching = 0;
// ĳ���ٲٱ�? ���ٲٱ� ���
constexpr char ClientReady = 0;
constexpr char ClientKeyInput = 0;

//-------------------------------------------------------------------------------------

constexpr char ServerLogin = 0;
constexpr char ServerMatchingOK = 0;
constexpr char ServerGameStart = 0;

constexpr char ServerPlayerInfo = 0;
constexpr char ServerObstacleInfo = 0;
constexpr char ServerGameTimer = 0;
constexpr char ServerGameEnd = 0;

constexpr char ServerGameResult = 0;

#pragma pack (push, 1)

struct ClinetLoginPacket
{
	unsigned char size;
	char	type;
	char	name[NAMESIZE];
};

struct ClinetMatchingPacket
{
	unsigned char size;
	char	type;
	char	gameMode;
};

struct ClinetReadyPacket
{
	unsigned char size;
	char	type;
};

struct ClinetKeyInputPacket
{
	unsigned char size;
	char	type;
	char	key;
};

//-------------------------------------------------------------------------------------------------

struct ServerLoginPacket
{
	unsigned char size;
	char	type;
	int		id;
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

struct ServerPlayerInfoPacket
{
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
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