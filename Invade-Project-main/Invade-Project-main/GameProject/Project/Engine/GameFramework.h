#pragma once
class CGameFramework
{
	SINGLE(CGameFramework)
public:
	HWND m_hMainhWnd;
	SOCKET socket;

	std::unordered_map<int, CGameObject*> networkObjects;
private:

	POINT m_ptOldCursorPos;
	Vec2 m_vMouseMove;

public:
	int Init(HWND _hWnd, const tResolution& _resolution, bool _bWindow);
	void Progress();
	void ProcessInput();
	void OnProcessingMouseMessage(HWND _hWnd, UINT _uMessageID, WPARAM _wParam, LPARAM _lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND _hWnd, UINT _uMessageID, WPARAM _wParam, LPARAM _lParam);

	const Vec2& GetMouseMove() { return m_vMouseMove; }

	void InitNetwork();
	void RecvPacket();
	void SendPacket(void* packet);
	void SendClientLoginPacket();
	void SendClientKeyInputPacket(const int key, Vec3 dir);
private:
	void ChangeWindowSize(HWND _hWnd, const tResolution _resolution);

};

WSAOVERLAPPED overlapped;
WSABUF wsaBuf;
char sendBuf[BUFFERSIZE];

WSAOVERLAPPED recvOverlapped;
WSABUF recvWsaBuf;
char recvBuf[BUFFERSIZE];

void CALLBACK RecvCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK SendCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
