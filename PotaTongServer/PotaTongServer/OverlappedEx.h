#pragma once

enum class OverlappedType
{
	Accept,
	Recv,
	Send,
	ServerEvent
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
		ZeroMemory(&overlapped, sizeof(overlapped));
	}

	OverlappedEx(void* packet)
	{
		wsaBuf.len = BUFFERSIZE;
		wsaBuf.buf = sendBuf;
		ZeroMemory(&overlapped, sizeof(overlapped));

		auto pack = reinterpret_cast<char*>(packet);
		memcpy(sendBuf, pack, pack[0]);
	}
};
