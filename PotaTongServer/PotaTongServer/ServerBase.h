#pragma once

class ServerBase
{
public:
	ServerBase();
	~ServerBase();

	void WorkerThread(HANDLE IOCP);
	void EventThread();

	void InitServer();
	void InitHandler();
	void InitObsatacleInfo();
	void InitMapInfo();
	void InitItemInfo();
	void InitMeteoInfo();
	void InitAI(int roomID, MapType mapType, int AINum);

	void Run();
	void Accept();
	void Recv(const int id, DWORD recvByte, OverlappedEx* overlappedEx);
	void ServerEvent(const int id, OverlappedEx* overlappedEx);

	void Disconnect(int ID);

	void ProcessPacket(const int id, char* packet);

	void ProcessInput(const int id, ClientKeyInputPacket* key);

	void ClientReady(const int id);

	void GameStartCount(const int id);
	void GameEnd(const int id);

private:
	SOCKET ServerSocket, ClientSocket;
	HANDLE IOCPHandle;
	OverlappedEx GlobalOverlapped;

	concurrency::concurrent_unordered_map<int, Client*> clients;
	MatchingManager* matchingManager;

	concurrency::concurrent_unordered_map<unsigned short,
		std::function<void(unsigned char*)>> handlerMap;
	concurrency::concurrent_unordered_map<unsigned short, GameMode*> gameRoomMap;
	concurrency::concurrent_unordered_map<unsigned short, chrono::system_clock::time_point> roomTimeMap;

	concurrency::concurrent_priority_queue<Event> eventQueue;

	concurrency::concurrent_unordered_map<int, int> remainingUnReadyClientNumByRoomID;
	concurrency::concurrent_unordered_map<int, int> startCountByRoomID;
	concurrency::concurrent_unordered_map<int, chrono::system_clock::time_point> startTimePointByRoomID;
	concurrency::concurrent_unordered_map<int, bool> isFeverByRoomID;
	concurrency::concurrent_unordered_map<int, bool*> isCoinActiveByRoomID;
	concurrency::concurrent_unordered_map<int, float> angularVelocityByRoomID;

	std::vector<Obstacle> obstacles;
	std::vector<Tile> coins;
	std::vector<Tile> superJump;
	std::vector<Tile> tiles;
	std::vector<Tile> racing2FTiles;
	std::vector<Tile> racing1FTiles;

	std::vector<thread> workerThreads;

	std::vector<Tile> meteoTiles;
	std::vector<Tile> jumpingTiles;

	atomic<int> clientID = 0;

	mutex lock;
};
