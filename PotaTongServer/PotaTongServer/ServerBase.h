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
	void InitAI();

	void Run();
	void Accept();
	void Recv(const int id, DWORD recvByte, OverlappedEx* overlappedEx);
	void ServerEvent(const int id, OverlappedEx* overlappedEx);

	void Disconnect(int ID);

	void ProcessPacket(const int id, char* packet);

	void ProcessInput(const int id, ClientKeyInputPacket* key);

private:
	SOCKET ServerSocket, ClientSocket;
	HANDLE IOCPHandle;
	OverlappedEx GlobalOverlapped;

	concurrency::concurrent_unordered_map<int, Client*> clients;
	MatchingManager* matchingManager;

	concurrency::concurrent_unordered_map<unsigned short,
		std::function<void(unsigned char*)>> handlerMap;
	concurrency::concurrent_unordered_map<unsigned short, GameMode*> gameRoomMap;

	concurrency::concurrent_priority_queue<Event> eventQueue;

	std::vector<Obstacle> obstacles;
	std::vector<Tile> tiles;
	std::vector<thread> workerThreads;

	atomic<int> clientID = 0;
};
