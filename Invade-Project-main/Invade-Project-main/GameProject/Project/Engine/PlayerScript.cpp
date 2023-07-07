#include "pch.h"
#include "PlayerScript.h"
#include "ArrowScript.h"
#include "MeshRender.h"
#include "Camera.h"
#include"CameraScript.h"
#include "NetworkMgr.h"
#include "ParticleSystem.h"

void CPlayerScript::Awake()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	m_pParticle = new CGameObject;
	m_pParticle->SetName(L"Particle");
	m_pParticle->AddComponent(new CTransform);
	switch (m_iType)
	{
	case ELEMENT_TYPE::FROZEN:
		m_pParticle->AddComponent(new CParticleSystem);
		m_pParticle->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"Snow"));
		m_pParticle->ParticleSystem()->SetStartColor(Vec4(1.f, 0.7f, 0, 1.f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
		m_pParticle->ParticleSystem()->SetEndColor(Vec4(1.f, 1.f, 0.7f, 1.0f));
		m_pParticle->ParticleSystem()->SetMaxLifeTime(2.0f);
		m_pParticle->ParticleSystem()->SetMinLifeTime(0.5f);
		m_pParticle->ParticleSystem()->SetStartScale(6.f);
		m_pParticle->ParticleSystem()->SetEndScale(5.f);

		break;
	case ELEMENT_TYPE::FIRE:
		m_pParticle->AddComponent(new CParticleSystem);
		m_pParticle->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"particle_00"));
		m_pParticle->ParticleSystem()->SetStartColor(Vec4(1.f, 1.f, 0.f, 0.5f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
		m_pParticle->ParticleSystem()->SetEndColor(Vec4(1.f, 0.f, 0.f, 1.0f));
		m_pParticle->ParticleSystem()->SetStartScale(2.f);
		m_pParticle->ParticleSystem()->SetEndScale(5.f);
		break;
	case ELEMENT_TYPE::DARK:
		m_pParticle->AddComponent(new CParticleSystem);
		m_pParticle->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"smokeparticle"));
		m_pParticle->ParticleSystem()->SetStartColor(Vec4(1.f, 1.f, 0.f, 0.5f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
		m_pParticle->ParticleSystem()->SetEndColor(Vec4(0.f, 0.f, 0.f, 1.0f));
		m_pParticle->ParticleSystem()->SetStartScale(2.f);
		m_pParticle->ParticleSystem()->SetEndScale(5.f);
		break;
	case ELEMENT_TYPE::THUNDER:

		break;
	case ELEMENT_TYPE::WIND:

		break;

	}

	m_pParticle->FrustumCheck(false);
	m_pParticle->Transform()->SetLocalPos(Vec3(0.5f, 0.f, 0.f));
	pCurScene->FindLayer(L"Default")->AddGameObject(m_pParticle);

	GetObj()->AddChild(m_pParticle);
}

void CPlayerScript::Update()
{

	//if (KEY_TAB(KEY_TYPE::KEY_LBTN)) {
	//	CGameObject* pObj = GetObj()->GetChild()[0];
	//	Vec3 vPos = pObj->Transform()->GetLocalPos();
	//	Vec3 vRight = pObj->Transform()->GetLocalDir(DIR_TYPE::RIGHT);
	//	Vec3 vFront = pObj->Transform()->GetLocalDir(DIR_TYPE::FRONT);
	//	m_vRestorePos = vPos;
	//	vPos += vRight * 10.f;
	//	vPos += vFront * 10.f;
	//	pObj->Transform()->SetLocalPos(vPos);
	//	m_fArrowSpeed = 200.f;

	//}

#if LOCALPLAY
#else

	if (!isPlayable)
	{
		if (moveState == 1)
		{
			if (!runPlayer->IsActive())
				runPlayer->SetActive(true);
			if (IdlePlayer->IsActive())
				IdlePlayer->SetActive(false);
		}
		else
		{
			if (runPlayer->IsActive())
				runPlayer->SetActive(false);
			if (!IdlePlayer->IsActive())
				IdlePlayer->SetActive(true);
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

	//std::cout << fDegree << std::endl;

#if LOCALPLAY

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
		runPlayer->SetActive(true);
		IdlePlayer->SetActive(false);
	}
	else
	{
		runPlayer->SetActive(false);
		IdlePlayer->SetActive(true);
	}

	vPos += dir.Normalize() * SPEED * DT;
	Transform()->SetLocalPos(vPos);
	SetPlayerPos(vPos);
#else
	bool isMove = moveState;

	if (KEY_TAB(KEY_TYPE::KEY_SPACE))
	{
		NetworkMgr::GetInst()->SendClientKeyInputPacket(KeyType::Jump, dir, fDegree);
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
			runPlayer->SetActive(true);
		if (IdlePlayer->IsActive())
			IdlePlayer->SetActive(false);

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
		if (runPlayer->IsActive())
			runPlayer->SetActive(false);
		if (!IdlePlayer->IsActive())
			IdlePlayer->SetActive(true);
	}
#endif

	/*
	if (KEY_TAB(KEY_TYPE::KEY_LBTN)) {
		CGameObject* pObj=GetObj()->GetChild()[0];
		Vec3 vPos=pObj->Transform()->GetLocalPos();
		Vec3 vRight = pObj->Transform()->GetLocalDir(DIR_TYPE::RIGHT);
		Vec3 vFront = pObj->Transform()->GetLocalDir(DIR_TYPE::FRONT);
		m_vRestorePos = vPos;
		vPos += vRight * 10.f;
		vPos += vFront * 10.f;
		pObj->Transform()->SetLocalPos(vPos);
		m_fArrowSpeed = 200.f;
		
	}
	
	if (KEY_HOLD(KEY_TYPE::KEY_LBTN)) {
		m_fArrowSpeed += 1000.f*DT;
		if (m_fArrowSpeed > 2000.f) {
			m_fArrowSpeed = 2000.f;
		}
	}
	
	if (KEY_AWAY(KEY_TYPE::KEY_LBTN)) {
		CGameObject* pObj = GetObj()->GetChild()[0];
		Vec3 vPos = pObj->Transform()->GetLocalPos();
		Vec3 vRight =-pObj->Transform()->GetLocalDir(DIR_TYPE::RIGHT);
		Vec3 vFront = -pObj->Transform()->GetLocalDir(DIR_TYPE::FRONT);
		m_vRestorePos = vPos;
		vPos += vRight * 10.f;
		vPos += vFront * 10.f;
		pObj->Transform()->SetLocalPos(vPos);
		m_pArrow[m_iCurArrow]->SetActive(true);
		m_pArrow[m_iCurArrow]->GetScript<CArrowScript>()->Init();
		vRight = pObj->Transform()->GetWorldDir(DIR_TYPE::FRONT);
		m_pArrow[m_iCurArrow]->GetScript<CArrowScript>()->SetDir(vRight);
		m_pArrow[m_iCurArrow]->GetScript<CArrowScript>()->SetSpeed(m_fArrowSpeed);
		m_pArrow[m_iCurArrow]->GetScript<CArrowScript>()->SetVelocityX();
		m_pArrow[m_iCurArrow]->GetScript<CArrowScript>()->SetVelocityZ();


		Vec2 xzValue = GetDiagnal(m_fArcherLocation, vRight.x, vRight.z);
	

		CCameraScript* p=dynamic_cast<CCameraScript*>(GetObj()->GetChild()[0]->GetScripts()[0]);
		float fDegree= p->GetDegree();
		float fDegree2 = fDegree;
		fDegree *= -1.f;
	
	
		float yValue = sin(XMConvertToRadians(fDegree)) * m_fArcherLocation;

		Vec3 vArrowPos = Vec3(GetObj()->Transform()->GetLocalPos().x+xzValue.x, GetObj()->Transform()->GetLocalPos().y + 70+yValue, GetObj()->Transform()->GetLocalPos().z+xzValue.y);

		
		m_pArrow[m_iCurArrow]->Transform()->SetLocalPos(vArrowPos);
		fDegree *= -1.f;
		


		m_pArrow[m_iCurArrow]->Transform()->SetLocalRot(Vec3(GetObj()->Transform()->GetLocalRot().x, GetObj()->Transform()->GetLocalRot().y, GetObj()->Transform()->GetLocalRot().z));
		if (m_iCurArrow == 0) {
			int a = 0;
		}
		
		
		Vec3 vFront2 = vArrowPos;
		Vec3 vRight2 = Vec3(1, 0, 0);
		auto k = XMLoadFloat3(&vRight2);
		auto m = XMLoadFloat4x4(&m_pArrow[m_iCurArrow]->Transform()->GetWorldMat());
		auto r = XMVector3TransformNormal(k, m);
		XMFLOAT3 result;
		XMStoreFloat3(&result, XMVector3Normalize(r));
	
		float flength = sqrt(pow(result.x, 2) + pow(result.z, 2));
	
	
		vArrowPos.x += result.x;
		vArrowPos.z += result.z;
		float xValue = sqrt(pow(m_fArcherLocation, 2) - pow(yValue, 2));
		float xValue2 = xValue + flength;
		float fyValue2 = yValue * xValue2 / xValue;
	
		float SubeyValue2Value = fyValue2 - yValue;
		m_pArrow[m_iCurArrow]->GetScript<CArrowScript>()->SetFallSpeen(SubeyValue2Value);

		vArrowPos.y += SubeyValue2Value;
		Vec3 vTarget = vArrowPos - vFront2;
	
		vTarget.Normalize();
		float vDotValue = Dot(vTarget, result);
		Vec3 vCrossValue;
		if (vTarget.y > 0.f) {
			vCrossValue = Cross(vTarget, result);
		}
		else {
			vCrossValue = Cross(result, vTarget);
		}
		
		
		if (vCrossValue != Vec3(0.f, 0.f, 0.f)) {
	
			XMVECTOR xmmatrix = XMQuaternionRotationAxis(XMLoadFloat3(&vCrossValue), XMConvertToRadians(fDegree2));
			m_pArrow[m_iCurArrow]->Transform()->SetQuaternion(XMQuaternionMultiply(m_pArrow[m_iCurArrow]->Transform()->GetQuaternion(), xmmatrix));
	
		}
		
		m_iCurArrow++;
		m_iPower = 1;
		if (m_iCurArrow > 19) { 
			m_iCurArrow = 0;
		
		}

	}
	*/

	IdlePlayer->Transform()->SetLocalRot(vRot);
	runPlayer->Transform()->SetLocalRot(vRot);
	Transform()->SetLocalRot(vRot);
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
				obj->Transform()->SetLocalPos(Vec3(0, 60.f * 7, 220.f * 7));
				obj->Transform()->SetLocalRot(Vec3(0, -PI, 0));
				GetObj()->AddChild(obj);

				////pMainCam->Transform()->SetLocalPos(Vec3(-60,45,-10));
			//pMainCam->Transform()->SetLocalScale(Vec3(15000.f, 15000.f, 15000.f));
				break;
			}
			
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

void CPlayerScript::SetPlayerPos(Vec3 pos, float degree, bool isMove)
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

CPlayerScript::CPlayerScript() :CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT), m_bCheckStartMousePoint(false), m_fArcherLocation(20.f)
{

}

CPlayerScript::~CPlayerScript()
{
}
