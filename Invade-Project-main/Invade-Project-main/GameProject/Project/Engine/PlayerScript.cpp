#include "pch.h"
#include "PlayerScript.h"
#include "ArrowScript.h"
#include "MeshRender.h"
#include "Camera.h"
#include"CameraScript.h"
#include "NetworkMgr.h"


void CPlayerScript::Awake()
{

}

void CPlayerScript::Update()	// 여기서 상태바꿔주면될듯?
{
	if (!isPlayable) return;

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();

	//if (KEY_HOLD(KEY_TYPE::KEY_W) || KEY_HOLD(KEY_TYPE::KEY_S) || KEY_HOLD(KEY_TYPE::KEY_A) || KEY_HOLD(KEY_TYPE::KEY_D))
	//{
	//	if(!runPlayer->IsActive())
	//		runPlayer->SetActive(true);
	//	if(IdlePlayer->IsActive())
	//		IdlePlayer->SetActive(false);
	//}
	//else
	//{
	//	if (runPlayer->IsActive())
	//		runPlayer->SetActive(false);
	//	if (!IdlePlayer->IsActive())
	//		IdlePlayer->SetActive(true);
	//}

	runPlayer->Transform()->SetLocalPos(Vec3::Zero);
	IdlePlayer->Transform()->SetLocalPos(Vec3::Zero);
	
	if (KEY_HOLD(KEY_TYPE::KEY_W)) {
		if (!runPlayer->IsActive())
			runPlayer->SetActive(true);
		if (IdlePlayer->IsActive())
			IdlePlayer->SetActive(false);
		Vec3 vFront = Transform()->GetWorldDir(DIR_TYPE::FRONT);
		NetworkMgr::GetInst()->SendClientKeyInputPacket('w', vFront);
	}
	if (KEY_HOLD(KEY_TYPE::KEY_S)) {
		Vec3 vBack = -Transform()->GetWorldDir(DIR_TYPE::FRONT);
		NetworkMgr::GetInst()->SendClientKeyInputPacket('s', vBack);
	}
	if (KEY_HOLD(KEY_TYPE::KEY_A)) {
		Vec3 vLeft = -Transform()->GetWorldDir(DIR_TYPE::RIGHT);
		NetworkMgr::GetInst()->SendClientKeyInputPacket('a', vLeft);
	}
	if (KEY_HOLD(KEY_TYPE::KEY_D)) {
		Vec3 vRight = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
		NetworkMgr::GetInst()->SendClientKeyInputPacket('d', vRight);
	}
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
	Vec2 vDrag = CKeyMgr::GetInst()->GetDragDir();
	if (!m_bCheckStartMousePoint) {
		m_bCheckStartMousePoint = true;
	}
	else {
		vRot.y += vDrag.x * DT * 1.f;
		float fDegree = XMConvertToDegrees(vRot.y);
		if (fDegree < -360) {
			 fDegree+= 360.f;
			 vRot.y = XMConvertToRadians(fDegree);
		}
		else if (fDegree > 360) {
			fDegree -= 360.f;
			vRot.y = XMConvertToRadians(fDegree);
		}
	}
	



	Transform()->SetLocalRot(vRot);
	//Transform()->SetLocalPos(vPos);

}

CPlayerScript::CPlayerScript() :CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT), m_bCheckStartMousePoint(false), m_fArcherLocation(20.f)
{
}

CPlayerScript::~CPlayerScript()
{
}
