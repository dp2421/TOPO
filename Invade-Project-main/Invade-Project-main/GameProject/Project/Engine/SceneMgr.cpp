#include "pch.h"

#include "SceneMgr.h"

#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "ResMgr.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Animator2D.h"
#include "Animation2D.h"
#include "Light2D.h"
#include "Light3D.h"
#include "MeshData.h"
#include "Collider3D.h"
#include "Animator3D.h"
#include "Terrain.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Camera.h"

#include "CollisionMgr.h"
#include "EventMgr.h"
#include "RenderMgr.h"
#include "Device.h"

#include "GridScript.h"
#include "CameraScript.h"
#include "PlayerScript.h"
#include "MonsterScript.h"
#include "ObstacleScript.h"
#include "ParticleSystem.h"
#include "ArrowScript.h"
#include "GameFramework.h"

CScene* CSceneMgr::GetCurScene()
{
	return m_pCurScene;
}

void CSceneMgr::ChangeScene(CScene* _pNextScene)
{
	SAFE_DELETE(m_pCurScene);
	m_pCurScene = _pNextScene;
}

CSceneMgr::CSceneMgr()
	: m_pCurScene(nullptr)
{
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
}

void CSceneMgr::Init()
{
	// 필요한 리소스 로딩
	// Texture 로드

	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"TestTex", L"Texture\\Health.png");
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>(L"Explosion", L"Texture\\Explosion\\Explosion80.png");
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>(L"Black", L"Texture\\asd.png");
	Ptr<CTexture> pSky01 = CResMgr::GetInst()->Load<CTexture>(L"Sky01", L"Texture\\Skybox\\Sky01.png");
	Ptr<CTexture> pSky02 = CResMgr::GetInst()->Load<CTexture>(L"FS000_Day_01", L"Texture\\Skybox\\FS000_Day_01.png");
	CResMgr::GetInst()->Load<CTexture>(L"Snow", L"Texture\\Particle\\Snow50px.png");
	CResMgr::GetInst()->Load<CTexture>(L"smokeparticle", L"Texture\\Particle\\smokeparticle.png");
	CResMgr::GetInst()->Load<CTexture>(L"HardCircle", L"Texture\\Particle\\HardCircle.png");
	CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");


	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");


	Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_3, pSky01.GetPointer());
//
	pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_2, pSky01.GetPointer());
//

	m_pCurScene = new CScene;
	m_pCurScene->SetName(L"Play Scene");

	m_pCurScene->GetLayer(0)->SetName(L"Default");
	m_pCurScene->GetLayer(1)->SetName(L"Player");
	m_pCurScene->GetLayer(2)->SetName(L"Monster");
	m_pCurScene->GetLayer(3)->SetName(L"Arrow");
	m_pCurScene->GetLayer(4)->SetName(L"Minion");
	m_pCurScene->GetLayer(5)->SetName(L"Tower");
	m_pCurScene->GetLayer(6)->SetName(L"temp");
	m_pCurScene->GetLayer(7)->SetName(L"Racing");
	m_pCurScene->GetLayer(8)->SetName(L"Obstacle");
	//m_pCurScene->GetLayer(9)->SetName(L"ObstacleMove");


	m_pCurScene->GetLayer(31)->SetName(L"Tool");

	CGameObject* pMainCam = nullptr;

	// Camera Object
	pMainCam = new CGameObject;
	pMainCam->SetName(L"MainCam");
	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraScript);

	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);

	//pMainCam->Transform()->SetLocalRot(Vec3(0.f, 3.14f, 0.f));
	pMainCam->Camera()->SetFar(100000.f);
	pMainCam->Camera()->SetLayerAllCheck();

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pMainCam);

	CGameObject* pObject = nullptr;

	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);
	pObject->Light3D()->SetLightPos(Vec3(0.f, 500.f, 0.f));
	pObject->Light3D()->SetLightType(LIGHT_TYPE::DIR);
	pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pObject->Light3D()->SetAmbient(Vec3(0.4f, 0.4f, 0.4f));
	pObject->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
	pObject->Light3D()->SetLightRange(1000.f);
	pObject->Transform()->SetLocalPos(Vec3(-1000.f, 1000.f, -1000.f));
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);

#pragma region TempObj


	//pObject = new CGameObject;
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLight3D);
	//pObject->Light3D()->SetLightPos(Vec3(250.f, 200.f, -30.f));
	//pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	//pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 0.f, 1.f));
	//pObject->Light3D()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
	//pObject->Light3D()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
	//pObject->Light3D()->SetLightDir(Vec3(-1.f, -1.f, -1.f));
	//pObject->Light3D()->SetLightRange(300.f);
	////pObject->Transform()->SetLocalPos(Vec3(-1000.f, 1000.f, -1000.f));
	//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);

	//pObject = new CGameObject;
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CLight3D);
	//pObject->Light3D()->SetLightPos(Vec3(350.f, 200.f, -30.f));
	//pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	//pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 0.f, 1.f));
	//pObject->Light3D()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
	//pObject->Light3D()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
	//pObject->Light3D()->SetLightDir(Vec3(-1.f, -1.f, -1.f));
	//pObject->Light3D()->SetLightRange(300.f);
	////pObject->Transform()->SetLocalPos(Vec3(-1000.f, 1000.f, -1000.f));
	//m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
	
//	pMeshData->Save(pMeshData->GetPath());
	// MeshData 로드

#pragma endregion

#if LOCALPLAY
	AddNetworkGameObject(true, Vec3::Zero);
#else
#endif

#pragma region TempObj

	//// Temp Object
	//pObject = new CGameObject;
	//pObject->SetName(L"Player Object");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//// Transform 설정
	//pObject->Transform()->SetLocalPos(Vec3(0.f, 100.f, 0.f));
	//pObject->Transform()->SetLocalScale(Vec3(1000.f, 1000.f, 100.f));
	//pObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	//// MeshRender 설정
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"));
	//pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pColor.GetPointer());
	//pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_1, pNormal.GetPointer());
	//pObject->MeshRender()->SetDynamicShadow(true);

	//m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject);

	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\JPlayerv1.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\JPlayerv1.mdat", L"MeshData\\JPlayerv1.mdat", false, true);
	//pObject = new CGameObject;
	//
	//pObject = pMeshData->Instantiate();
	//pObject->SetName(L"Monster");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->AddComponent(new CPlayerScript);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f,1.f,1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(50.f, 115.f, 100.f));
	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	//pObject->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	////
	////pMainCam->Transform()->SetLocalPos(Vec3(-60,45,-10));
	//////pMainCam->Transform()->SetLocalScale(Vec3(15000.f, 15000.f, 15000.f));
	////pMainCam->Transform()->SetLocalRot(Vec3(0, PI/2, -PI/18));
	////pObject->AddChild(pMainCam);
	//m_pCurScene->FindLayer(L"Monster")->AddGameObject(pObject, false);

	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Canon_min.mdat", L"MeshData\\Canon_min.mdat");
	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(100.f, 100.f, 150.f));
	//pObject->Transform()->SetLocalScale(Vec3(0.25f, 0.25f, 0.25f));
	//pObject->MeshRender()->SetDynamicShadow(true);

	//pObject->MeshRender()->GetMesh()->GetAnimClip();

	//m_pCurScene->FindLayer(L"Minion")->AddGameObject(pObject);

	// pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\C25.fbx");

	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(300.f, 110.f, 300.f));
	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(0);

	//m_pCurScene->FindLayer(L"Minion")->AddGameObject(pObject);

	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\C07.fbx");

	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(400.f, 100.f, 300.f));
	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);

	//m_pCurScene->FindLayer(L"Minion")->AddGameObject(pObject);
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\C07.fbx");

	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(500.f, 100.f, 300.f));
	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);

	//m_pCurScene->FindLayer(L"Minion")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Player_Idle.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	//
	//pObject = pMeshData->Instantiate();
	//pObject->SetName(L"IdlePlayer");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->AddComponent(new CPlayerScript);

	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	//pObject->FrustumCheck(false);
	////pObject->Transform()->SetLocalRot(Vec3(-3.14f / 2, -3.14f / 2, 0.f));
	//pObject->Transform()->SetLocalPos(Vec3(100.f, 115.f, 100.f));

	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);
	////pObject->GetScript<CPlayerScript>()->SetPlayable(true);
	//pObject->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	//pObject->GetScript<CPlayerScript>()->SetState(PLAYER_STATE::IDLE);
	//m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject);
	//pObject->SetActive(false);

	////pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Player_Run.fbx");
	////pMeshData->Save(pMeshData->GetPath());
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);
	//pObject = pMeshData->Instantiate();
	//pObject->SetName(L"RunPlayer");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->AddComponent(new CPlayerScript);

	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	//pObject->FrustumCheck(false);
	////pObject->Transform()->SetLocalRot(Vec3(-3.14f / 2, -3.14f / 2, 0.f));
	//pObject->Transform()->SetLocalPos(Vec3(100.f, 115.f, 100.f));

	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);
	////pObject->GetScript<CPlayerScript>()->SetPlayable(true);
	//pObject->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	//pObject->GetScript<CPlayerScript>()->SetState(PLAYER_STATE::RUN);
	//m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject);
	//pObject->SetActive(false);

	////pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Player_Walk.fbx");
	////pMeshData->Save(pMeshData->GetPath());
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Walk.mdat", L"MeshData\\Player_Walk.mdat", false, true);
	//pObject = pMeshData->Instantiate();
	//pObject->SetName(L"WalkPlayer");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->AddComponent(new CPlayerScript);

	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	//pObject->FrustumCheck(false);
	////pObject->Transform()->SetLocalRot(Vec3(-3.14f / 2, -3.14f / 2, 0.f));
	//pObject->Transform()->SetLocalPos(Vec3(100.f, 115.f, 100.f));

	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);
	////pObject->GetScript<CPlayerScript>()->SetPlayable(true);
	//pObject->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	//pObject->GetScript<CPlayerScript>()->SetState(PLAYER_STATE::WALK);
	//m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject);
	//pObject->SetActive(false);

	////pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Player_Happy.fbx");
	////pMeshData->Save(pMeshData->GetPath());
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Happy.mdat", L"MeshData\\Player_Happy.mdat", false, true);
	//pObject = pMeshData->Instantiate();
	//pObject->SetName(L"HappyPlayer");
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->AddComponent(new CPlayerScript);

	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	//pObject->FrustumCheck(false);
	////pObject->Transform()->SetLocalRot(Vec3(-3.14f / 2, -3.14f / 2, 0.f));
	//pObject->Transform()->SetLocalPos(Vec3(100.f, 115.f, 100.f));

	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);
	////pObject->GetScript<CPlayerScript>()->SetPlayable(true);
	//pObject->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	//pObject->GetScript<CPlayerScript>()->SetState(PLAYER_STATE::HAPPY);
	//m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject);
	//pObject->SetActive(false);

#pragma endregion

	///////////////////////////////////////////////////////////////
	////장애물테스트
	//360도
	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle10_2.fbx");
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10.mdat", L"MeshData\\Obstacle10.mdat");
	pMeshData->Save(pMeshData->GetPath());
	CGameObject* pMoveObs = nullptr;
	pMoveObs = new CGameObject;

	pMoveObs = pMeshData->Instantiate();
	pMoveObs->AddComponent(new CTransform);
	pMoveObs->AddComponent(new CCollider3D);
	pMoveObs->AddComponent(new CObstacleScript);
	pMoveObs->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);

	//pMoveObs->AddComponent(new CMeshRender);
	pMoveObs->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pMoveObs->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 100.f));
	pMoveObs->Collider3D()->SetOffsetPos(Vec3(0.f, 100.f, 100.f));
	pMoveObs->FrustumCheck(false);
	pMoveObs->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
	pMoveObs->Transform()->SetLocalPos(Vec3(600.f, 10.f, 50.f));

	pMoveObs->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pMoveObs->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(0);

	m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pMoveObs);

	//시계추회전
	pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle5_2.fbx");
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10.mdat", L"MeshData\\Obstacle10.mdat");
	pMeshData->Save(pMeshData->GetPath());
	pMoveObs = pMeshData->Instantiate();
	pMoveObs->AddComponent(new CTransform);
	pMoveObs->AddComponent(new CCollider3D);
	pMoveObs->AddComponent(new CObstacleScript);
	pMoveObs->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEB);

	//pMoveObs->AddComponent(new CMeshRender);
	pMoveObs->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pMoveObs->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pMoveObs->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pMoveObs->FrustumCheck(false);
	pMoveObs->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
	pMoveObs->Transform()->SetLocalPos(Vec3(500.f, 330.f, 50.f));

	pMoveObs->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pMoveObs->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(0);
	m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pMoveObs);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//360도회전장애물 : 2층(26) + 1층(17)
	{	//2층
		pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle10_2.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10.mdat", L"MeshData\\Obstacle10.mdat");
		//pMeshData->Save(pMeshData->GetPath());
		CGameObject* pObstaclesA;
		pObstaclesA = nullptr;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				for (int k = -1; k <= 1; k += 2)
				{
					pObstaclesA = pMeshData->Instantiate();
					pObstaclesA->AddComponent(new CTransform);
					pObstaclesA->AddComponent(new CCollider3D);
					pObstaclesA->AddComponent(new CObstacleScript);
					pObstaclesA->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);
					pObstaclesA->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
					pObstaclesA->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
					pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
					pObstaclesA->FrustumCheck(false);
					pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
					pObstaclesA->Transform()->SetLocalPos(Vec3(k*(1280.f + 280.f * i + 640.f * j), 10.f, 1200.f + 400.f * i));
					pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					pObstaclesA->MeshRender()->SetDynamicShadow(false);
					m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
				}
			}
		}
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				pObstaclesA = pMeshData->Instantiate();
				pObstaclesA->AddComponent(new CTransform);
				pObstaclesA->AddComponent(new CCollider3D);
				pObstaclesA->AddComponent(new CObstacleScript);
				pObstaclesA->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);
				pObstaclesA->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
				pObstaclesA->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
				pObstaclesA->FrustumCheck(false);
				pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesA->Transform()->SetLocalPos(Vec3(-1600.f + 600.f * i + 640.f, 10.f, 8960.f + 1280.f * j));
				pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->MeshRender()->SetDynamicShadow(false);
				m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
			}
		}
		for (int i = 0; i < 2; ++i)
		{
			pObstaclesA = pMeshData->Instantiate();
			pObstaclesA->AddComponent(new CTransform);
			pObstaclesA->AddComponent(new CCollider3D);
			pObstaclesA->AddComponent(new CObstacleScript);
			pObstaclesA->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);
			pObstaclesA->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObstaclesA->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
			pObstaclesA->FrustumCheck(false);
			pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesA->Transform()->SetLocalPos(Vec3(500.f -i*(500.f), 10.f, 21000.f));
			pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesA->MeshRender()->SetDynamicShadow(false);
			m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
		}

		//1층
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				pObstaclesA = pMeshData->Instantiate();
				pObstaclesA->AddComponent(new CTransform);
				pObstaclesA->AddComponent(new CCollider3D);
				pObstaclesA->AddComponent(new CObstacleScript);
				pObstaclesA->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);
				pObstaclesA->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
				pObstaclesA->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
				pObstaclesA->FrustumCheck(false);
				pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesA->Transform()->SetLocalPos(Vec3(-1120.f + 560.f * i, 10.f - FLOORHEIGET, 8400.f + 1600.f * j));
				pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->MeshRender()->SetDynamicShadow(false);
				m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
			}
		}
		for (int i = 0; i < 2; ++i)
		{
			pObstaclesA = pMeshData->Instantiate();
			pObstaclesA->AddComponent(new CTransform);
			pObstaclesA->AddComponent(new CCollider3D);
			pObstaclesA->AddComponent(new CObstacleScript);
			pObstaclesA->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);
			pObstaclesA->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObstaclesA->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
			pObstaclesA->FrustumCheck(false);
			pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesA->Transform()->SetLocalPos(Vec3(-2560.f + 480.f * i, 10.f - FLOORHEIGET, 14720.f));
			pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesA->MeshRender()->SetDynamicShadow(false);
			m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
		}
	}


	//시계추회전장애물 : 2층(14) + 1층(9)
	{
		pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle5_2.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10.mdat", L"MeshData\\Obstacle10.mdat");
		//pMeshData->Save(pMeshData->GetPath());
		CGameObject* pObstaclesB;
		pObstaclesB = nullptr;

		//2층
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				pObstaclesB = pMeshData->Instantiate();
				pObstaclesB->AddComponent(new CTransform);
				pObstaclesB->AddComponent(new CCollider3D);
				pObstaclesB->AddComponent(new CObstacleScript);
				pObstaclesB->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEB);
				pObstaclesB->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
				pObstaclesB->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
				pObstaclesB->FrustumCheck(false);
				pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesB->Transform()->SetLocalPos(Vec3(-840.f + 840.f*i, 330.f, 8400.f + 1200.f*j));
				pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesB->MeshRender()->SetDynamicShadow(false);
				m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
			}
		}
		for (int i = 0; i < 2; ++i)
		{
			pObstaclesB = pMeshData->Instantiate();
			pObstaclesB->AddComponent(new CTransform);
			pObstaclesB->AddComponent(new CCollider3D);
			pObstaclesB->AddComponent(new CObstacleScript);
			pObstaclesB->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEB);
			pObstaclesB->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObstaclesB->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
			pObstaclesB->FrustumCheck(false);
			pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesB->Transform()->SetLocalPos(Vec3(-1680.f + 1680.f*i, 330.f, 14800.f));
			pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesB->MeshRender()->SetDynamicShadow(false);
			m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
		}

		//1층
		for (int i = 0; i < 3; ++i)
		{
			pObstaclesB = pMeshData->Instantiate();
			pObstaclesB->AddComponent(new CTransform);
			pObstaclesB->AddComponent(new CCollider3D);
			pObstaclesB->AddComponent(new CObstacleScript);
			pObstaclesB->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEB);
			pObstaclesB->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObstaclesB->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
			pObstaclesB->FrustumCheck(false);
			pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesB->Transform()->SetLocalPos(Vec3(-640.f - 640.f * i, 330.f-FLOORHEIGET, 12800.f + 600.f*i));
			pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesB->MeshRender()->SetDynamicShadow(false);
			m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
		}
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				pObstaclesB = pMeshData->Instantiate();
				pObstaclesB->AddComponent(new CTransform);
				pObstaclesB->AddComponent(new CCollider3D);
				pObstaclesB->AddComponent(new CObstacleScript);
				pObstaclesB->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEB);
				pObstaclesB->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
				pObstaclesB->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
				pObstaclesB->FrustumCheck(false);
				pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesB->Transform()->SetLocalPos(Vec3(-480.f + 960.f * i, 330.f - FLOORHEIGET, 16800.f + 800.f * j));
				pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesB->MeshRender()->SetDynamicShadow(false);
				m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
			}
		}
	}

	//허들 장애물 : 2층(7) + 1층(34)
	{
		pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\huddle_2.fbx");
		//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10.mdat", L"MeshData\\Obstacle10.mdat");
		//pMeshData->Save(pMeshData->GetPath());
		CGameObject* pObstaclesH;
		pObstaclesH = nullptr;

		//2층
		for (int i = 0; i < 3; ++i)
		{
			pObstaclesH = pMeshData->Instantiate();
			pObstaclesH->AddComponent(new CTransform);
			pObstaclesH->AddComponent(new CCollider3D);
			pObstaclesH->AddComponent(new CObstacleScript);
			pObstaclesH->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::STOP);
			pObstaclesH->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObstaclesH->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesH->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
			pObstaclesH->FrustumCheck(false);
			pObstaclesH->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesH->Transform()->SetLocalPos(Vec3(-160.f + 160.f * i, 80.f, 21000.f));
			pObstaclesH->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesH->MeshRender()->SetDynamicShadow(false);
			m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH);
			
		}
		for (int i = 0; i < 2; ++i)
		{
			for (int j = -1; j <= 1; j+=2)
			{
				pObstaclesH = pMeshData->Instantiate();
				pObstaclesH->AddComponent(new CTransform);
				pObstaclesH->AddComponent(new CCollider3D);
				pObstaclesH->AddComponent(new CObstacleScript);
				pObstaclesH->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::STOP);
				pObstaclesH->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
				pObstaclesH->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesH->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
				pObstaclesH->FrustumCheck(false);
				pObstaclesH->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesH->Transform()->SetLocalPos(Vec3(j*(800.f + 160.f * i), 80.f, 21000.f));
				pObstaclesH->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesH->MeshRender()->SetDynamicShadow(false);
				m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH);
			}
		}

		//1층
		for (int i = 0; i < 17; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				pObstaclesH = pMeshData->Instantiate();
				pObstaclesH->AddComponent(new CTransform);
				pObstaclesH->AddComponent(new CCollider3D);
				pObstaclesH->AddComponent(new CObstacleScript);
				pObstaclesH->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::STOP);
				pObstaclesH->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
				pObstaclesH->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesH->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
				pObstaclesH->FrustumCheck(false);
				pObstaclesH->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesH->Transform()->SetLocalPos(Vec3(-1280.f + 160.f * i, 80.f-FLOORHEIGET, 9160.f +1680.f*j));
				pObstaclesH->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesH->MeshRender()->SetDynamicShadow(false);
				m_pCurScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH);
			}
		}
	}

	//맵 ====================================================
	//단위(unity추출기준) : 원본*400
	//2층
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part0.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part0.mdat", L"MeshData\\2part0.mdat");

	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);
	
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part1.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part1.mdat", L"MeshData\\2part1.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 1000.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part2.fbx");
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part2.mdat", L"MeshData\\2part2.mdat");
	//pMeshData->Save(pMeshData->GetPath());
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 3800.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\1jinggum.fbx");
	//pMeshData->Save(pMeshData->GetPath());

	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\1jinggum.mdat", L"MeshData\\1jinggum.mdat");
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part3.mdat", L"MeshData\\2part3.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 5000.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);

	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part4.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part4.mdat", L"MeshData\\2part4.mdat");

	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 8000.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);

	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part5.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part5.mdat", L"MeshData\\2part5.mdat");

	//pMeshData->Save(pMeshData->GetPath());
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 13000.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);

	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part6.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part6.mdat", L"MeshData\\2part6.mdat");

	//pMeshData->Save(pMeshData->GetPath());
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 16700.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part7.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part7.mdat", L"MeshData\\2part7.mdat");

	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 20800.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);

	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part8.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\2part8.mdat", L"MeshData\\2part8.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f, 21400.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);

	//1층 ==============================================================================================
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\1water.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\1water.mdat", L"MeshData\\1water.mdat");

	//pMeshData->Save(pMeshData->GetPath());
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f-FLOORHEIGET, 4760.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);
	//pObject->Animator3D()->SetClipIndex(1);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\1part1.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\1part1.mdat", L"MeshData\\1part1.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGET, 8200.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\1part2.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\1part2.mdat", L"MeshData\\1part2.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGET, 12600.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\1part3.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\1part3.mdat", L"MeshData\\1part3.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGET, 16800.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);


	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\1part1.mdat", L"MeshData\\1part1.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGET, 19100.f));
	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->MeshRender()->SetDynamicShadow(false);

	m_pCurScene->FindLayer(L"Racing")->AddGameObject(pObject);

	///////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region TempObj

	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\cafetest.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\cafetest.mdat", L"MeshData\\cafetest.mdat",false, true);

	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(300.f, 400.f, 100.f));
	//pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));

	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);

	//m_pCurScene->FindLayer(L"Tower")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\cafeganpan.fbx");
	////pMeshData->Save(pMeshData->GetPath());
	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(300.f, 200.f, 0.f));
	//pObject->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));

	//pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);

	//m_pCurScene->FindLayer(L"Tower")->AddGameObject(pObject);


	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\wizard_min.mdat", L"MeshData\\wizard_min.mdat");
	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(200.f, 100.f, 150.f));
	//pObject->Transform()->SetLocalScale(Vec3(0.25f, 0.25f, 0.25f));
	//pObject->MeshRender()->SetDynamicShadow(true);

	////auto p=pObject->MeshRender()->GetMesh()->GetAnimClip()->at(4);

	//m_pCurScene->FindLayer(L"Minion")->AddGameObject(pObject);



//	pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\FirstTower.fbx");
 //   pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\FirstTower.mdat", L"MeshData\\FirstTower.mdat");
	////pMeshData->Save(pMeshData->GetPath());
	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(150.f, 100.f, 200.f)); 
	//pObject->Transform()->SetLocalRot(Vec3(-3.14f/2, 0.f, 0.f));

	//pObject->Transform()->SetLocalScale(Vec3(7.f, 7.f, 7.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	////pObject->Animator3D()->SetClipIndex(1);


	//m_pCurScene->FindLayer(L"Tower")->AddGameObject(pObject);

//	pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\SecondTower.fbx");
////	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SecondTower.mdat", L"MeshData\\SecondTower.mdat");
//	pMeshData->Save(pMeshData->GetPath());
//	pObject = pMeshData->Instantiate();
//	pObject->AddComponent(new CTransform);
//	pObject->AddComponent(new CCollider3D);
//	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
//	pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
//	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
//	pObject->FrustumCheck(false);
//	pObject->Transform()->SetLocalPos(Vec3(150.f, 100.f, 400.f));
//	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
//	pObject->MeshRender()->SetDynamicShadow(true);
//


	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\SecondTower01.fbx");
//	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\SecondTower01.mdat", L"MeshData\\SecondTower01.mdat");
//	//pMeshData->Save(pMeshData->GetPath());
//	pObject = pMeshData->Instantiate();
//	pObject->AddComponent(new CTransform);
//	pObject->AddComponent(new CCollider3D);
//	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
//	pObject->Collider3D()->SetOffsetScale(Vec3(10.f, 40.f, 10.f));
//	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
//	pObject->FrustumCheck(false);
//	pObject->Transform()->SetLocalPos(Vec3(150.f, 100.f, -400.f));
//	pObject->Transform()->SetLocalRot(Vec3(0.f, 3.14f, 0.f));
//	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
//	pObject->MeshRender()->SetDynamicShadow(true);
//
//
//	m_pCurScene->FindLayer(L"Tower")->AddGameObject(pObject);
//


//	pObject = new CGameObject;
//	pObject->SetName(L"Terrain");
//	pObject->AddComponent(new CTransform);
//	pObject->AddComponent(new CMeshRender);
//	pObject->AddComponent(new CTerrain);
//
//	pObject->FrustumCheck(false);
//	pObject->Transform()->SetLocalPos(Vec3(0.f, 100.f, 0.f));
//	pObject->Transform()->SetLocalScale(Vec3(50.f, 400.f, 50.f));
//	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TerrainMtrl"));
//	pObject->Terrain()->Init();
//
//
//	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);
//

#pragma endregion


	pObject = new CGameObject;
	pObject->SetName(L"Particle");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CParticleSystem);
	pObject->ParticleSystem()->Init((CResMgr::GetInst()->FindRes<CTexture>(L"Snow")));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(500.f, 0.f, 0.f));

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);


	
	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pSky02.GetPointer());

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pObject);




	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
	CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"Monster");
	m_pCurScene->Awake();
	m_pCurScene->Start();
}

void CSceneMgr::Update()
{
	m_pCurScene->Update();
	m_pCurScene->LateUpdate();

	// rendermgr 카메라 초기화
	//CRenderMgr::GetInst()->ClearCamera();

	m_pCurScene->FinalUpdate();

	// 충돌 처리
	CCollisionMgr::GetInst()->Update();
}

void CSceneMgr::Update_Tool()
{
	// rendermgr 카메라 초기화
	CRenderMgr::GetInst()->ClearCamera();
	m_pCurScene->FinalUpdate();
}

void CSceneMgr::FindGameObjectByTag(const wstring& _strTag, vector<CGameObject*>& _vecFindObj)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObject = m_pCurScene->GetLayer(i)->GetObjects();
		for (size_t j = 0; j < vecObject.size(); ++j)
		{
			if (_strTag == vecObject[j]->GetName())
			{
				_vecFindObj.push_back(vecObject[j]);
			}
		}
	}
}


CGameObject* CSceneMgr::AddNetworkGameObject(bool isPlayer, Vec3 pos)
{
	std::cout << "add obj" << std::endl;
	// MeshRender 설정
	CGameObject* pPlayer = nullptr;
	pPlayer = new CGameObject;

	pPlayer->SetName((L"Player"));
	pPlayer->AddComponent(new CTransform);
	pPlayer->AddComponent(new CMeshRender);
	pPlayer->AddComponent(new CCollider3D);
	pPlayer->AddComponent(new CPlayerScript);
	pPlayer->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pPlayer->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pPlayer->Collider3D()->SetOffsetPos(Vec3(0.f, 50.f, 0.f));
	pPlayer->FrustumCheck(false);


	// Transform 설정

	// MeshRender 설정
	pPlayer->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pPlayer->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"));

#if LOCALPLAY
	pPlayer->Transform()->SetLocalPos(Vec3(0.f, 10.f, 0.f));
#else
	pPlayer->Transform()->SetLocalPos(pos);
#endif

	pPlayer->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	pPlayer->GetScript<CPlayerScript>()->SetPlayable(false);
	pPlayer->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	pPlayer->GetScript<CPlayerScript>()->SetState(PLAYER_STATE::IDLE);

	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	CGameObject* pObject = nullptr;

	pObject = new CGameObject;
	pObject = pMeshData->Instantiate();
	pObject->SetName(L"IdlePlayer");
	pObject->AddComponent(new CTransform);
	pObject->Transform()->SetLocalScale(Vec3(3, 3, 3));
	pObject->SetActive(true);
	pObject->MeshRender()->SetDynamicShadow(false);
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject, false);

	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetIdlePlayer(pObject);

	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);
	pObject = new CGameObject;
	pObject = pMeshData->Instantiate();
	pObject->SetName(L"RunPlayer");
	pObject->AddComponent(new CTransform);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(3, 3, 3));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));

	pObject->MeshRender()->SetDynamicShadow(false);
	m_pCurScene->FindLayer(L"Player")->AddGameObject(pObject, false);

	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetRunPlayer(pObject);

	if (isPlayer)
	{
		pPlayer->GetScript<CPlayerScript>()->SetPlayable(true);

		for (auto obj : m_pCurScene->FindLayer(L"Default")->GetParentObj())
		{
			if (obj->GetName().compare(L"MainCam") == 0)
			{
				obj->Transform()->SetLocalPos(Vec3(0, 70.f, 300.f));
				obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
				pObject->AddChild(obj);

				////pMainCam->Transform()->SetLocalPos(Vec3(-60,45,-10));
		//pMainCam->Transform()->SetLocalScale(Vec3(15000.f, 15000.f, 15000.f));
				break;
			}
		}
	}

	m_pCurScene->FindLayer(L"Player")->AddGameObject(pPlayer, false);

	return pPlayer;
}


bool Compare(CGameObject* _pLeft, CGameObject* _pRight)
{
	return (_pLeft->Transform()->GetWorldPos().z < _pRight->Transform()->GetWorldPos().z);
}

