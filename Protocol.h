#pragma once

constexpr float DT = 1 / 60;
constexpr float SPEED = 200.f;

constexpr int PORTNUM = 4000;
constexpr int BUFFERSIZE = 256;
constexpr int NAMESIZE = 20;
constexpr char SERVERIP[] = "127.0.0.1";

constexpr char ClientLogin = 100;
constexpr char ClientMatching = 101;
// 캐릭바꾸기? 색바꾸기 고려
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
	// 플레이어가 바라보고 있는 방향 받아옴
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
	// 방향도 넣어야 할까? 벡터 느낌으로
};

struct ServerObstacleInfoPacket
{
	unsigned char size;
	char	type;
	//타입에 따라서 내용이 달라질 것 같음
	//장애물에 번호 부여?
	//NPC같은 느낌으로 따로 클래스?
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
	// 랭킹만? 스코어만? 둘다?
};

#pragma pack (pop)