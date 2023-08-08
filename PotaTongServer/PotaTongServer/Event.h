#pragma once

class Event
{
public:
	int objID;
	OverlappedType eventType;
	chrono::system_clock::time_point excuteTime;

	constexpr bool operator < (const Event& L) const
	{
		return (excuteTime > L.excuteTime);
	}
};

