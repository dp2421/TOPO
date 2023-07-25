#pragma once

// HANDLE �޾Ƽ� �ʱ�ȭ
// �ο� ���� or �ð� ��� �� PQCS �̿� ��Ī �Ϸ� ����

class MatchingManager
{
public:
	MatchingManager(HANDLE handle);
	~MatchingManager();

	const int& getRoomID() { return roomID; }

	void DoMatching(Client* client, concurrency::concurrent_priority_queue<Event>& eventQueue);
	void ExitMatching(Client* client);

	int CompleteMatching(const int roomID, MapType mapType);

private:
	HANDLE handle;

	std::mutex lock;
	std::atomic<int> roomID;
	std::atomic<bool> isDoMatchingRacing;
	std::atomic<bool> isDoMatchingObstacle;
	ThreadSafeQueue<Client*> racingQueue;
	ThreadSafeQueue<Client*> obstacleQueue;
};