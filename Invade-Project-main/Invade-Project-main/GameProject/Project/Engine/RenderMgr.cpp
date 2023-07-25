#include "pch.h"
#include "RenderMgr.h"

#include "Device.h"
#include "Camera.h"
#include "SceneMgr.h"
#include "ConstantBuffer.h"
#include "TimeMgr.h"
#include "EventMgr.h"
#include "Light3D.h"

#include "UIScript.h"
#include "ResMgr.h"
#include "MRT.h"
CRenderMgr::CRenderMgr()
	:m_arrMRT{}, m_iRTVHeapSize(0)
{}
CRenderMgr::~CRenderMgr() {
	Safe_Delete_Array(m_arrMRT);
}


void CRenderMgr::Render()
{

	float arrColor[4] = { 0.f,0.f, 0.f, 1.f };
	CDevice::GetInst()->Render_Start(arrColor);

	static CConstantBuffer* pGlobalBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b5);
	CDevice::GetInst()->SetConstBufferToRegister(pGlobalBuffer, pGlobalBuffer->AddData(&g_global));
	// 광원 정보 업데이트
	UpdateLight2D();
	UpdateLight3D();

	// SwapChain MRT 초기화
	UINT iIdx = CDevice::GetInst()->GetSwapChainIndex();
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Clear(iIdx);

	// DeferredMRT 초기화
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Clear();

	// LightMRT 초기화
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Clear();

	// PostEffectMRT 초기화
	m_arrMRT[(UINT)MRT_TYPE::POSTEFFECT]->Clear();

	//

	m_vecCam[0]->SortGameObject();

	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();
	m_vecCam[0]->Render_Deferred();
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->TargetToResBarrier();

	// 윤곽선 렌더링

	Render_ShadowMap();

	Render_Lights();

	Merge_Light();

	m_vecCam[0]->Render_Forward(); // skybox, grid


#if LOCALPLAY
	if (CSceneMgr::GetInst()->GetSceneType() == SCENE_TYPE::LOBBY)
	{
		m_vecCam[1]->SortUIObject();
		m_vecCam[1]->Render_UI();
	}
	else
	{
		m_vecCam[1]->SortUIObject();
		m_vecCam[1]->Render_UI();

	}
#endif

	m_vecCam[1]->SortUIObject();
	m_vecCam[1]->Render_UI();


	////PostProcess Effect
	//Render_PostEffect();



	// 출력
	CDevice::GetInst()->Render_Present();

	if (b_SceneChanged == true)
	{
		CSceneMgr::GetInst()->ChangeScene(m_sceneType);
		b_SceneChanged = false;
	}


}

void CRenderMgr::Render_PostEffect()
{
	//여기서 이제... setPipeLineState해주는
	static Ptr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	static Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");

	pMtrl->UpdateData();
	pRectMesh->Render();

}


void CRenderMgr::Render_Tool()
{
	float arrColor[4] = { 0.f,0.f,0.f,1.f };
	UpdateLight2D();
	UpdateLight3D();
}

void Render_OutLine()
{
	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::OUTLINE)->Clear();
	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::OUTLINE)->OMSet();

	//// 렌더링 코드

	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::OUTLINE)->TargetToResBarrier();
}


void CRenderMgr::Render_ShadowMap()
{
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SHADOWMAP)->Clear();
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SHADOWMAP)->OMSet();
	for (UINT i = 0; i < m_vecLight3D.size(); ++i) {
		if (m_vecLight3D[i]->GetLight3DInfo().iLightType != (UINT)LIGHT_TYPE::DIR)
			continue;
		m_vecLight3D[i]->Render_ShadowMap();
	}
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SHADOWMAP)->TargetToResBarrier();
}

void CRenderMgr::Render_UI()
{
	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::UI)->Clear();

	//CRenderMgr::GetInst()->GetMRT(MRT_TYPE::UI)->OMSet();
}

void CRenderMgr::Render_Lights()
{
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->OMSet();

	CCamera* pMainCam = CRenderMgr::GetInst()->GetCamera(0);
	if (nullptr == pMainCam)
		return;
	g_transform.matView = pMainCam->GetViewMat();
	g_transform.matProj = pMainCam->GetProjMat();
	g_transform.matViewInv = pMainCam->GetViewInvMat();

	// 광원을 그린다.
	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		if (m_vecLight3D[i]->Light3D()->GetLight3DInfo().iLightType == (int)LIGHT_TYPE::DIR)
		{
			if (b_isFever)
				m_vecLight3D[i]->Light3D()->SetDiffuseColor(Vec3(0.06f, 0.149f, 0.4f));
		}

		if (m_vecLight3D[i]->Light3D()->GetLight3DInfo().iLightType == (int)LIGHT_TYPE::POINT)
		{
			f_lightpow += 0.01;
			if (f_lightpow > 1)
				f_lightpow = 0.f;
			m_vecLight3D[i]->Light3D()->SetAmbient(Vec3(f_lightpow, f_lightpow, f_lightpow));
		}

		m_vecLight3D[i]->Light3D()->Render();
	}

	m_vecLight3D.clear();
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->TargetToResBarrier();

	// SwapChain MRT 셋팅
	UINT iIdx = CDevice::GetInst()->GetSwapChainIndex();
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet(1, iIdx);
}


void CRenderMgr::Merge_Light()
{
	static Ptr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	static Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");

	pMtrl->UpdateData();
	pRectMesh->Render();
}

void CRenderMgr::PlaySound()
{
	SCENE_TYPE curscene = CSceneMgr::GetInst()->GetSceneType();
	switch (curscene)
	{
	case SCENE_TYPE::LOBBY:
		for (int i = 0; i < (int)SOUND_TYPE::END; ++i)
			if (i != (int)SOUND_TYPE::LOBBY)
				m_sounds[i]->Stop();
		m_sounds[(int)SOUND_TYPE::LOBBY]->Play(0);
		break;
	case SCENE_TYPE::RACING:
		for (int i = 0; i < (int)SOUND_TYPE::END; ++i)
			if (i != (int)SOUND_TYPE::RACING)
				m_sounds[i]->Stop();
		m_sounds[(int)SOUND_TYPE::RACING]->Play(0);
		break;
	case SCENE_TYPE::JUMP:
		for (int i = 0; i < (int)SOUND_TYPE::END; ++i)
			if (i != (int)SOUND_TYPE::SURVIVAL)
				m_sounds[i]->Stop();
		m_sounds[(int)SOUND_TYPE::SURVIVAL]->Play(0);
		break;
	case SCENE_TYPE::METOR:
		for (int i = 0; i < (int)SOUND_TYPE::END; ++i)
			if (i != (int)SOUND_TYPE::SURVIVAL)
				m_sounds[i]->Stop();
		m_sounds[(int)SOUND_TYPE::SURVIVAL]->Play(0);
		break;
	case SCENE_TYPE::AWARD:
		for (int i = 0; i < (int)SOUND_TYPE::END; ++i)
			if (i != (int)SOUND_TYPE::LOBBY)
				m_sounds[i]->Stop();
		m_sounds[(int)SOUND_TYPE::LOBBY]->Play(0);
		break;
	default:
		break;
	}
}


void CRenderMgr::UpdateLight2D()
{
	static CConstantBuffer* pLight2DBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b3);
	UINT iOffsetPos = pLight2DBuffer->AddData(&m_tLight2DInfo);
	CDevice::GetInst()->SetConstBufferToRegister(pLight2DBuffer, iOffsetPos);
	m_tLight2DInfo.iCount = 0;
}

void CRenderMgr::UpdateLight3D()
{
	static CConstantBuffer* pLight3DBuffer = CDevice::GetInst()->GetCB(CONST_REGISTER::b4);
	tLight3DInfo tLight3DArray;
	for (size_t i = 0; i < m_vecLight3D.size(); ++i) {
		const tLight3D& info = m_vecLight3D[i]->GetLight3DInfo();
		tLight3DArray.arrLight3D[i] = info;
	}
	tLight3DArray.iCurCount = (UINT)m_vecLight3D.size();
	UINT iOffsetPos = pLight3DBuffer->AddData(&tLight3DArray);
	CDevice::GetInst()->SetConstBufferToRegister(pLight3DBuffer, iOffsetPos);
}

void CRenderMgr::CreateMRT()
{
	// RenderTargetViewHeap 의 메모리 사이즈
	m_iRTVHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	g_global.vResolution = m_tResolution;

	// =============
	// SwapChain MRT
	// =============
	wchar_t szRTVName[50] = {};
	tRT arrRT[2] = {};

	for (UINT i = 0; i < 2; ++i)
	{
		wsprintf(szRTVName, L"SwapchainTargetTex_%d", i);
		ComPtr<ID3D12Resource> pTarget;
		CDevice::GetInst()->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&pTarget));
		arrRT[i].pTarget = CResMgr::GetInst()->CreateTextureFromResource(szRTVName, pTarget);
	}

	Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"DepthStencilTex"
		, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
		, DXGI_FORMAT_D32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
		, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);



	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(2, arrRT, pDSTex);


	// ============
	// Deferred MRT
	// ============
	{
		tRT arrRT[8] = {};

		arrRT[0].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[0].pTarget = CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
			, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
			, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

		arrRT[1].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[1].pTarget = CResMgr::GetInst()->CreateTexture(L"NormalTargetTex"
			, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
			, DXGI_FORMAT_R32G32B32A32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

		arrRT[2].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[2].pTarget = CResMgr::GetInst()->CreateTexture(L"PositionTargetTex"
			, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
			, DXGI_FORMAT_R32G32B32A32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);


		m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(3, arrRT, pDSTex); // 깊이 텍스쳐는 SwapChain 것을 사용한다.
	}


	// ============
	// Light MRT
	// ============
	{
		tRT arrRT[8] = {};

		arrRT[0].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[0].pTarget = CResMgr::GetInst()->CreateTexture(L"DiffuseLightTargetTex"
			, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
			, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

		arrRT[1].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[1].pTarget = CResMgr::GetInst()->CreateTexture(L"SpecularLightTargetTex"
			, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
			, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(2, arrRT, pDSTex); // 깊이 텍스쳐는 SwapChain 것을 사용한다.
	}
	{
		tRT arrRT[8] = {};

		arrRT[0].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[0].pTarget = CResMgr::GetInst()->CreateTexture(L"ShadowMapTargetTex"
			, 4096, 4096
			, DXGI_FORMAT_R32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

		// 별도의 깊이버퍼를 가짐
		Ptr<CTexture> pDSTex = CResMgr::GetInst()->CreateTexture(L"ShadowMapDepthTex"
			, 4096, 4096
			, DXGI_FORMAT_D32_FLOAT, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		m_arrMRT[(UINT)MRT_TYPE::SHADOWMAP] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::SHADOWMAP]->Create(1, arrRT, pDSTex); // 별도의 깊이버퍼 를 가짐
	}

	// ==============
	// POSTEFFECT MRT
	// ==============
	{
		tRT arrRT[8] = {};

		arrRT[0].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[0].pTarget = CResMgr::GetInst()->CreateTexture(L"PostEffectTex"
			, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
			, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

		m_arrMRT[(UINT)MRT_TYPE::POSTEFFECT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::POSTEFFECT]->Create(1, arrRT, pDSTex); // 깊이 텍스쳐는 SwapChain 것을 사용한다.
	}



	//{
	//	tRT arrRT[8] = {};

	//	arrRT[0].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
	//	arrRT[0].pTarget = CResMgr::GetInst()->CreateTexture(L"OutlineTargetTex"
	//		, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
	//		, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
	//		, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

	//	m_arrMRT[(UINT)MRT_TYPE::OUTLINE] = new CMRT;
	//	m_arrMRT[(UINT)MRT_TYPE::OUTLINE]->Create(1, arrRT, pDSTex); // 깊이 텍스쳐는 SwapChain 것을 사용한다.
	//}

	// ============
	// UI MRT
	// ============

	{
		tRT arrRT[8] = {};

		arrRT[0].vClearColor = Vec4(0.f, 0.f, 0.f, 0.f);
		arrRT[0].pTarget = CResMgr::GetInst()->CreateTexture(L"UITargetTex"
			, (UINT)m_tResolution.fWidth, (UINT)m_tResolution.fHeight
			, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE
			, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, arrRT[0].vClearColor);

		m_arrMRT[(UINT)MRT_TYPE::UI] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::UI]->Create(1, arrRT, pDSTex); // 깊이 텍스쳐는 SwapChain 것을 사용한다.
	}
}

CCamera* CRenderMgr::GetMainCam()
{
	return m_vecCam[0];
}

