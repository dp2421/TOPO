#pragma once

class MatchingManager
{
public:
	MatchingManager();
	~MatchingManager();

private:
	ThreadSafeQueue<Client*> queue;
};