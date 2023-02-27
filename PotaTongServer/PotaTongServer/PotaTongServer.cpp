#include "pch.h"


// 추상화 작업 예정

void ProcessPacket(int key, char* packet)
{
	// 패킷 작업
	switch (packet[1])
	{
	case ClientMatching:
		break;
	}
}

void AcceptClient()
{
	cout << "Accept Success" << endl;
}

void DisconnectClient(int id)
{
	// 연결끊기
	// 게임중, 로비 구별
	// 게임중이면 같은 그룹에게 전달
	// 로비면 그냥 끊기
}

void EventThread()
{

}

void WorkerThread(HANDLE IOCP)
{
}

void ServerSetting()
{

}

int main()
{
	ServerSetting();

	vector <thread> workerThreads;
	int numThreads = std::thread::hardware_concurrency();

	thread eventThread{ EventThread };
	for (int i = 0; i < numThreads; ++i)
		workerThreads.emplace_back(WorkerThread, IOCPHandle);

	eventThread.join();
	for (auto& thread : workerThreads)
		thread.join();


	closesocket(ServerSocket);
	WSACleanup();
}