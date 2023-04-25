#pragma once

enum class OverlappedType
{
	Accept,
	Recv,
	Send,
	MatchingStart,
	MatchingComplete,
	Update
};

class OverlappedEx
{
public:
	WSAOVERLAPPED overlapped;
	WSABUF wsaBuf;
	char sendBuf[BUFFERSIZE];
	OverlappedType type;

	OverlappedEx()
	{
		wsaBuf.len = BUFFERSIZE;
		wsaBuf.buf = sendBuf;
		type = OverlappedType::Recv;
		ZeroMemory(&overlapped, sizeof(overlapped));
	}

	OverlappedEx(char* packet)
	{
		wsaBuf.len = packet[0];
		wsaBuf.buf = sendBuf;
		type = OverlappedType::Send;
		ZeroMemory(&overlapped, sizeof(overlapped));

		auto pack = reinterpret_cast<char*>(packet);
		memcpy(sendBuf, pack, pack[0]);
	}
};
