#pragma once
//vector�� ���� recv ���� �޸� ����

class Client
{
public:
	int ID;
	int RoomID = -1;

	SOCKET socket;
	int prevRemainData;
public:
	Client();
	~Client();

	void SendPacket(void* packet);
	void RecvPacket();

	void SendServerLoginPacket(const int id);
private:
	OverlappedEx recv;
};

