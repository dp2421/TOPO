#pragma once

constexpr int PORTNUM = 20150;
constexpr int BUFFERSIZE = 512;
constexpr int NAMESIZE = 20;

constexpr char SERVERIP[] = "210.117.115.67";
//constexpr char SERVERIP[] = "127.0.0.1";

constexpr unsigned char ClientLogin = 100;
constexpr unsigned char ClientMatching = 101;
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

enum class ObsType : int
{
	Rotate,
	Pendulum,
	Stop
};

//-------------------------------------------------------------------------------------

constexpr unsigned char ServerLogin = 201;
constexpr char ServerMatchingOK = 202;
constexpr char ServerGameStart = 203;

constexpr unsigned char ServerAddPlayer = 204;
constexpr unsigned char ServerRemovePlayer = 205;
constexpr unsigned char ServerPlayerInfo = 206;
constexpr unsigned char ServerObstacleInfo = 207;
constexpr unsigned char ServerGameTimer = 208;
constexpr unsigned char ServerGameEnd = 209;
constexpr unsigned char ServerGameResult = 210;
constexpr unsigned char ServerObstacleRPS = 211;
constexpr unsigned char ServerSingleObstacleInfo = 212;

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
	// 플레이어가 바라보고 있는 방향 받아옴
	float			x, y, z;
	float degree;
};

struct ClientMovePacket
{
	unsigned char size;
	unsigned char type;
	float x, y, z;
	float degree;
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

struct ServerRemovePlayerPacket
{
	unsigned char size;
	unsigned char	type;
	int		id;
};

struct ServerPlayerInfoPacket
{
	unsigned char size;
	unsigned char	type;
	int		id;
	bool	isMove;
	float	xPos, yPos, zPos;
	float	degree;
	// 방향도 넣어야 할까? 벡터 느낌으로
};

struct ServerObstacleInfoPacket
{
	unsigned char size;
	unsigned char	type;
	unsigned short		degree[66];
	//타입에 따라서 내용이 달라질 것 같음
	//장애물에 번호 부여?
	//NPC같은 느낌으로 따로 클래스?
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
	// 경과시간, 랭킹
};

struct ServerObstacleRPSPacket
{
	unsigned char size;
	unsigned char	type;
	unsigned short	angularVelocity[66];
};

struct ServerSingleObstacleInfoPacket
{
	unsigned char	size;
	unsigned char	type;
	unsigned char	id;
	unsigned short	degree;
};


#pragma pack (pop)