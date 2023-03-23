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

	void Accept();
	void Reve(const int id, DWORD recvByte, OverlappedEx* overlappedEx);
	void Disconnect(int ID);

	void ProcessPacket(const int id, char* packet);
	void ProcessInput(const int id, int key);

private:
	SOCKET ServerSocket, ClientSocket;
	HANDLE IOCPHandle;
	OverlappedEx GlobalOverlapped;

	concurrency::concurrent_unordered_map<int, Client> clients;
	MatchingManager* matchingManager;

	concurrency::concurrent_unordered_map<unsigned short,
		std::function<void(unsigned char*)>> handlerMap;
	concurrency::concurrent_unordered_map<unsigned short, GameMode*> gameRoomMap;

	std::vector<thread> workerThreads;

	atomic<int> clientID = 0;	
};

