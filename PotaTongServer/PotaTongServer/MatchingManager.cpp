#include "pch.h"
#include "MatchingManager.h"

MatchingManager::MatchingManager(HANDLE handle)
	: handle(handle), roomID(0), isDoMatching(false)
{

}

MatchingManager::~MatchingManager()
{

}

void MatchingManager::DoMatching(Client* client, concurrency::concurrent_priority_queue<Event>& eventQueue)
{
	// Ŭ���̾�Ʈ ��Ī �Է½�
	// ��Ī���Ͻ� -> ť�� �߰���
	// ��Ī �ƴҽ� -> ��Ī ����, ť�� �߰�
	if (this->isDoMatching)
	{
		this->queue.Push(client);
	}
	else
	{
		this->isDoMatching = true;
		OverlappedEx* over = new OverlappedEx;
		over->type = OverlappedType::ServerEvent;

		Event matchingEvent;
		matchingEvent.objID = roomID;
		matchingEvent.excuteTime = chrono::system_clock::now() + static_cast<chrono::seconds>(MatchingTime);
		eventQueue.push(matchingEvent);
	}
}

void MatchingManager::ExitMatching(Client* client)
{
	// ���� Ŭ���̾�Ʈ ����
	std::unique_lock<std::mutex> lock(this->mutex);

	this->queue.Erase(client);
}

void MatchingManager::CompleteMatching()
{
	// �Ѳ����� �� ���� ���
	this->isDoMatching = false;
	roomID++;
}
