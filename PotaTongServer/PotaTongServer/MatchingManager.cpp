#include "pch.h"
#include "MatchingManager.h"

MatchingManager::MatchingManager(HANDLE handle)
	: handle(handle), roomID(0), isDoMatchingRacing(false), isDoMatchingObstacle(false)
{

}

MatchingManager::~MatchingManager()
{

}

void MatchingManager::DoMatching(Client* client, concurrency::concurrent_priority_queue<Event>& eventQueue)
{
	lock_guard<mutex> lock{ this->lock };
	// 클라이언트 매칭 입력시
	// 매칭중일시 -> 큐에 추가만
	// 매칭 아닐시 -> 매칭 시작, 큐에 추가
	if (client->mapType == MapType::Racing)
	{
		if (!this->isDoMatchingRacing)
		{
			this->isDoMatchingRacing = true;
			OverlappedEx* over = new OverlappedEx;
			over->type = OverlappedType::MatchingRacingComplete;

			Event matchingEvent;
			matchingEvent.objID = roomID;
			matchingEvent.excuteTime = chrono::system_clock::now() + static_cast<chrono::seconds>(MatchingTime);
			eventQueue.push(matchingEvent);
			this->racingQueue.Push(client);

			roomID++;
		}
		else
		{
			this->racingQueue.Push(client);
		}
	}
	else if (client->mapType == MapType::Obstacle)
	{
		if (!this->isDoMatchingObstacle)
		{
			this->isDoMatchingObstacle = true;
			OverlappedEx* over = new OverlappedEx;
			over->type = OverlappedType::MatchingObstacleComplete;

			Event matchingEvent;
			matchingEvent.objID = roomID;
			matchingEvent.excuteTime = chrono::system_clock::now() + static_cast<chrono::seconds>(MatchingTime);
			eventQueue.push(matchingEvent);
			this->obstacleQueue.Push(client);

			roomID++;
		}
		else
		{
			this->obstacleQueue.Push(client);
		}
	}
}

void MatchingManager::ExitMatching(Client* client)
{
	// 동일 클라이언트 삭제
	//lock_guard<mutex> lock{ this->lock };

	//this->queue.Erase(client);
}

void MatchingManager::CompleteMatching(const int roomID, MapType mapType)
{
	lock_guard<mutex> lock{ this->lock };
	// 한꺼번에 다 빼기 고려
	if (mapType == MapType::Racing)
	{
		this->isDoMatchingRacing = false;
		Client* client;
		while (true)
		{
			if (true == this->racingQueue.TryPop(client))
			{
				lock_guard<mutex> lock{ client->lock };
				client->RoomID = roomID;
			}
			else break;
		}
	}
	else if (mapType == MapType::Obstacle)
	{
		this->isDoMatchingObstacle = false;
		Client* client;
		while (true)
		{
			if (true == this->obstacleQueue.TryPop(client))
			{
				lock_guard<mutex> lock{ client->lock };
				client->RoomID = roomID;
			}
			else break;
		}
	}
}
