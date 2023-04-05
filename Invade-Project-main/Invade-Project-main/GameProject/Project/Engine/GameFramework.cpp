#include "pch.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameFramework.h"
#include "Device.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "PathMgr.h"
#include "ConstantBuffer.h"
#include "ResMgr.h"
#include "RenderMgr.h"
#include "SceneMgr.h"
#include "EventMgr.h"
#include "InstancingMgr.h"

CGameFramework::CGameFramework():m_hMainhWnd(nullptr) {

}
CGameFramework::~CGameFramework() {
	closesocket(socket);
	WSACleanup();
}


int CGameFramework::Init(HWND _hWnd, const tResolution& _resolution, bool _bWindow)
{

	m_hMainhWnd = _hWnd;
	ChangeWindowSize(m_hMainhWnd, _resolution);
	ShowWindow(_hWnd, true);
	if (FAILED(CDevice::GetInst()->Init(_hWnd, _resolution, _bWindow)))
		return E_FAIL;
	CRenderMgr::GetInst()->Init(_hWnd, _resolution, _bWindow);
	CDevice::GetInst()->CreateConstantBuffer(L"TRANSFORM_MATRIX", sizeof(tTransform), 2048, CONST_REGISTER::b0);
	CDevice::GetInst()->CreateConstantBuffer(L"MATERIAL_PARAM", sizeof(tMtrlParam), 2048, CONST_REGISTER::b1);
	CDevice::GetInst()->CreateConstantBuffer(L"ANIM2D", sizeof(tMtrlParam), 2048, CONST_REGISTER::b2);
	
	CDevice::GetInst()->CreateConstantBuffer(L"LIGHT2D", sizeof(tLight2DInfo), 1, CONST_REGISTER::b3,true);
	CDevice::GetInst()->CreateConstantBuffer(L"LIGHT3D", sizeof(tLight3DInfo), 1, CONST_REGISTER::b4, true);
	CDevice::GetInst()->CreateConstantBuffer(L"GLOBAL VALUE", sizeof(tGlobalValue), 1, CONST_REGISTER::b5);

	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b3), 0);
	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b4), 0);


	CDevice::GetInst()->SetGlobalConstBufferToRegister(CDevice::GetInst()->GetCB(CONST_REGISTER::b5), 0);

	CPathMgr::Init();
	CKeyMgr::GetInst()->Init();
	CTimeMgr::GetInst()->Init();

	CResMgr::GetInst()->Init();

	CSceneMgr::GetInst()->Init();
	
	CEventMgr::GetInst()->Init();

	InitNetwork();
	SendClientLoginPacket();
	return S_OK;

}

void CGameFramework::InitNetwork()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN svr_addr;
	memset(&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(PORTNUM);
	inet_pton(AF_INET, SERVERIP, &svr_addr.sin_addr);
	WSAConnect(socket, reinterpret_cast<sockaddr*>(&svr_addr), sizeof(svr_addr), 0, 0, 0, 0);
	RecvPacket();
}

void CGameFramework::RecvPacket()
{
	DWORD recv_flag = 0;
	memset(&recvOverlapped, 0, sizeof(recvOverlapped));
	WSARecv(socket, &recvWsaBuf, 1, NULL, &recv_flag, &recvOverlapped, RecvCallback);
}

void CGameFramework::SendPacket(void* packet)
{
	wsaBuf.len = BUFFERSIZE;
	wsaBuf.buf = sendBuf;
	ZeroMemory(&overlapped, sizeof(overlapped));
	auto pack = reinterpret_cast<char*>(packet);
	memcpy(sendBuf, pack, pack[0]);

	WSASend(socket, &wsaBuf, 1, 0, 0, &overlapped, SendCallback);
}

void CGameFramework::SendClientLoginPacket()
{
	ClientLoginPacket packet;
	packet.size = sizeof(ClientLoginPacket);
	packet.type = ClientLogin;
	string playerName{ "p" };
	strcpy_s(packet.name, playerName.c_str());

	SendPacket(&packet);
}

void CGameFramework::SendClientKeyInputPacket(const int key, Vec3 dir)
{
	ClientKeyInputPacket packet;
	packet.size = sizeof(ClientKeyInputPacket);
	packet.type = ClientKeyInput;
	packet.key = key;
	packet.x = dir.x;
	packet.y = dir.y;
	packet.z = dir.z;

	SendPacket(&packet);
}

void CALLBACK RecvCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	switch(recvWsaBuf.buf[0])
	{
	case ServerLogin:
	{
		auto p = reinterpret_cast<ServerLoginPacket*>(recvBuf);
		CGameFramework::GetInst()->networkObjects[p->id] = CSceneMgr::GetInst()->AddNetworkGameObject(true);
		break;
	}
	case ServerAddPlayer:
	{
		auto p = reinterpret_cast<ServerAddPlayerPacket*>(recvBuf);
		CGameFramework::GetInst()->networkObjects[p->id] = CSceneMgr::GetInst()->AddNetworkGameObject(false);
		CGameFramework::GetInst()->networkObjects[p->id]->Transform()->SetLocalPos(Vec3(p->x, p->y, p->z));
		break;
	}
	case ServerPlayerInfo:
		auto p = reinterpret_cast<ServerPlayerInfoPacket*>(recvBuf);
		CGameFramework::GetInst()->networkObjects[p->id]->Transform()->SetLocalPos(Vec3(p->xPos, p->yPos, p->zPos));

		break;
	}
	CGameFramework::GetInst()->RecvPacket();
}

void CALLBACK SendCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	delete over;
}

void CGameFramework::Progress()
{

	CKeyMgr::GetInst()->Update();

	SleepEx(1, true);

	CTimeMgr::GetInst()->Update();
	CSound::g_pFMOD->update();
	CEventMgr::GetInst()->Clear();

	CSceneMgr::GetInst()->Update();
	CRenderMgr::GetInst()->Render();

	CEventMgr::GetInst()->Update();
}

void CGameFramework::ProcessInput()
{

	POINT ptCursorPos;
	
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		m_vMouseMove.x = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.f;
		m_vMouseMove.y = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.f;
		m_vMouseMove.y *= -1.f;

		m_ptOldCursorPos = ptCursorPos;
	//	SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	
}

void CGameFramework::OnProcessingMouseMessage(HWND _hWnd, UINT _uMessageID, WPARAM _wParam, LPARAM _lParam)
{
	switch (_uMessageID)
	{
		
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(_hWnd);
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
		ReleaseCapture();
	default:
		break;
	}
}

LRESULT CGameFramework::OnProcessingWindowMessage(HWND _hWnd, UINT _uMessageID, WPARAM _wParam, LPARAM _lParam)
{
	switch (_uMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(_hWnd, _uMessageID, _wParam, _lParam);
		break;
	default:
		break;
	}
	return (0);
}

void CGameFramework::ChangeWindowSize(HWND _hWnd, const tResolution _resolution)
{
	RECT rt = { 0,0,(int)_resolution.fWidth,(int)_resolution.fHeight };

	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

}


