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
	// 클라이언트 매칭 입력시
	// 매칭중일시 -> 큐에 추가만
	// 매칭 아닐시 -> 매칭 시작, 큐에 추가
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
	// 동일 클라이언트 삭제
	std::unique_lock<std::mutex> lock(this->mutex);

	this->queue.Erase(client);
}

void MatchingManager::CompleteMatching()
{
	// 한꺼번에 다 빼기 고려
	this->isDoMatching = false;
	roomID++;
}
