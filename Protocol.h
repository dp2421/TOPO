#pragma once
#include <chrono>

#define PACKETSIZE unsigned short

constexpr int PORTNUM = 20150;
constexpr int BUFFERSIZE = 1024;
constexpr int NAMESIZE = 20;
constexpr int OBSTACLENUM = 50;

//constexpr char SERVERIP[] = "210.117.115.67";
constexpr char SERVERIP[] = "127.0.0.1";

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

enum class MapType : int
{
	Lobby,
	Racing,
	Jump,
	Meteo,
	Bomb,
	Result
};

//-------------------------------------------------------------------------------------

constexpr unsigned char ServerLogin = 201;
constexpr unsigned char ServerMatchingOK = 202;
constexpr unsigned char ServerGameStart = 203;

constexpr unsigned char ServerAddPlayer = 204;
constexpr unsigned char ServerRemovePlayer = 205;
constexpr unsigned char ServerPlayerInfo = 206;
constexpr unsigned char ServerObstacleInfo = 207;

constexpr unsigned char ServerGameTimer = 208;
constexpr unsigned char ServerGameEnd = 209;
constexpr unsigned char ServerGameResult = 210;

constexpr unsigned char ServerObstacleRPS = 211;
constexpr unsigned char ServerSingleObstacleInfo = 212;
constexpr unsigned char ServerMeteoInfo = 213;
constexpr unsigned char ServerEnterCoin = 214;
constexpr unsigned char ServerStartTime = 215;
constexpr unsigned char ServerPushed = 216;
constexpr unsigned char ServerPushCoolTime = 217;

#pragma pack (push, 1)

struct ClientLoginPacket
{
	PACKETSIZE size;
	unsigned char	type;
	char	name[NAMESIZE];
};

struct ClientMatchingPacket
{
	PACKETSIZE size;
	unsigned char	type;
	char	gameMode;
};

struct ClientReadyPacket
{
	PACKETSIZE size;
	unsigned char	type;
};

struct ClientKeyInputPacket
{
	PACKETSIZE	size;
	unsigned char	type;
	KeyType			key;
	// 플레이어가 바라보고 있는 방향 받아옴
	float			x, y, z;
	float degree;
};

struct ClientMovePacket
{
	PACKETSIZE size;
	unsigned char type;
	float x, y, z;
	float degree;
};

//-------------------------------------------------------------------------------------------------

struct ServerLoginPacket
{
	PACKETSIZE size;
	unsigned char	type;
	int		id;
};

struct ServerMatchingOKPacket
{
	PACKETSIZE size;
	unsigned char	type;
	unsigned char	gameMode;
};

struct ServerGameStartPacket
{
	PACKETSIZE size;
	unsigned char	type;

	// 3, 2, 1, 0
	unsigned char	count;
};

struct ServerStartTimePacket
{
	PACKETSIZE size;
	unsigned char	type;

	std::chrono::system_clock::time_point startTime;
};

struct ServerAddPlayerPacket
{
	PACKETSIZE size;
	unsigned char	type;
	int		id;
	float	x, y, z;
};

struct ServerRemovePlayerPacket
{
	PACKETSIZE size;
	unsigned char	type;
	int		id;
};

struct ServerPlayerInfoPacket
{
	PACKETSIZE size;
	unsigned char	type;
	int		id;
	bool	isMove;
	bool	isColl;
	bool	isGoal;
	float	xPos, yPos, zPos;
	float	degree;
	// 방향도 넣어야 할까? 벡터 느낌으로
};

struct ServerObstacleInfoPacket
{
	PACKETSIZE size;
	unsigned char	type;
	unsigned short		degree[OBSTACLENUM];
	//타입에 따라서 내용이 달라질 것 같음
	//장애물에 번호 부여?
	//NPC같은 느낌으로 따로 클래스?
};

struct ServerGameTimerPacket
{
	PACKETSIZE size;
	unsigned char	type;
	int		time;
};

struct ServerGameEndPacket
{
	PACKETSIZE size;
	unsigned char	type;
	// 마지막은 Result
	bool	isFever;
};

struct ServerGameResultPacket
{
	PACKETSIZE size;
	unsigned char	type;
	int		id[3];
	// 경과시간, 랭킹
	// 랭킹만
};

struct ServerObstacleRPSPacket
{
	PACKETSIZE size;
	unsigned char	type;
	unsigned short	angularVelocity[OBSTACLENUM];
};

struct ServerSingleObstacleInfoPacket
{
	PACKETSIZE	size;
	unsigned char	type;
	int				id;
	unsigned short	degree;
};

/// <summary>
/// metorLayerState : enum typecast
/// targetTime : 무너지는 시간 
/// </summary>
struct ServerMeteoInfoPacket
{
	PACKETSIZE	size;
	unsigned char	type;
	unsigned char	metorLayerState;
	std::chrono::system_clock::time_point targetTime;
};

/// <summary>
/// ID : 코인 먹은 사람
/// coinIndex : 코인 번호
/// </summary>
struct ServerEnterCoinPacket
{
	PACKETSIZE	size;
	unsigned char	type;
	int				id;
	unsigned char	coinIndex;
};

/// <summary>
/// id : 밀린 캐릭터 ID
/// </summary>
struct ServerPushedPacket
{
	PACKETSIZE	size;
	unsigned char	type;
	int				id;
	std::chrono::system_clock::time_point effectTime;
};

struct ServerPushCoolTimePacket
{
	PACKETSIZE	size;
	unsigned char	type;
	std::chrono::system_clock::time_point effectTime;
};

#pragma pack (pop)