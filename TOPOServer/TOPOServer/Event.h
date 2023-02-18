#pragma once

enum class EventType
{
	MatchingComplite,
	Update
};

class Event
{
	int eventId;
	int targetId;
};