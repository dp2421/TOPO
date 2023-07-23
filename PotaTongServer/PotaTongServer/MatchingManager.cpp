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
	cout << "Matching Start " << endl;
	lock_guard<mutex> lock{ this->lock };
	// Ŭ���̾�Ʈ ��Ī �Է½�
	// ��Ī���Ͻ� -> ť�� �߰���
	// ��Ī �ƴҽ� -> ��Ī ����, ť�� �߰�
	if (client->mapType == MapType::Racing)
	{
		if (!this->isDoMatchingRacing)
		{
			this->isDoMatchingRacing = true;

			Event matchingEvent{ roomID, OverlappedType::MatchingRacingComplete, chrono::system_clock::now() + static_cast<chrono::seconds>(MatchingTime) };
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

			Event matchingEvent{ roomID, OverlappedType::MatchingObstacleComplete, chrono::system_clock::now() + static_cast<chrono::seconds>(MatchingTime) };
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
	// ���� Ŭ���̾�Ʈ ����
	//lock_guard<mutex> lock{ this->lock };

	//this->queue.Erase(client);
}

void MatchingManager::CompleteMatching(const int roomID, MapType mapType)
{
	lock_guard<mutex> lock{ this->lock };
	// �Ѳ����� �� ���� ���
	if (mapType == MapType::Racing)
	{
		this->isDoMatchingRacing = false;
		Client* client;
		while (true)
		{
			if (true == this->racingQueue.TryPop(client))
			{
				if (client->RoomID != -1) return;
				lock_guard<mutex> lock{ client->lock };
				client->RoomID = roomID;
				client->SendMatchingOKPacket(mapType);
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
				if (client->RoomID != -1) return;
				lock_guard<mutex> lock{ client->lock };
				client->RoomID = roomID;
				client->SendMatchingOKPacket(mapType);
			}
			else break;
		}
	}
}
