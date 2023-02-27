#pragma once

constexpr int MatchingTime = 60;

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

	void CompleteMatching();

private:
	
	HANDLE handle;

	std::mutex mutex;
	std::atomic<int> roomID;
	std::atomic<bool> isDoMatching;
	ThreadSafeQueue<Client*> queue;
};