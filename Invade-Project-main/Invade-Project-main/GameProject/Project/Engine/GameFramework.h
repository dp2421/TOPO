#pragma once
class CGameFramework
{
	SINGLE(CGameFramework)
public:
	HWND m_hMainhWnd;
	Vec2 m_WinSize;
	POINT m_ptOldCursorPos;

	bool m_isClick = false;
private:
	Vec2 m_vMouseMove;

public:
	int Init(HWND _hWnd, const tResolution& _resolution, bool _bWindow);
	void Progress();
	void ProcessInput();
	void OnProcessingMouseMessage(HWND _hWnd, UINT _uMessageID, WPARAM _wParam, LPARAM _lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND _hWnd, UINT _uMessageID, WPARAM _wParam, LPARAM _lParam);

	const Vec2& GetMouseMove() { return m_vMouseMove; }
	const bool GetIsClicked() { return m_isClick; }
	const void SetIsClicked(bool click) { m_isClick = click; }

private:
	void ChangeWindowSize(HWND _hWnd, const tResolution _resolution);

};