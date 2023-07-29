#include "pch.h"
#include "PlayerScript.h"
#include "ArrowScript.h"
#include "MeshRender.h"
#include "Camera.h"
#include"CameraScript.h"
#include "NetworkMgr.h"
#include "RenderMgr.h"
#include "UIScript.h"
//#include "ParticleSystem.h"

void CPlayerScript::Awake()
{
	//pushTime = std::chrono::system_clock::now();

	//CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	//m_pParticle = new CGameObject;
	//m_pParticle->SetName(L"Particle");
	//m_pParticle->AddComponent(new CTransform);
	//switch (m_iType)
	//{
	//case ELEMENT_TYPE::FROZEN:
	//	m_pParticle->AddComponent(new CParticleSystem);
	//	m_pParticle->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"Snow"));
	//	m_pParticle->ParticleSystem()->SetStartColor(Vec4(1.f, 0.7f, 0, 1.f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	//	m_pParticle->ParticleSystem()->SetEndColor(Vec4(1.f, 1.f, 0.7f, 1.0f));
	//	m_pParticle->ParticleSystem()->SetMaxLifeTime(3.0f);
	//	m_pParticle->ParticleSystem()->SetMinLifeTime(0.5f);
	//	m_pParticle->ParticleSystem()->SetStartScale(6.f);
	//	m_pParticle->ParticleSystem()->SetEndScale(9.f);
	//	break;
	//case ELEMENT_TYPE::FIRE:
	//	m_pParticle->AddComponent(new CParticleSystem);
	//	m_pParticle->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"particle_00"));
	//	m_pParticle->ParticleSystem()->SetStartColor(Vec4(1.f, 1.f, 0.f, 0.5f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	//	m_pParticle->ParticleSystem()->SetEndColor(Vec4(1.f, 0.f, 0.f, 1.0f));
	//	m_pParticle->ParticleSystem()->SetStartScale(2.f);
	//	m_pParticle->ParticleSystem()->SetEndScale(5.f);
	//	break;
	//case ELEMENT_TYPE::DARK:
	//	m_pParticle->AddComponent(new CParticleSystem);
	//	m_pParticle->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"smokeparticle"));
	//	m_pParticle->ParticleSystem()->SetStartColor(Vec4(1.f, 1.f, 0.f, 0.5f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	//	m_pParticle->ParticleSystem()->SetEndColor(Vec4(0.f, 0.f, 0.f, 1.0f));
	//	m_pParticle->ParticleSystem()->SetStartScale(2.f);
	//	m_pParticle->ParticleSystem()->SetEndScale(5.f);
	//	break;
	//case ELEMENT_TYPE::THUNDER:
	//	break;
	//case ELEMENT_TYPE::WIND:
	//	break;
	//}
	//m_pParticle->FrustumCheck(false);
	//m_pParticle->Transform()->SetLocalPos(Vec3(1.5f, 150.f, 0.f));
	//pCurScene->FindLayer(L"Default")->AddGameObject(m_pParticle, pCurScene);
	//GetObj()->AddChild(m_pParticle);
}

void CPlayerScript::Update()
{

#if LOCALPLAY
#else

	if (!isPlayable)
	{
		if (moveState == 1)
		{
			if (!runPlayer->IsActive())
			{
				runPlayer->MeshRender()->SetDynamicShadow(true);
				runPlayer->SetActive(true);
			}
			if (IdlePlayer->IsActive() || VictoryPlayer->IsActive() || FalldownPlayer->IsActive())
			{
				IdlePlayer->SetActive(false);
				VictoryPlayer->SetActive(false);
				FalldownPlayer->SetActive(false);
			}
		}
		else
		{
			if (runPlayer->IsActive() || VictoryPlayer->IsActive() || FalldownPlayer->IsActive())
			{
				VictoryPlayer->SetActive(false);
				FalldownPlayer->SetActive(false);
				runPlayer->SetActive(false);
			}
			if (!IdlePlayer->IsActive())
			{
				IdlePlayer->MeshRender()->SetDynamicShadow(true);
				IdlePlayer->SetActive(true);
			}
		}
		prePosition = Transform()->GetLocalPos();
		return;
	}
#endif

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();

	Vec3 dir = Vec3::Zero;

	float fDegree = XMConvertToDegrees(vRot.y);
	Vec2 vDrag = CKeyMgr::GetInst()->GetDragDir();
	if (!m_bCheckStartMousePoint) {
		m_bCheckStartMousePoint = true;
	}
	else {
		vRot.y += vDrag.x * DT * 1.f;
		if (fDegree < -360) {
			fDegree += 360.f;
			vRot.y = XMConvertToRadians(fDegree);
		}
		else if (fDegree > 360) {
			fDegree -= 360.f;
			vRot.y = XMConvertToRadians(fDegree);
		}
	}

#if LOCALPLAY
	bool isMove = true; //로컬테스트용

	if (KEY_HOLD(KEY_TYPE::KEY_W))
	{
		dir += -Transform()->GetWorldDir(DIR_TYPE::FRONT);
	}
	if (KEY_HOLD(KEY_TYPE::KEY_S))
	{
		dir += Transform()->GetWorldDir(DIR_TYPE::FRONT);
	}
	if (KEY_HOLD(KEY_TYPE::KEY_A))
	{
		dir += Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	}
	if (KEY_HOLD(KEY_TYPE::KEY_D))
	{
		dir += -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	}

	if (dir != Vec3::Zero)
	{
		runPlayer->MeshRender()->SetDynamicShadow(true);
		runPlayer->SetActive(true);
		IdlePlayer->MeshRender()->SetDynamicShadow(false);
		IdlePlayer->SetActive(false);
	}
	else
	{
		runPlayer->MeshRender()->SetDynamicShadow(false);
		runPlayer->SetActive(false);
		IdlePlayer->MeshRender()->SetDynamicShadow(true);
		IdlePlayer->SetActive(true);

	}

	vPos += dir.Normalize() * SPEED * DT;
	Transform()->SetLocalPos(vPos);
	SetPlayerPos(vPos);
#else
	bool isMove = moveState;

	if (KEY_TAB(KEY_TYPE::KEY_SPACE))
	{
		CRenderMgr::GetInst()->PlayEffect(SOUND_TYPE::JUMP);
		NetworkMgr::GetInst()->SendClientKeyInputPacket(KeyType::Jump, dir, fDegree);
	}

	if (KEY_TAB(KEY_TYPE::KEY_LCTRL) && std::chrono::system_clock::now() > pushTime)
	{
		CRenderMgr::GetInst()->PlayEffect(SOUND_TYPE::JUMP);
		NetworkMgr::GetInst()->SendClientKeyInputPacket(KeyType::Push, dir, fDegree);
	}

	SetPlayerMoveState(KEY_TYPE::KEY_W, KEY_STATE::STATE_TAB, dir);
	SetPlayerMoveState(KEY_TYPE::KEY_S, KEY_STATE::STATE_TAB, dir);
	SetPlayerMoveState(KEY_TYPE::KEY_A, KEY_STATE::STATE_TAB, dir);
	SetPlayerMoveState(KEY_TYPE::KEY_D, KEY_STATE::STATE_TAB, dir);

	SetPlayerMoveState(KEY_TYPE::KEY_W, KEY_STATE::STATE_AWAY, dir);
	SetPlayerMoveState(KEY_TYPE::KEY_S, KEY_STATE::STATE_AWAY, dir);
	SetPlayerMoveState(KEY_TYPE::KEY_A, KEY_STATE::STATE_AWAY, dir);
	SetPlayerMoveState(KEY_TYPE::KEY_D, KEY_STATE::STATE_AWAY, dir);

	if (moveState != (int)Direction::None && !isMove)
	{
		dir.Normalize();
		NetworkMgr::GetInst()->SendClientKeyInputPacket(KeyType::MoveStart, dir, fDegree);
	}
	if (moveState != (int)Direction::None)
	{

		if (!runPlayer->IsActive())
		{
			runPlayer->SetActive(true);
		}
		runPlayer->MeshRender()->SetDynamicShadow(true);
		if (IdlePlayer->IsActive() || VictoryPlayer->IsActive() || FalldownPlayer->IsActive())
		{
			VictoryPlayer->SetActive(false);
			FalldownPlayer->SetActive(false);
			IdlePlayer->SetActive(false);
		}
		if ((moveState & (int)Direction::Front) == (int)Direction::Front)
		{
			dir += -Transform()->GetWorldDir(DIR_TYPE::FRONT);
		}
		if ((moveState & (int)Direction::Back) == (int)Direction::Back)
		{
			dir += +Transform()->GetWorldDir(DIR_TYPE::FRONT);
		}
		if ((moveState & (int)Direction::Left) == (int)Direction::Left)
		{
			dir += +Transform()->GetWorldDir(DIR_TYPE::RIGHT);
		}
		if ((moveState & (int)Direction::Right) == (int)Direction::Right)
		{
			dir += -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
		}

		NetworkMgr::GetInst()->SendClientMovePacket(dir, fDegree);

	}
	else
	{

		if (runPlayer->IsActive() || VictoryPlayer->IsActive() || FalldownPlayer->IsActive())
		{
			VictoryPlayer->SetActive(false);
			FalldownPlayer->SetActive(false);
			runPlayer->SetActive(false);
		}
		if (!IdlePlayer->IsActive())
		{
			IdlePlayer->SetActive(true);
		}
		IdlePlayer->MeshRender()->SetDynamicShadow(true);
	}
	//std::cout << "run: " << std::boolalpha << runPlayer->MeshRender()->IsDynamicShadow() << std::endl;
	//std::cout << "Idle: " << std::boolalpha << IdlePlayer->MeshRender()->IsDynamicShadow() << std::endl;


#endif

	Pushed(isStun, stunTime);

	//if (m_isSetAwardScene)
	//{
	//	CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::AWARD);
	//	CRenderMgr::GetInst()->SetSceneChanged(true);
	//}

	if (CSceneMgr::GetInst()->GetSceneType() == SCENE_TYPE::AWARD)
	{
		std::cout << "플레이어스크립트 : 시상식씬 들어왓답니다" << std::endl;
		if (true)
			SetPlayerPos(Vec3(0.f, 10.f + 350.f, -200.f), -180);
		//else if (true)	// 2등
		//	SetPlayerPos(Vec3(475.f, 10.f + 175.f, -125.f));
		//else if (true) //3등 
		//	SetPlayerPos(Vec3(-475.f, 10.f + 175.f, -125.f));
		//else	// 패배자들
		//	SetPlayerPos(Vec3(0.f, 10.f + 350.f, -780.f));
		if (!VictoryPlayer->IsActive())
			VictoryPlayer->SetActive(true);
		if (IdlePlayer->IsActive() || runPlayer->IsActive() || FalldownPlayer->IsActive())
		{
			IdlePlayer->SetActive(false);
			runPlayer->SetActive(false);
			FalldownPlayer->SetActive(false);
		}
			for (auto obj : CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->GetParentObj())
			{
				if (obj->GetName().compare(L"AwardMainCam") == 0)
				{
					obj->Transform()->SetLocalRot(Vec3(PI/8, 0, 0));
					obj->Transform()->SetLocalPos(Vec3(0, 500, -1500));
					Vec3 temp = obj->Transform()->GetLocalRot();
					//std::cout << XMConvertToDegrees(temp.x) << ", " << XMConvertToDegrees(temp.y) << ", " << XMConvertToDegrees(temp.z) << std::endl;
				}
			}
		
	}

	IdlePlayer->Transform()->SetLocalRot(vRot);
	runPlayer->Transform()->SetLocalRot(vRot);
	Transform()->SetLocalRot(vRot);

	m_isFever = CRenderMgr::GetInst()->IsFever();
	LetParticle(vPos, PARTICLE_TYPE::RUNPARTICLE, isMove);
	SetSpeedLine(isMove);


}

void CPlayerScript::SetPlayable(bool value)
{
	isPlayable = value;
	if (isPlayable == true)
	{
		for (auto obj : CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->GetParentObj())
		{
			if (obj->GetName().compare(L"MainCam") == 0)
			{
				obj->Transform()->SetLocalPos(Vec3(0, 60.f * 5, 220.f * 3));
				obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
				GetObj()->AddChild(obj);

				////pMainCam->Transform()->SetLocalPos(Vec3(-60,45,-10));
			//pMainCam->Transform()->SetLocalScale(Vec3(15000.f, 15000.f, 15000.f));
				break;
			}
			//else if (obj->GetName().compare(L"AwardMainCam") == 0)
			//{
			//	//1등석
			//	SetPlayerPos(Vec3(0.f, 10.f + 350.f, -200.f));
			//	obj->Transform()->SetLocalPos(Vec3(0, 60.f * 3, 140.f * 7 + 200.f));
			//	obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			//	//2등석
			//	SetPlayerPos(Vec3(475.f, 10.f + 175.f, -125.f));
			//	obj->Transform()->SetLocalPos(Vec3(-470.f, 60.f * 3 + 250.f, 140.f * 7 + 125.f));
			//	obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			//	//3등석
			//	SetPlayerPos(Vec3(-475.f, 10.f + 175.f, -125.f));
			//	obj->Transform()->SetLocalPos(Vec3(470.f, 60.f * 3 + 250.f, 140.f * 7 + 125.f));
			//	obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			//	//기타등등들
			//	SetPlayerPos(Vec3(0.f, 10.f + 350.f, -780.f));
			//	obj->Transform()->SetLocalPos(Vec3(0, 60.f * 3, 140.f * 7 + 780.f));
			//	obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
			//	//
			//	GetObj()->AddChild(obj);
			//	break;
			//}

		}
	}
}

const void CPlayerScript::SetPlayerMoveState(KEY_TYPE key, KEY_STATE state, Vec3& dir)
{
	if (KEY(key, state))
	{
		if (state == KEY_STATE::STATE_TAB)
		{
			switch (key)
			{
			case KEY_TYPE::KEY_W:
				moveState |= (int)Direction::Front;
				dir += -Transform()->GetWorldDir(DIR_TYPE::FRONT);
				break;
			case KEY_TYPE::KEY_A:

				moveState |= (int)Direction::Left;
				dir += -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
				//moveState |= (int)Direction::Back;
				//dir += Transform()->GetWorldDir(DIR_TYPE::FRONT);
				break;
			case KEY_TYPE::KEY_S:
				//moveState |= (int)Direction::Left;
				//dir += Transform()->GetWorldDir(DIR_TYPE::RIGHT);

				moveState |= (int)Direction::Back;
				dir += Transform()->GetWorldDir(DIR_TYPE::FRONT);
				break;
			case KEY_TYPE::KEY_D:
				moveState |= (int)Direction::Right;
				dir += +Transform()->GetWorldDir(DIR_TYPE::RIGHT);
				break;
			default:
				break;
			}
		}
		else if (state == KEY_STATE::STATE_AWAY)
		{
			switch (key)
			{
			case KEY_TYPE::KEY_W:
				moveState &= ~(int)Direction::Front;
				break;
			case KEY_TYPE::KEY_A:
				moveState &= ~(int)Direction::Left;
				break;
			case KEY_TYPE::KEY_S:
				moveState &= ~(int)Direction::Back;
				break;
			case KEY_TYPE::KEY_D:
				moveState &= ~(int)Direction::Right;
				break;
			default:
				break;
			}

			if (moveState == 0)
			{
				Vec3 vRot = Transform()->GetLocalRot();
				NetworkMgr::GetInst()->SendClientKeyInputPacket(KeyType::MoveEnd, dir, XMConvertToDegrees(vRot.y));
			}
		}
	}
}

void CPlayerScript::SetPlayerPos(Vec3 pos, float degree, bool isMove, bool isGoal)
{
	if (!isPlayable)
	{
		moveState = (int)isMove;
		Vec3 vRot = Transform()->GetLocalRot();
		vRot.y = XMConvertToRadians(degree);
		IdlePlayer->Transform()->SetLocalRot(vRot);
		runPlayer->Transform()->SetLocalRot(vRot);
		Transform()->SetLocalRot(vRot);
	}
	prePosition = Transform()->GetLocalPos();
	Transform()->SetLocalPos(pos);
	runPlayer->Transform()->SetLocalPos(pos);
	IdlePlayer->Transform()->SetLocalPos(pos);
}


void CPlayerScript::LetParticle(Vec3 pos, PARTICLE_TYPE type, bool isstart)
{
	Vec3 notCollPos = Vec3(40000.f, 0.f, 0.f);
	for (auto obj : CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->GetObjects())
	{
		//if (obj->GetName().compare(L"Particle") == 0 && type == PARTICLE_TYPE::COLLPARICLE)
		if (obj->GetName().compare(L"Particle") == 0 && type == PARTICLE_TYPE::COLLPARICLE)
		{
			if (isstart)
			{
				obj->Transform()->SetLocalPos(Vec3(pos.x + 1.5f, pos.y + 150.f, pos.z));
				break;
			}
			else
			{
				obj->Transform()->SetLocalPos(notCollPos);
				break;
			}
		}
		if ((type == PARTICLE_TYPE::RUNPARTICLE && obj->GetName().compare(L"CartoonParticle") == 0))
		{
			if (isstart)
				obj->Transform()->SetLocalPos(Vec3(pos.x, pos.y - 10.f, pos.z - 10.f));
			else
				obj->Transform()->SetLocalPos(notCollPos);
		}
		if ((type == PARTICLE_TYPE::RUNPARTICLE && obj->GetName().compare(L"CartoonParticleF") == 0))
		{
			if (isstart && m_isFever)
				obj->Transform()->SetLocalPos(Vec3(pos.x, pos.y - 10.f, pos.z + 10.f));
			else
				obj->Transform()->SetLocalPos(notCollPos);
		}
	}
}

void CPlayerScript::SetSpeedLine(bool ismove)
{
	if (m_isFever)
	{
		for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
		{
			if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::SPEEDLINE)
				obj->SetActive(ismove);
		}
	}

}
void CPlayerScript::Pushed(bool ispush, std::chrono::system_clock::time_point time)
{

	if (ispush && (std::chrono::system_clock::now() <= time))
	{
		if (!FalldownPlayer->IsActive())
			FalldownPlayer->SetActive(true);
		if (IdlePlayer->IsActive() || runPlayer->IsActive())
		{
			IdlePlayer->SetActive(false);
			runPlayer->SetActive(false);
		}
	}

}

void CPlayerScript::startAwardScene(int rank)
{
	//CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::AWARD);
	//CRenderMgr::GetInst()->SetSceneChanged(true);
	//CRenderMgr::GetInst()->SetFever(false);

	std::cout << "StartAwardScene 진입..." << std::endl;

	Vec3 rot = Vec3(0.f, 3.14f, 0.f); //정면보게 회전?
	IdlePlayer->Transform()->SetLocalRot(rot);
	runPlayer->Transform()->SetLocalRot(rot);
	Transform()->SetLocalRot(rot);

	switch (rank)
	{
	case 0: //1등
		SetPlayerPos(Vec3(0.f, 10.f + 350.f, -200.f), -180);
		break;
	case 1: //2등
		SetPlayerPos(Vec3(475.f, 10.f + 175.f, -125.f));
		break;
	case 2: //3등
		SetPlayerPos(Vec3(-475.f, 10.f + 175.f, -125.f));
		break;
	default: //패배자들 : -1값넘어올때
		SetPlayerPos(Vec3(0.f, 10.f + 350.f, -780.f));
		break;
	}

	auto test = CSceneMgr::GetInst()->GetCurScene()->GetName();

	for (auto obj : CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->GetParentObj())
	{
		if (obj->GetName().compare(L"AwardMainCam") == 0)
		{
			obj->Transform()->SetLocalRot(Vec3(PI / 8, 0, 0));
			obj->Transform()->SetLocalPos(Vec3(0, 500, -1500));
			Vec3 temp = obj->Transform()->GetLocalRot();
			//std::cout << XMConvertToDegrees(temp.x) << ", " << XMConvertToDegrees(temp.y) << ", " << XMConvertToDegrees(temp.z) << std::endl;
		}
	}
}


CPlayerScript::CPlayerScript() :CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT), m_bCheckStartMousePoint(false), m_fArcherLocation(20.f)
{

}

CPlayerScript::~CPlayerScript()
{
}

