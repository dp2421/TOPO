#include "pch.h"
#include "Tile.h"
#include "SceneMgr.h"

#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "ResMgr.h"
#include "Shader.h"
#include "Mesh.h"
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
#include "Meteor.h"
#include "ItemScript.h"
#include "ParticleSystem.h"
#include "ArrowScript.h"
#include "AwardScript.h"
#include "UIScript.h"
#include "NumScript.h"
#include "GameFramework.h"
#include "NetworkMgr.h"

CScene* CSceneMgr::GetCurScene()
{
	return m_pCurScene;
}

void CSceneMgr::ChangeScene(CScene* _pNextScene)
{
	//SAFE_DELETE(m_pCurScene);
	m_pCurScene = _pNextScene;

}

void CSceneMgr::ChangeScene(SCENE_TYPE _type)
{
	//SAFE_DELETE(m_pCurScene);

	m_pSceneType = _type;

	if (_type == SCENE_TYPE::AWARD)
	{
		m_pCurScene = m_pAwardScene;
	}
	else if (_type == SCENE_TYPE::RACING)
	{
		m_pCurScene = m_pRacingScene;
		NetworkMgr::GetInst()->SendClientReadyPacket();
	}
	else if (_type == SCENE_TYPE::JUMP)
	{
		m_pCurScene = m_pJumpingScene;
		NetworkMgr::GetInst()->SendClientReadyPacket();
	}
	else if (_type == SCENE_TYPE::METOR)
	{
		m_pCurScene = m_pMetorScene;
		NetworkMgr::GetInst()->SendClientReadyPacket();
	}
	else if (_type == SCENE_TYPE::LOBBY)
	{
		m_pCurScene = m_pStartScene;
		//NetworkMgr::GetInst()->SendClientReadyPacket();
	}
}

void CSceneMgr::ChangeScene()
{
	SAFE_DELETE(m_pCurScene);
	//m_pCurScene = _pNextScene;
	m_pCurScene = m_pMetorScene;
	m_pSceneType = SCENE_TYPE::METOR;
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

	//while (!inFile.eof()) {
	MetorTile tile;
	inFile.read(reinterpret_cast<char*>(&tile), sizeof(tile));
	tiles.push_back(tile);
	//}
	//tiles.pop_back(); //ㅋㅋ수동지우기
	inFile.close();
}

void CSceneMgr::InitMainScene()
{
	// 필요한 리소스 로딩
	// Texture 로드

	Ptr<CTexture> pArrowTex = CResMgr::GetInst()->Load<CTexture>(L"Arrow", L"FBXTexture\\ArrowTex.png");
	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"TestTex", L"Texture\\Health.png");
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>(L"Explosion", L"Texture\\Explosion\\Explosion80.png");
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>(L"Black", L"Texture\\asd.png");
	m_pDaySkyBox = CResMgr::GetInst()->Load<CTexture>(L"FS003_Day", L"Texture\\Skybox\\FS003_Day.png");
	m_pNightSkyBox = CResMgr::GetInst()->Load<CTexture>(L"FS003_Night", L"Texture\\Skybox\\FS003_Night.png");
	CResMgr::GetInst()->Load<CTexture>(L"Snow", L"Texture\\Particle\\Snow50px.png");
	CResMgr::GetInst()->Load<CTexture>(L"smokeparticle", L"Texture\\Particle\\smokeparticle.png");
	CResMgr::GetInst()->Load<CTexture>(L"HardCircle", L"Texture\\Particle\\HardCircle.png");
	CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
	CResMgr::GetInst()->Load<CTexture>(L"CartoonSmoke", L"Texture\\Particle\\CartoonSmoke3.png");
	CResMgr::GetInst()->Load<CTexture>(L"CartoonSmokeF", L"Texture\\Particle\\CartoonSmokeF2.png");
	CResMgr::GetInst()->Load<CTexture>(L"LingParticle", L"Texture\\Particle\\LingParticle.png");


	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");
	Ptr<CTexture> pRoundOver = CResMgr::GetInst()->Load<CTexture>(L"RoundOver", L"Texture\\roundGameover2.png");
	Ptr<CTexture> pSpeedLine = CResMgr::GetInst()->Load<CTexture>(L"SpeedLine", L"Texture\\SpeedLine1.png");

	Vec2 winsize = CGameFramework::GetInst()->m_WinSize;



	Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_3, m_pDaySkyBox.GetPointer());
	//
	pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_2, m_pDaySkyBox.GetPointer());
	//

	m_pRacingScene = new CScene;
	m_pRacingScene->SetName(L"PlayScene");

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
	m_pRacingScene->GetLayer(10)->SetName(L"Coin");

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

	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pMainCam, m_pRacingScene);

	CGameObject* pObject = nullptr;

	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);
	pObject->Light3D()->SetLightPos(Vec3(0.f, 500.f, 0.f));
	pObject->Light3D()->SetLightType(LIGHT_TYPE::DIR);
	pObject->Light3D()->SetDiffuseColor(Vec3(1.f,1.f,1.f));
	pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pObject->Light3D()->SetAmbient(Vec3(0.4f, 0.4f, 0.4f));
	pObject->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
	pObject->Light3D()->SetLightRange(1000.f);
	pObject->Transform()->SetLocalPos(Vec3(-1000.f, 1000.f, -1000.f));
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);
	pObject->Light3D()->SetLightPos(Vec3(1530, -990, 18830));
	pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pObject->Light3D()->SetAmbient(Vec3(0.4f, 0.4f, 0.4f));
	pObject->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
	pObject->Light3D()->SetLightRange(450.f);
	pObject->Transform()->SetLocalPos(Vec3(1530, -990, 18830));
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CLight3D);
	pObject->Light3D()->SetLightPos(Vec3(-1660, -990, 18830));
	pObject->Light3D()->SetLightType(LIGHT_TYPE::POINT);
	pObject->Light3D()->SetDiffuseColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light3D()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pObject->Light3D()->SetAmbient(Vec3(1.f, 1.f, 1.f));
	pObject->Light3D()->SetLightDir(Vec3(1.f, -1.f, 1.f));
	pObject->Light3D()->SetLightRange(450.f);
	pObject->Transform()->SetLocalPos(Vec3(-1660, -990, 18830));
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);


	///////////////////////////////////////////////////////////////
	////장애물테스트
	//360도
	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle10_2.fbx");
	Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10_2.mdat", L"MeshData\\Obstacle10_2.mdat", false, true);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//360도회전장애물 : 2층(26) + 1층(17)
	{	//2층
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Obstacle10_2.fbx");
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Obstacle10_2.mdat", L"MeshData\\Obstacle10_2.mdat", false, true);
		//pMeshData->Save(pMeshData->GetPath());
		CGameObject* pObstaclesA;
		pObstaclesA = nullptr;

		for (int i = 0; i < 2; ++i) //3줄->2줄로 변경
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
					pObstaclesA->Transform()->SetLocalPos(Vec3(k * (1280.f + 560.f * i + 640.f * j), 10.f, 1200.f + 800.f * i));
					pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
					pObstaclesA->MeshRender()->SetDynamicShadow(true);
					m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA, m_pRacingScene);
				}
			}
		}
		for (int i = 0; i < 2; ++i) //4개 -> 2개로 변경
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
				pObstaclesA->Transform()->SetLocalPos(Vec3(-1040.f + 900.f * i + 640.f, 10.f, 8960.f + 1280.f * j));
				pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->MeshRender()->SetDynamicShadow(true);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA, m_pRacingScene);
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
			pObstaclesA->MeshRender()->SetDynamicShadow(true);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA, m_pRacingScene);
		}

		//1층
		for (int i = 0; i < 3; ++i) //5개 -> 3개로 변경
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
				pObstaclesA->Transform()->SetLocalPos(Vec3(-1120.f + 1120. * i, 10.f - FLOORHEIGHT, 8400.f + 1600.f * j));
				pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesA->MeshRender()->SetDynamicShadow(true);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA, m_pRacingScene);
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
			pObstaclesA->Transform()->SetLocalPos(Vec3(-2560.f + 480.f * i, 10.f - FLOORHEIGHT, 14720.f));
			pObstaclesA->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesA->MeshRender()->SetDynamicShadow(true);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesA, m_pRacingScene);
		}
	}



	//시계추회전장애물 : 2층(14) + 1층(9)
	{
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
				pObstaclesB->MeshRender()->SetDynamicShadow(true);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB, m_pRacingScene);
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
			pObstaclesB->MeshRender()->SetDynamicShadow(true);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB, m_pRacingScene);
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
			pObstaclesB->Transform()->SetLocalPos(Vec3(-640.f - 640.f * i, 330.f - FLOORHEIGHT, 12800.f + 600.f * i));
			pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObstaclesB->MeshRender()->SetDynamicShadow(true);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB, m_pRacingScene);
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
				pObstaclesB->Transform()->SetLocalPos(Vec3(-480.f + 960.f * i, 330.f - FLOORHEIGHT, 16800.f + 800.f * j));
				pObstaclesB->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesB->MeshRender()->SetDynamicShadow(true);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB, m_pRacingScene);
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
			pObstaclesH->MeshRender()->SetDynamicShadow(true);
			m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH, m_pRacingScene);

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
				pObstaclesH->MeshRender()->SetDynamicShadow(true);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH, m_pRacingScene);
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
				pObstaclesH->Transform()->SetLocalPos(Vec3(-1280.f + 160.f * i, 80.f - FLOORHEIGHT, 9160.f + 1680.f * j));
				pObstaclesH->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
				pObstaclesH->MeshRender()->SetDynamicShadow(true);
				m_pRacingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesH, m_pRacingScene);
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
		int coincount = -1;
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
			pObject->MeshRender()->SetDynamicShadow(true);
			//pObject->Animator3D()->SetClipIndex(1);
			if (i == 2)
			{
				int temp = tile.GetState();
				if (temp == LayerState::L1Sujum)
				{
					pObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));
					pObject->Transform()->SetLocalScale(Vec3(1.5f, 0.8f, 1.5f));
					pObject->AddComponent(new CItemScript);
					pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::SUPERJUMP);
					m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);
				}
				else if (temp == LayerState::LCoin)
				{
					coincount++;
					pObject->AddComponent(new CItemScript);
					pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::COIN);
					pObject->GetScript<CItemScript>()->CoinIdx(coincount);
					m_pRacingScene->FindLayer(L"Coin")->AddGameObject(pObject, m_pRacingScene);

				}
			}
			else
				m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);

		}

	}
	//열외.. MapPos1FF는 파일에서 안 불러오고 따로 읽음
	//이상하게..1FF.mdat 파일만 위의 방식으로 불러오면 텍스쳐가 누덕거림..
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
		pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGHT, 4760.f));
		pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
		pObject->Transform()->SetLocalScale(Vec3(4.f, 4.f, 1.f));
		pObject->MeshRender()->SetDynamicShadow(true);
		//pObject->Animator3D()->SetClipIndex(1);
		m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);
	}
	
	//골인 간판
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Goal2.fbx");
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Goal2.mdat", L"MeshData\\Goal2.mdat");
	pMeshData->Save(pMeshData->GetPath());
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f + 800.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(-400.f, 810.f, 22200.f));
	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 3.14f / 2));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"GoalMtrl"));
	m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);

	//골인깃발
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\greenFlag.fbx");
	//pMeshData->Save(pMeshData->GetPath());
	pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\greenFlag.mdat", L"MeshData\\greenFlag.mdat");
	pObject = pMeshData->Instantiate();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCollider3D);
	pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f + 800.f, 0.f));
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(-2600.f, -2790.f, 22200.f));
	pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pObject->Transform()->SetLocalRot(Vec3(0.f, 3.14f/2, 0.f));
	m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);


	//Arrow Sign (충돌체크 안할듯)
	for (int i = 0; i < 2; ++i)
	{
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Arrow.fbx");
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Arrow.mdat", L"MeshData\\Arrow.mdat");
		//pMeshData->Save(pMeshData->GetPath());
		pObject = pMeshData->Instantiate();
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CCollider3D);
		pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
		pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
		pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f + 800.f, 0.f));
		pObject->FrustumCheck(false);
		pObject->Transform()->SetLocalPos(Vec3(-1500.f + 3200.f * i, 10.f - FLOORHEIGHT, 18800.f));
		pObject->Transform()->SetLocalScale(Vec3(0.53f, 1.f, 0.65f));
		pObject->Transform()->SetLocalRot(Vec3(0.f, 3.14f, 3.14f / 4)); //위로올라가란표시
		//pObject->MeshRender()->SetDynamicShadow(true);
		//pObject->Animator3D()->SetClipIndex(1);
		m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);
	}

	//rain warf (이거 오류인가요? 아니요 기획의도인데요?)
	{
		//첫번째 슈점위치
		pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\effectwarf.mdat", L"MeshData\\effectwarf.mdat");
		pObject = pMeshData->Instantiate();
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CCollider3D);
		pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
		pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
		pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f + 800.f, 0.f));
		pObject->FrustumCheck(false);
		//pObject->Transform()->SetLocalPos(Vec3(-200.f, (10.f - FLOORHEIGHT)/2, 17600.f)); //테스트용
		pObject->Transform()->SetLocalPos(Vec3(0.f, (190.f - FLOORHEIGHT) / 2, 17600.f)); //실제수치
		pObject->Transform()->SetLocalScale(Vec3(3.f, 8.f, 3.f));
		pObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));

		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"Mesh\\effectwarf_0.mesh"));
		pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"BloomMtrl"));
		pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pArrowTex.GetPointer());

		m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);


		//끝에 슈점 두개 위치
		for (int i = 0; i < 2; ++i)
		{
			pObject = pMeshData->Instantiate();
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CCollider3D);
			pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f + 800.f, 0.f));
			pObject->FrustumCheck(false);
			pObject->Transform()->SetLocalPos(Vec3(-700.f + 1400.f*i, (190.f - FLOORHEIGHT) / 2, 20400.f)); //실제수치
			pObject->Transform()->SetLocalScale(Vec3(3.f, 8.f, 3.f));
			pObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));

			pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"Mesh\\effectwarf_0.mesh"));
			pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"BloomMtrl"));
			pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, pArrowTex.GetPointer());

			m_pRacingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pRacingScene);

		}
		
	}


	//파티클
	
	pObject = new CGameObject;
	pObject->SetName(L"Particle");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CParticleSystem);
	pObject->Transform()->SetLocalPos(Vec3(40000.f, 10.f, 0.f));
	pObject->ParticleSystem()->Init((CResMgr::GetInst()->FindRes<CTexture>(L"LingParticle")));
	pObject->ParticleSystem()->SetStartColor(Vec4(1.f, 0.7f, 0, 1.f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	pObject->ParticleSystem()->SetEndColor(Vec4(1.f, 1.f, 0.7f, 1.0f));
	pObject->ParticleSystem()->SetMaxLifeTime(3.0f);
	pObject->ParticleSystem()->SetMinLifeTime(0.5f);
	pObject->ParticleSystem()->SetStartScale(10.f);
	pObject->ParticleSystem()->SetEndScale(13.f);
	pObject->ParticleSystem()->SetFrequency(0.2f);
	pObject->ParticleSystem()->SetMinSpeed(20.0f);
	pObject->ParticleSystem()->SetMaxSpeed(40.0f);
	pObject->FrustumCheck(false);
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	//pObject->SetName(L"SuJumParticle");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CParticleSystem);
	pObject->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"Snow"));
	pObject->ParticleSystem()->SetStartColor(Vec4(0.8f, 1.0f, 0.5f, 1.f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	pObject->ParticleSystem()->SetEndColor(Vec4(0.8f, 1.f, 1.0f, 1.0f));
	pObject->ParticleSystem()->SetMaxLifeTime(10.0f);
	pObject->ParticleSystem()->SetMinLifeTime(1.0f);
	pObject->ParticleSystem()->SetStartScale(10.f);
	pObject->ParticleSystem()->SetEndScale(6.f);
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(-700.f, 10.f + 100.f - FLOORHEIGHT, 20600.f));
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	//pObject->SetName(L"SuJumParticle");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CParticleSystem);
	pObject->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"Snow"));
	pObject->ParticleSystem()->SetStartColor(Vec4(0.8f, 1.0f, 0.5f, 1.f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	pObject->ParticleSystem()->SetEndColor(Vec4(0.8f, 1.f, 1.0f, 1.0f));
	pObject->ParticleSystem()->SetMaxLifeTime(10.0f);
	pObject->ParticleSystem()->SetMinLifeTime(1.0f);
	pObject->ParticleSystem()->SetStartScale(10.f);
	pObject->ParticleSystem()->SetEndScale(6.f);
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(700.f, 10.f + 100.f - FLOORHEIGHT, 20600.f));
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	//pObject->SetName(L"SuJumParticle");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CParticleSystem);
	pObject->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"Snow"));
	pObject->ParticleSystem()->SetStartColor(Vec4(0.8f, 1.0f, 0.5f, 1.f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	pObject->ParticleSystem()->SetEndColor(Vec4(0.8f, 1.f, 1.0f, 1.0f));
	pObject->ParticleSystem()->SetMaxLifeTime(10.0f);
	pObject->ParticleSystem()->SetMinLifeTime(1.0f);
	pObject->ParticleSystem()->SetStartScale(10.f);
	pObject->ParticleSystem()->SetEndScale(6.f);
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(0.f, 100.f - FLOORHEIGHT, 17800.f));
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	pObject->SetName(L"CartoonParticle");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CParticleSystem);
	pObject->ParticleSystem()->Init((CResMgr::GetInst()->FindRes<CTexture>(L"CartoonSmoke")));
	pObject->ParticleSystem()->SetStartColor(Vec4(1.f, 1.f, 1.f, 1.0f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	pObject->ParticleSystem()->SetEndColor(Vec4(1.f, 1.f, 1.f, 0.2f));
	pObject->ParticleSystem()->SetMaxLifeTime(0.8f);
	pObject->ParticleSystem()->SetMinLifeTime(0.5f);
	pObject->ParticleSystem()->SetStartScale(35.f);
	pObject->ParticleSystem()->SetEndScale(22.f);
	pObject->ParticleSystem()->SetFrequency(0.8f);
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(40000.f, 10.f, 0.f));
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);


	pObject = new CGameObject;
	pObject->SetName(L"CartoonParticleF");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CParticleSystem);
	pObject->ParticleSystem()->Init((CResMgr::GetInst()->FindRes<CTexture>(L"CartoonSmokeF")));
	pObject->ParticleSystem()->SetStartColor(Vec4(1.f, 1.f, 1.f, 0.8f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	pObject->ParticleSystem()->SetEndColor(Vec4(1.f, 1.f, 1.f, 0.4f));
	pObject->ParticleSystem()->SetMaxLifeTime(0.8f);
	pObject->ParticleSystem()->SetMinLifeTime(0.2f);
	pObject->ParticleSystem()->SetStartScale(55.f);
	pObject->ParticleSystem()->SetEndScale(40.f);
	pObject->ParticleSystem()->SetFrequency(0.8f);
	pObject->ParticleSystem()->SetMinSpeed(50.f);
	pObject->ParticleSystem()->SetMaxSpeed(100.f);
	pObject->FrustumCheck(false);
	pObject->Transform()->SetLocalPos(Vec3(40000.f, 0.f, 0.f));
	pObject->SetActive(true);
	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);
	

	


	///////////////////////////////////////////////////////////////////////////////////////////////////////


#if LOCALPLAY
	//m_pCurScene = m_pRacingScene;
	AddNetworkGameObject(true, Vec3::Zero, m_pRacingScene);
#else
#endif
	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, m_pDaySkyBox.GetPointer());

	m_pRacingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pRacingScene);


	//1라운드게임오버시 출력할 이미지 레이싱씬에 추가
	pObject = new CGameObject;
	pObject->SetName(L"RoundOver");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::ROUNDOVER);
	// Transform
	pObject->Transform()->SetLocalPos(Vec3(0, 600.f, 20.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x / 2, 1.f));
	// MeshRender
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pRoundOver.GetPointer());
	// Collider2D
	//pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	//pObject->Collider2D()->SetOffsetPos(Vec3(0.f, 0.f, 0.f));
	pObject->SetActive(false);
	m_pRacingScene->FindLayer(L"UI")->AddGameObject(pObject, m_pRacingScene);


	//피버모드시 나올 스피드라인 생성
	pObject = new CGameObject;
	pObject->SetName(L"SpeedLine");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::SPEEDLINE);
	// Transform
	pObject->Transform()->SetLocalPos(Vec3(0, 600.f, 2.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.y, winsize.x, 1.f));
	// MeshRender
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pSpeedLine.GetPointer());
	pObject->SetActive(false);
	m_pRacingScene->FindLayer(L"UI")->AddGameObject(pObject, m_pRacingScene);

	//m_pCurScene = m_pRacingScene;
	//m_pRacingScene->Awake();

}


void CSceneMgr::InitStartScene()
{
	// 필요한 리소스 로딩
	// Texture 로드

	Ptr<CTexture> pTex = CResMgr::GetInst()->Load<CTexture>(L"TestTex", L"Texture\\Health.png");
	Ptr<CTexture> pExplosionTex = CResMgr::GetInst()->Load<CTexture>(L"Explosion", L"Texture\\Explosion\\Explosion80.png");
	Ptr<CTexture> pBlackTex = CResMgr::GetInst()->Load<CTexture>(L"Black", L"Texture\\asd.png");
	CResMgr::GetInst()->Load<CTexture>(L"Snow", L"Texture\\Particle\\Snow50px.png");
	CResMgr::GetInst()->Load<CTexture>(L"smokeparticle", L"Texture\\Particle\\smokeparticle.png");
	CResMgr::GetInst()->Load<CTexture>(L"HardCircle", L"Texture\\Particle\\HardCircle.png");
	CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
	Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);
	Ptr<CSound> pLobbySound = CResMgr::GetInst()->Load<CSound>(L"StartBgm", L"Sound\\LobbyBgm.wav");


	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");


	if (m_pSceneType == SCENE_TYPE::LOBBY)
	{
		Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_3, m_pDaySkyBox.GetPointer());
		//
		pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_2, m_pDaySkyBox.GetPointer());

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

	//m_pStartScene->GetLayer(5)->SetName(L"Cursor");
	//m_pStartScene->GetLayer(6)->SetName(L"Select");
	//m_pStartScene->GetLayer(7)->SetName(L"Matcing");
	//m_pStartScene->GetLayer(8)->SetName(L"Numbers");
	//m_pStartScene->GetLayer(9)->SetName(L"Mode");

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

	m_pStartScene->FindLayer(L"Default")->AddGameObject(pMainCam, m_pStartScene);

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
	m_pStartScene->FindLayer(L"Default")->AddGameObject(pObject, m_pStartScene);


	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Lobby.fbx");
	//pMeshData->Save(pMeshData->GetPath());

	//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\LobbyMap.mdat", L"MeshData\\LobbyMap.mdat", false, true);
	//pMeshData->Save(pMeshData->GetPath());
	//pObject = pMeshData->Instantiate();
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CCollider3D);
	//pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
	//pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
	//pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
	//pObject->FrustumCheck(false);
	//pObject->Transform()->SetLocalPos(Vec3(0, 10, 21400));
	//pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
	//pObject->Transform()->SetLocalScale(Vec3(3.f, 3.f, 3.f));
	//pObject->MeshRender()->SetDynamicShadow(true);
	//m_pStartScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pStartScene);

#if LOCALPLAY
	m_pCurScene = m_pStartScene;
	AddNetworkGameObject(true, Vec3::Zero, m_pStartScene);
#else
#endif

	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, m_pDaySkyBox.GetPointer());
	pObject->SetActive(false);

	m_pStartScene->FindLayer(L"Default")->AddGameObject(pObject, m_pStartScene);
}

void CSceneMgr::InitMetorScene()
{

	{
		// 필요한 리소스 로딩
		// Texture 로드
		CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
		Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
		Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);


		Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
		Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
		Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");


		Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_3, m_pDaySkyBox.GetPointer());
		//
		pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
		pPM->SetData(SHADER_PARAM::TEX_2, m_pDaySkyBox.GetPointer());


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

		m_pMetorScene->FindLayer(L"Default")->AddGameObject(pMainCam, m_pMetorScene);

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
		m_pMetorScene->FindLayer(L"Default")->AddGameObject(pObject, m_pMetorScene);



		//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\Meteor.fbx");
		//pMeshData->Save(pMeshData->GetPath());
		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Meteor.mdat", L"MeshData\\Meteor.mdat");
		//pMeshData->Save(pMeshData->GetPath());
		pObject = pMeshData->Instantiate();
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CCollider3D);
		pObject->AddComponent(new MeteorScript);
		pObject->GetScript<MeteorScript>()->SetType(MAP_TYPE::METEOR);
		pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
		pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
		pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
		pObject->FrustumCheck(false);
		pObject->Transform()->SetLocalPos(Vec3(950.f, 600.f, 100.f));
		pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
		pObject->Transform()->SetLocalScale(Vec3(60.f, 60.f, 60.f));
		pObject->MeshRender()->SetDynamicShadow(true);
		m_pMetorScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pMetorScene);

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorCenter.fbx");
		//pMeshData->Save(pMeshData->GetPath());
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorStone.fbx");
		//pMeshData->Save(pMeshData->GetPath());
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorWater.fbx");
		//pMeshData->Save(pMeshData->GetPath());
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorGrass.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		const wstring FileName[] = { L"LMetorCenter.bin",L"LMetorWood.bin",L"LMetorWater.bin",L"LMetorStone.bin",L"LMetorGrass.bin" };
		tiles.clear();
		for (int i = 0; i < 5; ++i)
		{
			LoadMetorMapInfoFromFile(FileName[i], tiles);
		}
		//tiles.pop_back(); // ㅋㅋ 수동지우기
		for (auto& tile : tiles)
		{
			Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetMetorPathName(), tile.GetMetorPathName());
			pObject = pMeshData->Instantiate();
			pObject->AddComponent(new CTransform);
			pObject->AddComponent(new CCollider3D);
			pObject->AddComponent(new MeteorScript);
			pObject->GetScript<MeteorScript>()->SetType(MAP_TYPE::GROUND);
			pObject->GetScript<MeteorScript>()->SetMapType((GROUND_TYPE)(tile.GetMState()-6));

			pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
			pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
			pObject->FrustumCheck(false);
			pObject->Transform()->SetLocalPos(tile.GetMetorTilePos());
			pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			pObject->MeshRender()->SetDynamicShadow(true);
			//pObject->Animator3D()->SetClipIndex(1);
			m_pMetorScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pMetorScene);

		}


#if LOCALPLAY
		//m_pCurScene = m_pMetorScene;
		AddNetworkGameObject(true, Vec3::Zero, m_pMetorScene);
#else
#endif

		//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorWater.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorStone.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		// pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorWood.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorCenter.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		//pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\LMetorGrass.fbx");
		//pMeshData->Save(pMeshData->GetPath());

		//const wstring FileNames[] = { L"MapPosition.bin"};
		//for (int i = 0; i < 1; ++i)
		//{
		//	tiles.clear();
		//	LoadMetorMapInfoFromFile(FileNames[i], tiles);

		//	for (auto& tile : tiles)
		//	{
		//		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetMetorPathName(), tile.GetMetorPathName());
		//		pObject = pMeshData->Instantiate();
		//		pObject->AddComponent(new CTransform);
		//		pObject->AddComponent(new CCollider3D);
		//		pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
		//		pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
		//		pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
		//		pObject->FrustumCheck(false);
		//		pObject->Transform()->SetLocalPos(tile.GetTilePos());
		//		pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
		//		pObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		//		pObject->MeshRender()->SetDynamicShadow(false);
		//		//pObject->Animator3D()->SetClipIndex(1);
		//		m_pMetorScene->FindLayer(L"Metor")->AddGameObject(pObject);

		//		//if (i == 2)
		//		//{
		//		//	int temp = tile.GetState();
		//		//	if (temp == LayerState::L1Sujum)
		//		//	{
		//		//		pObject->AddComponent(new CItemScript);
		//		//		pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::SUPERJUMP);
		//		//	}
		//		//	else if (temp == LayerState::LCoin)
		//		//	{
		//		//		pObject->AddComponent(new CItemScript);
		//		//		pObject->GetScript<CItemScript>()->SetState(ITEM_STATE::COIN);
		//		//	}
		//		//}

		//	}

		//}


		pObject = new CGameObject;
		pObject->SetName(L"SkyBox");
		pObject->FrustumCheck(false);
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);

		pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
		pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
		pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, m_pDaySkyBox.GetPointer());

		m_pMetorScene->FindLayer(L"Default")->AddGameObject(pObject, m_pMetorScene);
		//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
		//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"Monster");
		//m_pStartScene->Awake();
		//m_pStartScene->Start();
	}
}

void CSceneMgr::InitJumpingScene()
{
	// 필요한 리소스 로딩
// Texture 로드
	CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
	Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);


	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");


	//Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_3, m_pDaySkyBox.GetPointer());
	//
	pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_2, m_pDaySkyBox.GetPointer());


	//

	m_pJumpingScene = new CScene;
	m_pJumpingScene->SetName(L"Jumping Scene");

	m_pJumpingScene->GetLayer(0)->SetName(L"Default");
	m_pJumpingScene->GetLayer(1)->SetName(L"Player");
	m_pJumpingScene->GetLayer(2)->SetName(L"Monster");
	m_pJumpingScene->GetLayer(3)->SetName(L"Arrow");
	m_pJumpingScene->GetLayer(4)->SetName(L"Minion");
	m_pJumpingScene->GetLayer(5)->SetName(L"Tower");
	m_pJumpingScene->GetLayer(6)->SetName(L"temp");
	m_pJumpingScene->GetLayer(7)->SetName(L"Racing");
	m_pJumpingScene->GetLayer(8)->SetName(L"Obstacle");
	m_pJumpingScene->GetLayer(9)->SetName(L"UI");



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

	m_pJumpingScene->FindLayer(L"Default")->AddGameObject(pMainCam, m_pJumpingScene);

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
	m_pJumpingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pJumpingScene);



	const wstring FileName = { L"JumpMapCL.bin" };

	tiles.clear();
	LoadMapInfoFromFile(FileName, tiles);

	for (auto& tile : tiles)
	{
		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetPathName(), tile.GetPathName());
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
		pObject->MeshRender()->SetDynamicShadow(true);
		//pObject->Animator3D()->SetClipIndex(1);
		m_pJumpingScene->FindLayer(L"Racing")->AddGameObject(pObject, m_pJumpingScene);
	}


#if LOCALPLAY
	//m_pCurScene = m_pJumpingScene;
	AddNetworkGameObject(true, Vec3::Zero, m_pJumpingScene);
#else
#endif

	{
		//Ptr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\JumpObstacle.fbx");
		//pMeshData->Save(pMeshData->GetPath());
		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\JumpObstacle.mdat", L"MeshData\\JumpObstacle.mdat", false, true);
		CGameObject* pObstaclesB;
		pObstaclesB = nullptr;

		//2층
		for (int i = 0; i < 1; ++i)
		{

			pObstaclesB = pMeshData->Instantiate();
			pObstaclesB->AddComponent(new CTransform);
			pObstaclesB->AddComponent(new CCollider3D);
			pObstaclesB->AddComponent(new CObstacleScript);
			pObstaclesB->GetScript<CObstacleScript>()->SetState(OBSTACLE_STATE::MOVEA);
			pObstaclesB->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
			pObstaclesB->Collider3D()->SetOffsetScale(Vec3(100.f, 100.f, 300.f));
			pObstaclesB->Collider3D()->SetOffsetPos(Vec3(0.f, 0.f, -160.f));
			pObstaclesB->FrustumCheck(false);
			pObstaclesB->Transform()->SetLocalRot(Vec3(-3.14f / 2, 0.f, 0.f));
			pObstaclesB->Transform()->SetLocalPos(Vec3(0, 0, 0));
			pObstaclesB->Transform()->SetLocalScale(Vec3(2.5f, 2.5f, 1.f)); //레이싱기준 (Vec3(5.f, 5.f, 1.f)); scale크기
			pObstaclesB->MeshRender()->SetDynamicShadow(true);
			m_pJumpingScene->FindLayer(L"Obstacle")->AddGameObject(pObstaclesB, m_pJumpingScene);
		}
	}
	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, m_pDaySkyBox.GetPointer());

	m_pJumpingScene->FindLayer(L"Default")->AddGameObject(pObject, m_pJumpingScene);
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Player", L"Monster");
	//CCollisionMgr::GetInst()->CheckCollisionLayer(L"Arrow", L"Monster");
	m_pJumpingScene->Awake();
	m_pJumpingScene->Start();
}


void CSceneMgr::InitAwardScene()
{
	// 필요한 리소스 로딩
	// Texture 로드
	CResMgr::GetInst()->Load<CTexture>(L"particle_00", L"Texture\\Particle\\particle_00.png");
	Ptr<CMeshData> idleData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Run.mdat", L"MeshData\\Player_Run.mdat", false, true);

	Ptr<CTexture> pDiffuseTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	Ptr<CTexture> pNormalTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	Ptr<CTexture> pPositionTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");

	//Ptr<CTexture> pTestUAVTexture = CResMgr::GetInst()->CreateTexture(L"UAVTexture", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	Ptr<CMaterial> pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_3, m_pDaySkyBox.GetPointer());
	//
	pPM = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pPM->SetData(SHADER_PARAM::TEX_2, m_pDaySkyBox.GetPointer());
	//
	m_pAwardScene = new CScene;
	m_pAwardScene->SetName(L"AwardScene");

	m_pAwardScene->GetLayer(0)->SetName(L"Default");
	m_pAwardScene->GetLayer(1)->SetName(L"Player");
	m_pAwardScene->GetLayer(2)->SetName(L"Monster");
	m_pAwardScene->GetLayer(3)->SetName(L"Arrow");
	m_pAwardScene->GetLayer(4)->SetName(L"Minion");
	m_pAwardScene->GetLayer(5)->SetName(L"Tower");
	m_pAwardScene->GetLayer(6)->SetName(L"temp");
	m_pAwardScene->GetLayer(7)->SetName(L"Racing");
	m_pAwardScene->GetLayer(8)->SetName(L"Obstacle");
	m_pAwardScene->GetLayer(9)->SetName(L"UI");
	m_pAwardScene->GetLayer(10)->SetName(L"Award");


	CGameObject* pMainCam = nullptr;

	// Camera Object
	pMainCam = new CGameObject;
	pMainCam->SetName(L"AwardMainCam");
	pMainCam->AddComponent(new CTransform);
	pMainCam->AddComponent(new CCamera);
	pMainCam->AddComponent(new CCameraScript);

	pMainCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);

	//pMainCam->Transform()->SetLocalRot(Vec3(0.f, 3.14f, 0.f));
	pMainCam->Camera()->SetFar(100000.f);
	pMainCam->Camera()->SetLayerAllCheck();
	//pMainCam->Transform()->SetLocalPos(Vec3());
	pMainCam->Transform()->SetLocalPos(Vec3(0, 60.f * 8, 140.f * 7 + 200.f));
	pMainCam->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	m_pAwardScene->FindLayer(L"Default")->AddGameObject(pMainCam, m_pAwardScene);

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
	m_pAwardScene->FindLayer(L"Default")->AddGameObject(pObject, m_pAwardScene);


	//.bin으로 읽어오기
	tiles.clear();
	const wstring FileName = { L"NewAwardPos.bin" };
	LoadMapInfoFromFile(FileName, tiles);
	for (auto& tile : tiles)
	{
		Ptr<CMeshData> pMeshData = CResMgr::GetInst()->Load<CMeshData>(tile.GetPathName(), tile.GetPathName());
		pObject = pMeshData->Instantiate();
		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CCollider3D);
		pObject->Collider3D()->SetCollider3DType(COLLIDER3D_TYPE::CUBE);
		pObject->Collider3D()->SetOffsetScale(Vec3(1.f, 1.f, 1.f));
		pObject->Collider3D()->SetOffsetPos(Vec3(0.f, 10.f, 0.f));
		pObject->FrustumCheck(false);

		pObject->Transform()->SetLocalScale(tile.GetTileScaleV2());
		pObject->Transform()->SetLocalPos(tile.GetTilePos());
		pObject->MeshRender()->SetDynamicShadow(true);
		//pObject->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));
		if (tile.GetState() == LayerState::LAwardGrs)
		{
			pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 0.f));
			pObject->Transform()->SetLocalScale(Vec3(1.f,1.f,1.f));
		}
		else if (tile.GetState() == LayerState::LAwardTr)
		{
			pObject->Transform()->SetLocalRot(Vec3(3.14f / 2, 0.f, 3.14f));
		}
		else
		{
			pObject->Transform()->SetLocalRot(Vec3(0.f, 3.14f, 0.f));
		}
		//pObject->Animator3D()->SetClipIndex(1);
		m_pAwardScene->FindLayer(L"Award")->AddGameObject(pObject, m_pAwardScene);
	}

#if LOCALPLAY
	//m_pCurScene = m_pAwardScene;
	AddNetworkGameObject(true, Vec3::Zero, m_pAwardScene);
#else
#endif

	pObject = new CGameObject;
	pObject->SetName(L"SkyBox");
	pObject->FrustumCheck(false);
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyboxMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, m_pDaySkyBox.GetPointer());

	m_pAwardScene->FindLayer(L"Default")->AddGameObject(pObject, m_pAwardScene);
	
	//m_pAwardScene->Awake();
	//m_pAwardScene->Start();

	//===============================================================================================

	Ptr<CMeshData> victoryData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Victory.mdat", L"MeshData\\Player_Victory.mdat", false, true);
	Ptr<CMeshData> falldownMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Falldown.mdat", L"MeshData\\Player_Falldown.mdat", false, true);
	std::cout << "add obj" << std::endl;

	pObject = new CGameObject;
	pObject = victoryData->Instantiate();
	pObject->SetName(L"VictoryPlayer");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CAwardScript);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(3.0f, 3.0f, 3.0f));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	pObject->Transform()->SetLocalPos(Vec3(0.f, 10.f + 350.f, -200.f)); //플레이어위치
	pObject->GetScript<CAwardScript>()->AwardPlayerIdx(0);
	pObject->MeshRender()->SetDynamicShadow(true);
	m_pAwardScene->FindLayer(L"Player")->AddGameObject(pObject, m_pAwardScene, false);


	pObject = new CGameObject;
	pObject = victoryData->Instantiate();
	pObject->SetName(L"VictoryPlayer");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CAwardScript);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(3.0f, 3.0f, 3.0f));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	pObject->Transform()->SetLocalPos(Vec3(475.f, 10.f + 175.f, -125.f)); //플레이어위치
	pObject->GetScript<CAwardScript>()->AwardPlayerIdx(1);
	pObject->MeshRender()->SetDynamicShadow(true);
	m_pAwardScene->FindLayer(L"Player")->AddGameObject(pObject, m_pAwardScene, false);


	pObject = new CGameObject;
	pObject = victoryData->Instantiate();
	pObject->SetName(L"VictoryPlayer");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CAwardScript);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(3.0f, 3.0f, 3.0f));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	pObject->Transform()->SetLocalPos(Vec3(-475.f, 10.f + 175.f, -125.f)); //플레이어위치
	pObject->GetScript<CAwardScript>()->AwardPlayerIdx(1);
	pObject->MeshRender()->SetDynamicShadow(true);
	m_pAwardScene->FindLayer(L"Player")->AddGameObject(pObject, m_pAwardScene, false);

}

void CSceneMgr::InitUI()
{

	Ptr<CTexture> pStartButton = CResMgr::GetInst()->Load<CTexture>(L"UIButton", L"Texture\\startbutton.png");
	Ptr<CTexture> pWindow = CResMgr::GetInst()->Load<CTexture>(L"Window", L"Texture\\SWindow.png");;
	Ptr<CTexture> pSurvival = CResMgr::GetInst()->Load<CTexture>(L"Survival", L"Texture\\SSurvival.png");
	Ptr<CTexture> pRacing = CResMgr::GetInst()->Load<CTexture>(L"Racing", L"Texture\\SRacing.png");
	Ptr<CTexture> pMatching = CResMgr::GetInst()->Load<CTexture>(L"Matching", L"Texture\\Matching.png");
	Ptr<CTexture> pMatching0 = CResMgr::GetInst()->Load<CTexture>(L"Matching0", L"Texture\\Matching0.png");
	Ptr<CTexture> pMatching1 = CResMgr::GetInst()->Load<CTexture>(L"Matching1", L"Texture\\Matching1.png");
	Ptr<CTexture> pMatching2 = CResMgr::GetInst()->Load<CTexture>(L"Matching2", L"Texture\\Matching2.png"); 
	Ptr<CTexture> pMatching3 = CResMgr::GetInst()->Load<CTexture>(L"Matching3", L"Texture\\Matching3.png");
	Ptr<CTexture> pRoundOver = CResMgr::GetInst()->Load<CTexture>(L"RoundOver", L"Texture\\roundGameover2.png");
	Ptr<CTexture> pRoundStart = CResMgr::GetInst()->Load<CTexture>(L"RoundStart", L"Texture\\StartUI.png");
	Ptr<CTexture> pRoundDone = CResMgr::GetInst()->Load<CTexture>(L"RoundDone", L"Texture\\GameDone.png");
	Ptr<CTexture> pFeverUI  = CResMgr::GetInst()->Load<CTexture>(L"Fever", L"Texture\\FeverUI.png");
	Ptr<CTexture> pLobbyUI  = CResMgr::GetInst()->Load<CTexture>(L"Lobby", L"Texture\\GotoLobby.png");
	Ptr<CTexture> pTitle  = CResMgr::GetInst()->Load<CTexture>(L"Title", L"Texture\\TitleUI.png");

	
	Vec2 winsize = CGameFramework::GetInst()->m_WinSize;


	CGameObject* pUICam = nullptr;
	CGameObject* pObject = new CGameObject;

	for (int i = 0; i < 5; ++i)
	{

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


		switch (i)
		{
		case 0:
			m_pStartScene->FindLayer(L"UI")->AddGameObject(pUICam, m_pStartScene);
			break;
		case 1:
			m_pRacingScene->FindLayer(L"UI")->AddGameObject(pUICam, m_pRacingScene);
			break;
		case 2:
			m_pMetorScene->FindLayer(L"UI")->AddGameObject(pUICam, m_pMetorScene);
			break;
		case 3:
			m_pJumpingScene->FindLayer(L"UI")->AddGameObject(pUICam, m_pJumpingScene);
			break;
		case 4:
			m_pAwardScene->FindLayer(L"UI")->AddGameObject(pUICam, m_pAwardScene);
			break;
		default:
			break;
		}
	}
	pObject = new CGameObject;
	pObject->SetName(L"ToLobby UI");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::TOLOBBY);
	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(-winsize.x / 2, winsize.y / 8, 0));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x / 4, winsize.y / 12, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pLobbyUI.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-winsize.x / 2, winsize.y / 8, 0.f));
	pObject->SetActive(true);
	m_pAwardScene->FindLayer(L"UI")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	pObject->SetName(L"Start UI");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::START);
	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0 / 2, 580.f, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pRoundStart.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-winsize.x / 2, winsize.y / 8, 0.f));
	pObject->SetActive(false);
	m_pRacingScene->FindLayer(L"UI")->AddGameObject(pObject, m_pRacingScene);

	pObject = new CGameObject;
	pObject->SetName(L"Done UI");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::DONE);
	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0 / 2, 580.f, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pRoundDone.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-winsize.x / 2, winsize.y / 8, 0.f));
	pObject->SetActive(false);
	m_pRacingScene->FindLayer(L"UI")->AddGameObject(pObject, m_pRacingScene);


	pObject = new CGameObject;
	pObject->SetName(L"Fever UI");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::FEVER);
	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0 / 2, 580.f, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pFeverUI.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-winsize.x / 2, winsize.y / 8, 0.f));
	pObject->SetActive(false);
	m_pRacingScene->FindLayer(L"UI")->AddGameObject(pObject, m_pRacingScene);


	pObject = new CGameObject;
	pObject->SetName(L"UI Object");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::SELECT_BUTTON);
	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(-winsize.x/2, winsize.y/16, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x/4, winsize.y/12, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pStartButton.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-winsize.x / 2, winsize.y / 8, 0.f));
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);


	pObject = new CGameObject;
	pObject->SetName(L"Title");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);

	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::TITLE);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0, winsize.y / 4, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x*1.2 , winsize.x*1.2, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pTitle.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(true);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);


	pObject = new CGameObject;
	pObject->SetName(L"SWindow");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);

	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::WINDOW);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0, winsize.y/4, 0.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x*1.5, winsize.y/2.f, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pWindow.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);


	pObject = new CGameObject;
	pObject->SetName(L"SRacing");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::MODE_RACING);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(-winsize.x/3, 580.f, 5.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x/2, winsize.y/10, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pRacing.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);


	pObject = new CGameObject;
	pObject->SetName(L"SSurvival");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::MODE_SURVIVAL);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(winsize.x/3, 580.f, 2.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x / 2, winsize.y / 10, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pSurvival.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);

	pObject = new CGameObject;
	pObject->SetName(L"Matching");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::MATCHING);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0, 600.f, 2.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x/2, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pMatching.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);

	pObject = new CGameObject;
	pObject->SetName(L"Matching0");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::MATCHING3);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0, 600.f, 3.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x / 2, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pMatching0.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);

	pObject = new CGameObject;
	pObject->SetName(L"Matching1");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::MATCHING2);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0, 600.f, 3.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x / 2, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pMatching1.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);

	pObject = new CGameObject;
	pObject->SetName(L"Matching2");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::MATCHING1);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0, 600.f, 3.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x / 2, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pMatching2.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);

	pObject = new CGameObject;
	pObject->SetName(L"Matching3");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider2D);
	pObject->AddComponent(new CUIScript);
	pObject->GetScript<CUIScript>()->SetType(UI_TYPE::MATCHING0);

	// Transform ����
	pObject->Transform()->SetLocalPos(Vec3(0, 600.f, 3.f));
	pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
	pObject->Transform()->SetLocalScale(Vec3(winsize.x, winsize.x / 2, 1.f));
	// MeshRender ����
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
	pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pMatching3.GetPointer());
	// Collider2D
	pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
	pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
	pObject->SetActive(false);
	m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);


	Ptr<CTexture> pNums[10];
	for (int j = 0; j < 5; ++j)
	{
		for (int i = 0; i < 10; ++i)
		{
			for (int k = 0; k < 5; ++k)
			{


				wstring num = std::to_wstring(i);
				wstring tex = L"Texture\\NUM_";
				wstring png = L".png";
				wstring finalval = tex + num + png;

				wstring name = L"NUMS";
				wstring finalname = name + num;
				pNums[i] = CResMgr::GetInst()->Load<CTexture>(finalname, finalval);

				CGameObject* pObject = new CGameObject;
				pObject->SetName(finalname);
				pObject->AddComponent(new CTransform);
				pObject->AddComponent(new CMeshRender);
				pObject->AddComponent(new CCollider2D);
				pObject->AddComponent(new CUIScript);
				pObject->AddComponent(new CNumScript);

				pObject->GetScript<CUIScript>()->SetType(UI_TYPE::NUMBER);
				pObject->GetScript<CNumScript>()->SetNumInfo((NUM_TYPE)i, j, NUMPOS::NONE);

				// Transform ����
				pObject->Transform()->SetLocalPos(Vec3(300.f - 50.f * i, 600.f, 10.f));
				pObject->Transform()->SetLocalScale(Vec3(150.f, 150.f, 1.f));
				pObject->Transform()->SetLocalRot(Vec3(XM_PI, 0.f, XM_PI));
				// MeshRender ����
				pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
				pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"UIMtrl"));
				pObject->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, pNums[i].GetPointer());
				// Collider2D
				pObject->Collider2D()->SetCollider2DType(COLLIDER2D_TYPE::RECT);
				pObject->Collider2D()->SetOffsetPos(Vec3(-400.f, 400.f, 0.f));
				pObject->SetActive(false);

				switch (k)
				{
				case 0:
					m_pStartScene->FindLayer(L"UI")->AddGameObject(pObject, m_pStartScene);
					break;
				case 1:
					m_pRacingScene->FindLayer(L"UI")->AddGameObject(pObject, m_pRacingScene);
					break;
				case 2:
					m_pMetorScene->FindLayer(L"UI")->AddGameObject(pObject, m_pMetorScene);
					break;
				case 3:
					m_pJumpingScene->FindLayer(L"UI")->AddGameObject(pObject, m_pJumpingScene);
					break;
				case 4:
					m_pAwardScene->FindLayer(L"UI")->AddGameObject(pObject, m_pAwardScene);
					break;
				default:
					break;
				}
			}
		}
	}
}


void CSceneMgr::InitScene()
{
	m_pSceneType = SCENE_TYPE::LOBBY;
	m_pCurScene = m_pStartScene;

	Ptr<CSound> m_sounds = CResMgr::GetInst()->Load<CSound>(L"StartBgm", L"Sound\\LobbyBgm.wav");
	CRenderMgr::GetInst()->SetSound(m_sounds.GetPointer(), SOUND_TYPE::LOBBY);

	m_sounds = CResMgr::GetInst()->Load<CSound>(L"RacingBgm", L"Sound\\RacingBgm.wav");
	CRenderMgr::GetInst()->SetSound(m_sounds.GetPointer(), SOUND_TYPE::RACING);

	m_sounds = CResMgr::GetInst()->Load<CSound>(L"SurvivalBgm", L"Sound\\SurvivalBgm.wav");
	CRenderMgr::GetInst()->SetSound(m_sounds.GetPointer(), SOUND_TYPE::SURVIVAL);

	m_sounds = CResMgr::GetInst()->Load<CSound>(L"SelectBgm", L"Sound\\Select.wav");
	CRenderMgr::GetInst()->SetSound(m_sounds.GetPointer(), SOUND_TYPE::CLICK);

	m_sounds = CResMgr::GetInst()->Load<CSound>(L"JumpBgm", L"Sound\\Jump.wav");
	CRenderMgr::GetInst()->SetSound(m_sounds.GetPointer(), SOUND_TYPE::JUMP);

	m_sounds = CResMgr::GetInst()->Load<CSound>(L"SuperBgm", L"Sound\\SuperJump.wav");
	CRenderMgr::GetInst()->SetSound(m_sounds.GetPointer(), SOUND_TYPE::SUPERJUMP);

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
	if (curscene == NULL)
	{
		std::cout << "CurScene is NULL" << std::endl;
		curscene = GetCurScene();
	}
	//ChangeScene(curscene);
	//Ptr<CMeshData> idleData = CResMgr::GetInst()->LoadFBX(L"FBX\\test_run.fbx");
	//idleData->Save(idleData->GetPath());
	//pMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Victory.mdat", L"MeshData\\Player_Victory.mdat", false, true);
	Ptr<CMeshData> idleData= CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Idle.mdat", L"MeshData\\Player_Idle.mdat", false, true);
	Ptr<CMeshData> victoryData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Victory.mdat", L"MeshData\\Player_Victory.mdat", false, true);
	Ptr<CMeshData> runMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\test_run.mdat", L"MeshData\\test_run.mdat", false, true);
	Ptr<CMeshData> falldownMeshData = CResMgr::GetInst()->Load<CMeshData>(L"MeshData\\Player_Falldown.mdat", L"MeshData\\Player_Falldown.mdat", false, true);
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
	pPlayer->Transform()->SetLocalPos(Vec3(0.f, 10.f , 21400.f)); //10.f - FLOORHEIGHT
	//pPlayer->Transform()->SetLocalPos(Vec3(0.f, 10.f - FLOORHEIGHT, 15000.f));

	for (auto obj : curscene->FindLayer(L"Default")->GetParentObj())
	{
		if (obj->GetName().compare(L"MainCam") == 0)
		{
			obj->Transform()->SetLocalPos(Vec3(0, 60.f * 10, 140.f * 7));
			obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			pPlayer->AddChild(obj);
			//obj->Transform()->SetLocalPos(Vec3(-60,45,-10));
			//obj->Transform()->SetLocalScale(Vec3(15000.f, 15000.f, 15000.f));

			break;

		}
		else if (obj->GetName().compare(L"AwardMainCam") == 0)
		{
			//1등석
			pPlayer->Transform()->SetLocalPos(Vec3(0.f, 10.f + 350.f, -200.f));
			obj->Transform()->SetLocalPos(Vec3(0, 60.f * 3, 140.f * 7 + 200.f));
			obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			pPlayer->AddChild(obj);

			//2등석
			pPlayer->Transform()->SetLocalPos(Vec3(475.f, 10.f + 175.f, -125.f));
			obj->Transform()->SetLocalPos(Vec3(-470.f, 60.f * 3 + 250.f, 140.f * 7 + 125.f));
			obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			pPlayer->AddChild(obj);

			//3등석
			pPlayer->Transform()->SetLocalPos(Vec3(-475.f, 10.f + 175.f, -125.f));
			obj->Transform()->SetLocalPos(Vec3(470.f, 60.f * 3 + 250.f, 140.f * 7 + 125.f));
			obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			pPlayer->AddChild(obj);

			//기타등등..벽뒤에사람있어요.
			pPlayer->Transform()->SetLocalPos(Vec3(0.f, 10.f + 350.f, -780.f));
			obj->Transform()->SetLocalPos(Vec3(0, 60.f * 3, 140.f * 7 + 780.f));
			obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			pPlayer->AddChild(obj);
			break;

		}
	}

#else
	pPlayer->Transform()->SetLocalPos(pos);
#endif

	pPlayer->Transform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

	pPlayer->GetScript<CPlayerScript>()->SetPlayable(false);
	pPlayer->GetScript<CPlayerScript>()->SetType(ELEMENT_TYPE::FROZEN);
	pPlayer->GetScript<CPlayerScript>()->SetState(PLAYER_STATE::IDLE);
	CGameObject* pObject = nullptr;

	pObject = new CGameObject;
	pObject = idleData->Instantiate();
	pObject->SetName(L"IdlePlayer");
	pObject->AddComponent(new CTransform);
	pObject->Transform()->SetLocalScale(Vec3(1.5, 1.5, 1.5));
	pObject->SetActive(true);
	pObject->MeshRender()->SetDynamicShadow(true);
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));
	curscene->FindLayer(L"Player")->AddGameObject(pObject, curscene,false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pObject, false);


	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetIdlePlayer(pObject);

	pObject = new CGameObject;
	pObject = runMeshData->Instantiate();
	pObject->SetName(L"RunPlayer");
	pObject->AddComponent(new CTransform);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(1.5, 1.5, 1.5));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));

	pObject->MeshRender()->SetDynamicShadow(true);
	curscene->FindLayer(L"Player")->AddGameObject(pObject, curscene, false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pObject, false);

	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetRunPlayer(pObject);

	curscene->FindLayer(L"Player")->AddGameObject(pPlayer, curscene, false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pPlayer, false);

	pObject = new CGameObject;
	pObject = victoryData->Instantiate();
	pObject->SetName(L"VictoryPlayer");
	pObject->AddComponent(new CTransform);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(1.5, 1.5, 1.5));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));

	pObject->MeshRender()->SetDynamicShadow(true);
	curscene->FindLayer(L"Player")->AddGameObject(pObject, curscene, false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pObject, false);

	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetVictoryPlayer(pObject);
	curscene->FindLayer(L"Player")->AddGameObject(pPlayer, curscene, false);

	pObject = new CGameObject;
	pObject = falldownMeshData->Instantiate();
	pObject->SetName(L"FalldownPlayer");
	pObject->AddComponent(new CTransform);
	pObject->SetActive(true);
	pObject->Transform()->SetLocalScale(Vec3(1.5, 1.5, 1.5));
	pObject->Transform()->SetLocalRot(Vec3(0, -PI, 0));

	pObject->MeshRender()->SetDynamicShadow(true);
	curscene->FindLayer(L"Player")->AddGameObject(pObject, curscene, false);
	//m_pStartScene->FindLayer(L"Player")->AddGameObject(pObject, false);

	//pPlayer->AddChild(pObject);
	pPlayer->GetScript<CPlayerScript>()->SetFalldownPlayer(pObject);
	curscene->FindLayer(L"Player")->AddGameObject(pPlayer, curscene, false);

	return pPlayer;
}

void CSceneMgr::RemoveNetworkGameObject(CGameObject* obj)
{
	auto runPlayer = obj->GetScript<CPlayerScript>()->runPlayer;
	auto IdlePlayer = obj->GetScript<CPlayerScript>()->IdlePlayer;
	auto victoryPlayer = obj->GetScript<CPlayerScript>()->VictoryPlayer;
	auto FalldownPlayer = obj->GetScript<CPlayerScript>()->FalldownPlayer;
	m_pCurScene->GetLayer(obj->GetLayerIdx())->RemoveParentObj(obj);
	CSceneMgr::GetInst()->GetCurScene()->GetLayer(runPlayer->GetLayerIdx())->RemoveParentObj(runPlayer);
	CSceneMgr::GetInst()->GetCurScene()->GetLayer(IdlePlayer->GetLayerIdx())->RemoveParentObj(IdlePlayer);
	CSceneMgr::GetInst()->GetCurScene()->GetLayer(victoryPlayer->GetLayerIdx())->RemoveParentObj(victoryPlayer);
	CSceneMgr::GetInst()->GetCurScene()->GetLayer(FalldownPlayer->GetLayerIdx())->RemoveParentObj(FalldownPlayer);

	runPlayer->SetActive(false);
	IdlePlayer->SetActive(false);
	victoryPlayer->SetActive(false);
	FalldownPlayer->SetActive(false);
	obj->SetActive(false);
}


bool Compare(CGameObject* _pLeft, CGameObject* _pRight)
{
	return (_pLeft->Transform()->GetWorldPos().z < _pRight->Transform()->GetWorldPos().z);
}

