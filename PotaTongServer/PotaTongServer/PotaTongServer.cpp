#include "pch.h"


// �߻�ȭ �۾� ����

void ProcessPacket(int key, char* packet)
{
	// ��Ŷ �۾�
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
	// �������
	// ������, �κ� ����
	// �������̸� ���� �׷쿡�� ����
	// �κ�� �׳� ����
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