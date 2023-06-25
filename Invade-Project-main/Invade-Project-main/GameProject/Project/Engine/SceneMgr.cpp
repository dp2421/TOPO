#include "pch.h"
#include "Tile.h"
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
#include "ItemScript.h"
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


void CSceneMgr::ChangeScene()
{
	SAFE_DELETE(m_pCurScene);
	//m_pCurScene = _pNextScene;
	m_pCurScene = m_pRacingScene;
	m_pSceneType = SCENE_TYPE::RACING;
}

CSceneMgr::CSceneMgr()
	: m_pCurScene(m_pRacingScene), m_pStartScene(nullptr), m_pSceneType(SCENE_TYPE::RACING)
{
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
}

void CSceneMgr::LoadMapInfoFromFile(const wstring& FileName, vector<Tile>& tiles)
{
	// 맵 타일 로드
	std::ifstream inFile(FileName, std::ios::in | std::ios::binary);

	if (!inFile) {
		//std::cerr << "Failed to open Map File: " << FileNames << std::endl;
		return;
	}

	while (!inFile.eof()) {
		TileInfo tile;
		inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
		tiles.push_back(tile);
	}
	tiles.pop_back(); //ㅋㅋ수동지우기
	inFile.close();
}

void CSceneMgr::LoadMetorMapInfoFromFile(const wstring& FileName, vector<Tile>& tiles)
{
	// 맵 타일 로드
	std::ifstream inFile(FileName, std::ios::in | std::ios::binary);

	if (!inFile) {
		//std::cerr << "Failed to open Map File: " << FileNames << std::endl;
		return;
	}

	while (!inFile.eof()) {
		MetorTile tile;
		inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
		tiles.push_back(tile);
	}
	tiles.pop_back(); //ㅋㅋ수동지우기
	inFile.close();
}

void CSceneMgr::InitMainScene()
{
	// 필요한 리소스 로딩
	// Texture 로드

	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"TestTex", L"Texture\\Health.png");
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>(L"Explosion", L"Texture\\Explosion\\Explosion80.png");
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>(L"Black", L"Texture\\asd.png");
	Ptr<CTexture> pSky01 = CResMgr::GetInst()->Load<CTexture>(L"Sky01", L"Texture\\Skybox\\Sky01.png");
	Ptr<CTexture> pSky02 = CResMgr::GetInst()->Load<CTexture>(L"FS003_Day", L"Texture\\Skybox\\FS003_Day.png");
	CResMgr::GetInst()->Load<CTexture>(L"Snow", L"Texture\\Particle\\Snow50px.png");
	CResMgr::GetInst()->Load<CTexture>(L"smokeparticle", L"Texture\\Particle\\smokeparticle.png");
	CResMgr::GetInst()->Load<CTexture>(L"HardCircle", L"Texture\\Particle\\HardCircle.png");
	CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
	Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);


	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");


	//Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);




	Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_3, pSky02.GetPointer());
	//
	pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_2, pSky02.GetPointer());
	//

	m_pRacingScene = new CScene;
	m_pRacingScene->SetName(L"Play Scene");

	m_pRacingScene->GetLayer(0)->SetName(L"Default");
	m_pRacingScene->GetLayer(1)->SetName(L"Player");
	m_pRacingScene->GetLayer(2)->SetName(L"Monster");
	m_pRacingScene->GetLayer(3)->SetName(L"Arrow");
	m_pRacingScene->GetLayer(4)->SetName(L"Minion");
	m_pRacingScene->GetLayer(5)->SetName(L"Tower");
	m_pRacingScene->GetLayer(6)->SetName(L"temp");
	m_pRacingScene->GetLayer(7)->SetName(L"Racing");
	m_pRacingScene->GetLayer(8)->SetName(L"Obstacle");
	//m_pCurScene->GetLayer(9)->SetName(L"ObstacleMove");
	m_pRacingScene->GetLayer(9)->SetName(L"UI");



	m_pRacingScene->GetLayer(31)->SetName(L"Tool");


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

	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pMainCam);

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
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject);


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
	m_pCurScene = m_pRacingScene;
	AddNetworkGameObject(true, Vec3::Zero, m_pRacingScene);
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
#pragma endregion

	///////////////////////////////////////////////////////////////
	////장애물테스트
	//360도
	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle10_2.fbx");
	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10_2.mdat", L"MeshData\\Obstacle10_2.mdat", false, true);
	//pMeshData->Save(pMeshData->GetPath());
	//CGameObject* pMoveObs = nullptr;
	//pMoveObs = new CGameObject;
	//
	//pMoveObs = pMeshData->Instantiate();
	//pMoveObs->AddComponent(new CTransform);
	//pMoveObs->AddComponent(new CCollider3D);
	//pMoveObs->AddComponent(new CObstacleScript);
	//pMoveObs->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);
	//
	//auto TempObj = new CGameObject;
	//TempObj->AddComponent(new CTransform);
	//TempObj->AddComponent(new CCollider3D);
	//TempObj->Transform()->SetLocalRot(Vec3(0.0f, 0.0f, 0.0f));
	//TempObj->Transform()->SetLocalPos(Vec3(0.0, 0.0f, 0.0f));
	//TempObj->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//TempObj->Collider3D()->SetOffsetScale(Vec3(90.f, 90.f, 230.f));
	//TempObj->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 115.f));
	//m_pCurScene->FindLayer(L"TestObstacle")->AddGameObject(TempObj);
	//
	//pMoveObs->AddChild(TempObj);
	//
	////pMoveObs->AddComponent(new CMeshRender);
	//pMoveObs->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pMoveObs->Collider3D()->SetOffsetScale(Vec3(450.f, 80.f, 65.f));
	//pMoveObs->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 152.f));
	//pMoveObs->FrustumCheck(false);
	//pMoveObs->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
	//pMoveObs->Transform()->SetLocalPos(Vec3(600.f, 10.f, 50.f));
	//
	//pMoveObs->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pMoveObs->MeshRender()->SetDynamicShadow(false);
	////pObject->Animator3D()->SetClipIndex(0);
	//
	//m_pCurScene->FindLayer(L"TestObstacle")->AddGameObject(pMoveObs);
	//
	////시계추회전
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle5_2.fbx");
	////pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10.mdat", L"MeshData\\Obstacle10.mdat");
	//pMeshData->Save(pMeshData->GetPath());
	//pMoveObs = pMeshData->Instantiate();
	//pMoveObs->AddComponent(new CTransform);
	//pMoveObs->AddComponent(new CCollider3D);
	//pMoveObs->AddComponent(new CObstacleScript);
	//pMoveObs->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEB);
	//
	////pMoveObs->AddComponent(new CMeshRender);
	//pMoveObs->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pMoveObs->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 300.f));
	//pMoveObs->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, -160.f));
	//pMoveObs->FrustumCheck(false);
	//pMoveObs->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
	//pMoveObs->Transform()->SetLocalPos(Vec3(1300.f, 330.f, 50.f));
	//
	//pMoveObs->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//pMoveObs->MeshRender()->SetDynamicShadow(false);
	////pObject->Animator3D()->SetClipIndex(0);
	//m_pCurScene->FindLayer(L"TestObstacle")->AddGameObject(pMoveObs);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//360도회전장애물 : 2층(26) + 1층(17)
	{	//2층
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle10_2.fbx");
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10_2.mdat", L"MeshData\\Obstacle10_2.mdat", false, true);
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
					pObstaclesA->Collider3D()->SetOffsetScale(Vec3(450.f, 80.f, 65.f));
					pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 152.f));
					pObstaclesA->FrustumCheck(false);
					pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
					pObstaclesA->Transform()->SetLocalPos(Vec3(k * (1280.f + 280.f * i + 640.f * j), 10.f, 1200.f + 400.f * i));
					pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					pObstaclesA->MeshRender()->SetDynamicShadow(false);
					m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
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
				pObstaclesA->Collider3D()->SetOffsetScale(Vec3(450.f, 80.f, 65.f));
				pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 152.f));
				pObstaclesA->FrustumCheck(false);
				pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesA->Transform()->SetLocalPos(Vec3(-1600.f + 600.f * i + 640.f, 10.f, 8960.f + 1280.f * j));
				pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->MeshRender()->SetDynamicShadow(false);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
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
			pObstaclesA->Collider3D()->SetOffsetScale(Vec3(450.f, 80.f, 65.f));
			pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 152.f));
			pObstaclesA->FrustumCheck(false);
			pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesA->Transform()->SetLocalPos(Vec3(500.f - i * (1000.f), 10.f, 21000.f));
			pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesA->MeshRender()->SetDynamicShadow(false);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
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
				pObstaclesA->Collider3D()->SetOffsetScale(Vec3(450.f, 80.f, 65.f));
				pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 152.f));
				pObstaclesA->FrustumCheck(false);
				pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesA->Transform()->SetLocalPos(Vec3(-1120.f + 560.f * i, 10.f - FLOORHEIGET, 8400.f + 1600.f * j));
				pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->MeshRender()->SetDynamicShadow(false);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
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
			pObstaclesA->Collider3D()->SetOffsetScale(Vec3(450.f, 80.f, 65.f));
			pObstaclesA->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, 152.f));
			pObstaclesA->FrustumCheck(false);
			pObstaclesA->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesA->Transform()->SetLocalPos(Vec3(-2560.f + 480.f * i, 10.f - FLOORHEIGET, 14720.f));
			pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesA->MeshRender()->SetDynamicShadow(false);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA);
		}
	}


	//시계추회전장애물 : 2층(14) + 1층(9)
	{
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle5_2.fbx");
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle5_2.mdat", L"MeshData\\Obstacle5_2.mdat", false, true);
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
				pObstaclesB->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 300.f));
				pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, -160.f));
				pObstaclesB->FrustumCheck(false);
				pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesB->Transform()->SetLocalPos(Vec3(-840.f + 840.f * i, 330.f, 8400.f + 1200.f * j));
				pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesB->MeshRender()->SetDynamicShadow(false);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
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
			pObstaclesB->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 300.f));
			pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, -160.f));
			pObstaclesB->FrustumCheck(false);
			pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesB->Transform()->SetLocalPos(Vec3(-1680.f + 3360.f * i, 330.f, 14800.f));
			pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesB->MeshRender()->SetDynamicShadow(false);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
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
			pObstaclesB->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 300.f));
			pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, -160.f));
			pObstaclesB->FrustumCheck(false);
			pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesB->Transform()->SetLocalPos(Vec3(-640.f - 640.f * i, 330.f - FLOORHEIGET, 12800.f + 600.f * i));
			pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesB->MeshRender()->SetDynamicShadow(false);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
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
				pObstaclesB->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 300.f));
				pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, -160.f));
				pObstaclesB->FrustumCheck(false);
				pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
				pObstaclesB->Transform()->SetLocalPos(Vec3(-480.f + 960.f * i, 330.f - FLOORHEIGET, 16800.f + 800.f * j));
				pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesB->MeshRender()->SetDynamicShadow(false);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB);
			}
		}
	}

	//허들 장애물 : 2층(7) + 1층(34)
	{
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\huddle_2.fbx");
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\huddle_2.mdat", L"MeshData\\huddle_2.mdat", false, true);
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
			pObstaclesH->Collider3D()->SetOffsetScale(Vec3(140.0, 20, 50));
			pObstaclesH->Collider3D()->SetOffsetPos(Vec3(0.f, 0.0f, 0.f));
			pObstaclesH->FrustumCheck(false);
			pObstaclesH->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesH->Transform()->SetLocalPos(Vec3(-160.f + 160.f * i, 80.f, 21000.f));
			pObstaclesH->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesH->MeshRender()->SetDynamicShadow(false);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH);

		}
		for (int i = 0; i < 2; ++i)
		{
			for (int j = -1; j <= 1; j += 2)
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
				pObstaclesH->Transform()->SetLocalPos(Vec3(j * (800.f + 160.f * i), 80.f, 21000.f));
				pObstaclesH->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesH->MeshRender()->SetDynamicShadow(false);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH);
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
				pObstaclesH->Transform()->SetLocalPos(Vec3(-1280.f + 160.f * i, 80.f - FLOORHEIGET, 9160.f + 1680.f * j));
				pObstaclesH->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesH->MeshRender()->SetDynamicShadow(false);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH);
			}
		}
	}

	//맵 ====================================================
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\2part0.fbx");
	//pMeshData->Save(pMeshData->GetPath());

	const wstring FileNames[] = { L"MapPos1F.bin", L"MapPos2F.bin", L"MapItem.bin" };
	for (int i = 0; i < 3; ++i)
	{
		LoadMapInfoFromFile(FileNames[i], tiles);

		for (auto& tile : tiles)
		{
			pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetPathName(), tile.GetPathName());
			pObject = pMeshData->Instantiate();
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CCollider3D);
			pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
			pObject->FrustumCheck(false);
			pObject->Transform()->SetLocalPos(tile.GetTilePos());
			pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObject->MeshRender()->SetDynamicShadow(false);
			//pObject->Animator3D()->SetClipIndex(1);
			m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject);

			if (i == 2)
			{
				int temp = tile.GetState();
				if (temp == LayerState::L1Sujum)
				{
					pObject->AddComponent(new CItemScript);
					pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::SUPERJUMP);
				}
				else if (temp == LayerState::LCoin)
				{
					pObject->AddComponent(new CItemScript);
					pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::COIN);
				}
			}

		}

	}

	//열외.. MapPos1FF는 파일에서 안 불러오고 따로 읽습니다.
	//이상하게..1FF.mdat 파일만 위의 방식으로 불러오면 텍스쳐가 누덕누덕거림..
	{
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\L1Water.mdat", L"MeshData\\L1Water.mdat");
		//pMeshData->Save(pMeshData->GetPath());
		pObject = pMeshData->Instantiate();
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CCollider3D);
		pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
		pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
		pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
		pObject->FrustumCheck(false);
		pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGET, 4760.f));
		pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
		pObject->Transform()->SetLocalScale(Vec3(4.f, 4.f, 1.f));
		pObject->MeshRender()->SetDynamicShadow(false);
		//pObject->Animator3D()->SetClipIndex(1);
		m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region TempObj

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

	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject);



	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pSky02.GetPointer());

	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject);


	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"Monster");

	//m_pRacingScene->Awake();
	//m_pRacingScene->Start();
}


void CSceneMgr::InitStartScene()
{
	// 필요한 리소스 로딩
	// Texture 로드

	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"TestTex", L"Texture\\Health.png");
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>(L"Explosion", L"Texture\\Explosion\\Explosion80.png");
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>(L"Black", L"Texture\\asd.png");
	Ptr<CTexture> pSky02 = CResMgr::GetInst()->Load<CTexture>(L"FS003_Day", L"Texture\\Skybox\\FS003_Day.png");
	CResMgr::GetInst()->Load<CTexture>(L"Snow", L"Texture\\Particle\\Snow50px.png");
	CResMgr::GetInst()->Load<CTexture>(L"smokeparticle", L"Texture\\Particle\\smokeparticle.png");
	CResMgr::GetInst()->Load<CTexture>(L"HardCircle", L"Texture\\Particle\\HardCircle.png");
	CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
	Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);


	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");


	//Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	if (m_pSceneType == SCENE_TYPE::LOBBY)
	{
		Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_3, pSky02.GetPointer());
		//
		pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_2, pSky02.GetPointer());

	}
	//

	m_pStartScene = new CScene;
	m_pStartScene->SetName(L"Start Scene");

	m_pStartScene->GetLayer(0)->SetName(L"Default");
	m_pStartScene->GetLayer(1)->SetName(L"Player");
	m_pStartScene->GetLayer(2)->SetName(L"Monster");
	m_pStartScene->GetLayer(3)->SetName(L"Arrow");
	m_pStartScene->GetLayer(4)->SetName(L"Minion");
	m_pStartScene->GetLayer(5)->SetName(L"Tower");
	m_pStartScene->GetLayer(6)->SetName(L"temp");
	m_pStartScene->GetLayer(7)->SetName(L"Racing");
	m_pStartScene->GetLayer(8)->SetName(L"Obstacle");
	m_pStartScene->GetLayer(9)->SetName(L"UI");



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

	m_pStartScene->FindLayer(L"Default")->AddGameObject(pMainCam);

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
	m_pStartScene->FindLayer(L"Default")->AddGameObject(pObject);



	//const wstring FileName = { L"MapPosition.bin" };

	//LoadMapInfoFromFile(FileName, tiles);

	//for (auto& tile : tiles)
	//{
	//	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetPathName(), tile.GetPathName());
	//	pObject = pMeshData->Instantiate();
	//	pObject->AddComponent(new CTransform);
	//	pObject->AddComponent(new CCollider3D);
	//	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	//	pObject->FrustumCheck(false);
	//	pObject->Transform()->SetLocalPos(tile.GetTilePos());
	//	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
	//	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	//	pObject->MeshRender()->SetDynamicShadow(false);
	//	//pObject->Animator3D()->SetClipIndex(1);
	//	m_pStartScene->FindLayer(L"Racing")->AddGameObject(pObject);

	//}


#if LOCALPLAY
	m_pCurScene = m_pStartScene;
	AddNetworkGameObject(true, Vec3::Zero, m_pStartScene);
#else
#endif

	if (m_pSceneType == SCENE_TYPE::RACING)
	{

		pObject = new CGameObject;
		pObject->SetName(L"SkyBox");
		pObject->FrustumCheck(false);
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
		pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pSky02.GetPointer());

		m_pStartScene->FindLayer(L"Default")->AddGameObject(pObject);
	}
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"Monster");
	//m_pStartScene->Awake();
	//m_pStartScene->Start();
}

void CSceneMgr::InitMetorScene()
{

	{
		// 필요한 리소스 로딩
		// Texture 로드
		Ptr<CTexture> pSky02 = CResMgr::GetInst()->Load<CTexture>(L"FS003_Day", L"Texture\\Skybox\\FS003_Day.png");
		CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
		Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
		Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);


		Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
		Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
		Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");


		//Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_3, pSky02.GetPointer());
		//
		pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_2, pSky02.GetPointer());


		//

		m_pMetorScene = new CScene;
		m_pMetorScene->SetName(L"Metor Scene");

		m_pMetorScene->GetLayer(0)->SetName(L"Default");
		m_pMetorScene->GetLayer(1)->SetName(L"Player");
		m_pMetorScene->GetLayer(2)->SetName(L"Monster");
		m_pMetorScene->GetLayer(3)->SetName(L"Arrow");
		m_pMetorScene->GetLayer(4)->SetName(L"Minion");
		m_pMetorScene->GetLayer(5)->SetName(L"Tower");
		m_pMetorScene->GetLayer(6)->SetName(L"temp");
		m_pMetorScene->GetLayer(7)->SetName(L"Racing");
		m_pMetorScene->GetLayer(8)->SetName(L"Obstacle");
		m_pMetorScene->GetLayer(9)->SetName(L"UI");



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

		m_pMetorScene->FindLayer(L"Default")->AddGameObject(pMainCam);

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
		m_pMetorScene->FindLayer(L"Default")->AddGameObject(pObject);



		//const wstring FileName = { L"MapPosition.bin" };

		//LoadMapInfoFromFile(FileName, tiles);

		//for (auto& tile : tiles)
		//{
		//	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetPathName(), tile.GetPathName());
		//	pObject = pMeshData->Instantiate();
		//	pObject->AddComponent(new CTransform);
		//	pObject->AddComponent(new CCollider3D);
		//	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
		//	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
		//	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
		//	pObject->FrustumCheck(false);
		//	pObject->Transform()->SetLocalPos(tile.GetTilePos());
		//	pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
		//	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		//	pObject->MeshRender()->SetDynamicShadow(false);
		//	//pObject->Animator3D()->SetClipIndex(1);
		//	m_pStartScene->FindLayer(L"Racing")->AddGameObject(pObject);

		//}


#if LOCALPLAY
		//m_pCurScene = m_pMetorScene;
		//AddNetworkGameObject(true, Vec3::Zero, m_pMetorScene);
#else
#endif
		
		//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetor1.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetor2.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		// pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetor3.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetor4.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetor5.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		const wstring FileNames[] = { L"MapPosition.bin"};
		for (int i = 0; i < 1; ++i)
		{
			tiles.clear();
			LoadMetorMapInfoFromFile(FileNames[i], tiles);

			for (auto& tile : tiles)
			{
				Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetMetorPathName(), tile.GetMetorPathName());
				pObject = pMeshData->Instantiate();
				pObject->AddComponent(new CTransform);
				pObject->AddComponent(new CCollider3D);
				pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
				pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
				pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
				pObject->FrustumCheck(false);
				pObject->Transform()->SetLocalPos(tile.GetTilePos());
				pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
				pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObject->MeshRender()->SetDynamicShadow(false);
				//pObject->Animator3D()->SetClipIndex(1);
				m_pMetorScene->FindLayer(L"Metor")->AddGameObject(pObject);

				//if (i == 2)
				//{
				//	int temp = tile.GetState();
				//	if (temp == LayerState::L1Sujum)
				//	{
				//		pObject->AddComponent(new CItemScript);
				//		pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::SUPERJUMP);
				//	}
				//	else if (temp == LayerState::LCoin)
				//	{
				//		pObject->AddComponent(new CItemScript);
				//		pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::COIN);
				//	}
				//}

			}

		}


		pObject = new CGameObject;
		pObject->SetName(L"SkyBox");
		pObject->FrustumCheck(false);
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
		pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pSky02.GetPointer());

		m_pMetorScene->FindLayer(L"Default")->AddGameObject(pObject);
		//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
		//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"Monster");
		//m_pStartScene->Awake();
		//m_pStartScene->Start();
	}
}

void CSceneMgr::InitUI()
{

	Ptr<CTexture> pStartButton = CResMgr::GetInst()->Load<CTexture>(L"UIButton", L"Texture\\startbutton.png");
	Ptr<CTexture> pCursor = CResMgr::GetInst()->Load<CTexture>(L"Cursor", L"Texture\\cursor.png");

	CGameObject* pUICam = nullptr;

	// Camera Object
	pUICam = new CGameObject;
	pUICam->SetName(L"UICam");
	pUICam->AddComponent(new CTransform);
	pUICam->AddComponent(new CCamera);
	//pUICam->AddComponent(new CCameraScript);

	pUICam->Camera()->SetProjType(PROJ_TYPE::ORTHGRAPHIC);
	pUICam->Camera()->SetFar(100000.f);
	pUICam->Camera()->SetLayerAllCheck();
	pUICam->Transform()->SetLocalPos(Vec3(0, 60.f * 10, 160.f * 7));
	pUICam->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pUICam);

	CGameObject* pObject = new CGameObject;

	pObject = new CGameObject;
	pObject->SetName(L"Cursor Object");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(-400.f, 400.f, 0.f));
	pObject->Transform()->SetLocalScale(Vec3(20.f, 20.f, 10.f));

	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));

	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pCursor.GetPointer());

	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));

	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject);


	pObject = new CGameObject;
	pObject->SetName(L"UI Object");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(-400.f, 400.f, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(200.f, 100.f, 10.f));

	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pStartButton.GetPointer());


	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));


	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject);

}


void CSceneMgr::Update()
{
	m_pCurScene->Update();
	m_pCurScene->LateUpdate();

	// rendermgr 카메라 초기화
	CRenderMgr::GetInst()->ClearCamera();

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

CGameObject* CSceneMgr::AddNetworkGameObject(bool isPlayer, Vec3 pos, CScene* curscene)
{
	Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);
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
	pPlayer->Collider3D()->SetOffsetScale(Vec3(100.0f, 160.0f, 100.0f));
	pPlayer->Collider3D()->SetOffsetPos(Vec3(0.f, 90.0f, 0.f));
	pPlayer->FrustumCheck(false);
	// Transform 설정

	// MeshRender 설정
	pPlayer->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pPlayer->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"));

#if LOCALPLAY
	pPlayer->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGET, 0.f));
	for (auto obj : curscene->FindLayer(L"Default")->GetParentObj())
	{
		if (obj->GetName().compare(L"MainCam") == 0)
		{
			obj->Transform()->SetLocalPos(Vec3(0, 60.f * 10, 160.f * 7));
			obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			pPlayer->AddChild(obj);
			//obj->Transform()->SetLocalPos(Vec3(-60,45,-10));
			//obj->Transform()->SetLocalScale(Vec3(15000.f, 15000.f, 15000.f));

			break;
		}
	}
#else
	pPlayer->Transform()->SetLocalPos(pos);
#endif

	pPlayer->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	pPlayer->GetScript<CPlayerScript>()->SetPlayable(false);
	pPlayer->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	pPlayer->GetScript<CPlayerScript>()->SetState(PLAYER_STATE::IDLE);
	CGameObject* pObject = nullptr;

	pObject = new CGameObject;
	pObject = idleData->Instantiate();
	pObject->SetName(L"IdlePlayer");
	pObject->AddComponent(new CTransform);
	pObject->Transform()->SetLocalScale(Vec3(7, 7, 7));
	pObject->SetActive(true);
	pObject->MeshRender()->SetDynamicShadow(false);
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	curscene->FindLayer(L"Player")->AddGameObject(pObject, false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pObject, false);


	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetIdlePlayer(pObject);

	pObject = new CGameObject;
	pObject = runMeshData->Instantiate();
	pObject->SetName(L"RunPlayer");
	pObject->AddComponent(new CTransform);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(7, 7, 7));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));

	pObject->MeshRender()->SetDynamicShadow(false);
	curscene->FindLayer(L"Player")->AddGameObject(pObject, false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pObject, false);

	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetRunPlayer(pObject);

	curscene->FindLayer(L"Player")->AddGameObject(pPlayer, false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pPlayer, false);

	return pPlayer;
}

void CSceneMgr::RemoveNetworkGameObject(CGameObject* obj)
{
	auto runPlayer = obj->GetScript<CPlayerScript>()->runPlayer;
	auto IdlePlayer = obj->GetScript<CPlayerScript>()->IdlePlayer;
	m_pCurScene->GetLayer(obj->GetLayerIdx())->RemoveParentObj(obj);
	CSceneMgr::GetInst()->GetCurScene()->GetLayer(runPlayer->GetLayerIdx())->RemoveParentObj(runPlayer);
	CSceneMgr::GetInst()->GetCurScene()->GetLayer(IdlePlayer->GetLayerIdx())->RemoveParentObj(IdlePlayer);
	runPlayer->SetActive(false);
	IdlePlayer->SetActive(false);
	obj->SetActive(false);
}


bool Compare(CGameObject* _pLeft, CGameObject* _pRight)
{
	return (_pLeft->Transform()->GetWorldPos().z < _pRight->Transform()->GetWorldPos().z);
}

