#pragma once
#include "stdafx.h"
#include "../../Protocol.h"

class Client
{
public:
	int ID;
	int RoomID;

public:
	Client();
	~Client();

	void SendPacket();
	void RecvPacket();

private:
	OverlappedEx recv;
};