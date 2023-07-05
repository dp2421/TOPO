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
		wsaBuf.len = reinterpret_cast<PACKETSIZE*>(packet)[0];
		wsaBuf.buf = sendBuf;
		type = OverlappedType::Send;
		ZeroMemory(&overlapped, sizeof(overlapped));

		memcpy(sendBuf, packet, reinterpret_cast<PACKETSIZE*>(packet)[0]);
	}
};
