#pragma once

enum class OverlappedType
{
	Accept,
	Recv,
	Send,
	MatchingStart,
	MatchingComplete,
	RotateObs,
	SendRotateInfo,
	AddAI,
	UpdateAI,
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
		wsaBuf.len = (unsigned char)packet[0];
		wsaBuf.buf = sendBuf;
		type = OverlappedType::Send;
		ZeroMemory(&overlapped, sizeof(overlapped));

		auto pack = reinterpret_cast<char*>(packet);
		memcpy(sendBuf, pack, (unsigned char)pack[0]);
	}
};
