#pragma once
//vector를 통한 recv 버퍼 메모리 관리

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

