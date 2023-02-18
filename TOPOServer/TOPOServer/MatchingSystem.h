#pragma once

#include "ThreadSafeQueue.h"
#include "Client.h"

class MatchingSystem
{
public:
	MatchingSystem();
	~MatchingSystem();

private:
	ThreadSafeQueue<Client*> queue;
};