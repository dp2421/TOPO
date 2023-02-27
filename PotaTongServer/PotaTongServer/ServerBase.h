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
	void Disconnect(int ID);

private:
	SOCKET ServerSocket, ClientSocket;
	HANDLE IOCPHandle;
	OverlappedEx GlobalOverlapped;

	concurrency::concurrent_unordered_map<int, Client> clinets;
	MatchingManager matchingManager;

	concurrency::concurrent_unordered_map<unsigned short,
		std::function<void(unsigned char*)>> handlerMap;
	concurrency::concurrent_unordered_map<unsigned short, GameMode*> gameRoomMap;

	std::vector<thread> workerThreads;
};

