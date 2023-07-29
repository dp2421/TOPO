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
	else if (client->mapType == MapType::Jump)
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

int MatchingManager::CompleteMatching(const int roomID, MapType mapType)
{
	lock_guard<mutex> lock{ this->lock };
	int count = 0;
	// �Ѳ����� �� ���� ���
	if (mapType == MapType::Racing)
	{
		count = racingQueue.Size();
		this->isDoMatchingRacing = false;
		Client* client;
		for(int i = 0; i < count; ++i)
		{
			if (true == this->racingQueue.TryPop(client))
			{
				if (client->RoomID != -1) break;;
				lock_guard<mutex> lock{ client->lock };
				client->RoomID = roomID;
				client->mapType = mapType;
				client->SendMatchingOKPacket(mapType);
				client->position = PlayerStartPos;
				client->position.x += PlayerStartDistance * i;
			}
			else break;
		}
	}
	else
	{
		count = obstacleQueue.Size();
		this->isDoMatchingObstacle = false;
		Client* client;
		for (int i = 0; i < count; ++i)
		{
			if (true == this->obstacleQueue.TryPop(client))
			{
				if (client->RoomID != -1) break;
				lock_guard<mutex> lock{ client->lock };
				client->RoomID = roomID;
				client->mapType = mapType;
				client->SendMatchingOKPacket(mapType);
				if (mapType == MapType::Jump)
				{
					client->position = JumpStartPos;
					client->position += JumpStartDistance * i;
				}
				else if (mapType == MapType::Meteo)
				{
					client->position = MeteoStartPos;
					client->position += MeteoStartDistance * i;
				}
			}
			else break;
		}
	}

	return count;
}
