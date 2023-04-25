#pragma once

enum class EventType
{
	MatchingStart,
	MatchingComplete,
	Update
};

class Event
{
public:
	int objID;
	EventType eventType;
	chrono::system_clock::time_point excuteTime;

	constexpr bool operator < (const Event& L) const
	{
		return (excuteTime > L.excuteTime);
	}
};

