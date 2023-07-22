#pragma once
class CBlendState;
class CDepthStencilState;
class CCamera;
class CLight3D;

class CRenderTarget24;
class CMRT;

#include "Sound.h"
#include "Ptr.h"
#include "Texture.h"


class CRenderMgr
{
	SINGLE(CRenderMgr)
private:
	Ptr<CTexture> m_pRenderTargetTex;
	Ptr<CTexture> m_pDepthStencilTex;

	tResolution m_tResolution;
	CMRT* m_arrMRT[(UINT)MRT_TYPE::END];

	tLight2DInfo m_tLight2DInfo;
	
	vector<CLight3D*> m_vecLight3D;
	vector<CCamera*> m_vecCam;

	HWND m_hWnd;

	bool m_bWindowed;

	UINT m_iRTVHeapSize;

	bool b_SceneChanged = false;
	SCENE_TYPE m_sceneType;

	CSound* m_sounds[(int)SOUND_TYPE::END];

	bool b_isFever = false;
	bool b_isMatchComplete = false;

public:
	void Init(HWND _hWnd, const tResolution& _res, bool _bWindow);
	void Render();
	void Render_Tool();
	void Render_ShadowMap();
	void Render_UI();
	void Render_Lights();
	void Merge_Light();
	void PlaySound();
	void Render_PostEffect();
	//void Render_OutLine();
private:

	void UpdateLight2D();
	void UpdateLight3D();

	void CreateMRT();

public:

	void RegisterLight2D(const tLight2D& _Light2D) {
		if (100 <= m_tLight2DInfo.iCount)
			return;
		m_tLight2DInfo.arrLight2D[m_tLight2DInfo.iCount++] = _Light2D;
	}
	int RegisterLight3D(CLight3D* _pLight3D){
		if (100 <= m_vecLight3D.size()) {
			return -1;
		}
		m_vecLight3D.push_back(_pLight3D);
		return (int)m_vecLight3D.size() - 1;
	}

	CCamera* GetCamera(int _iIdx) { return m_vecCam[_iIdx]; }
	CCamera* GetMainCam();
	void RegisterCamera(CCamera* _pCamera) {
		m_vecCam.push_back(_pCamera);
	}

	void ClearCamera() { m_vecCam.clear(); }

	HWND GethWnd() { return m_hWnd; }

	tResolution GetResolution() { return m_tResolution; }
	UINT GetRTVHeapSize() { return m_iRTVHeapSize; }
	
	
	CMRT* GetMRT(MRT_TYPE _eType) { return m_arrMRT[(UINT)_eType]; }
	friend class CSceneMgr;

	void SetSceneChanged(bool sc) { b_SceneChanged = true; }
	void SetSceneType(SCENE_TYPE type) { m_sceneType = type; }

	void SetSound(CSound* sound, SOUND_TYPE type) { m_sounds[(int)type] = sound; }

	void SetMatchComplete(bool complete) { b_isMatchComplete = complete; }
	bool GetMatchComplete() { return b_isMatchComplete; }
};

