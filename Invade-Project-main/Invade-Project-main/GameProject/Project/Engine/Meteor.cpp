#include "pch.h"
#include "Meteor.h"
#include "TimeMgr.h"

void MeteorScript::Update()
{
	if (m_iType == GROUND)
	{
		Vec3 pObj = Transform()->GetLocalPos();
		GroundPos = pObj;
	}
	m_Time += CTimeMgr::GetInst()->GetDeltaTime()*0.3;
	m_iMapType;
	if (m_iType == MAP_TYPE::GROUND)
	{
		if (m_iMapType==CENTER)
		{
			if (m_Time < 2.0f)
			{

			}
			else if (m_Time > 2.0f && m_Time < 6.0f)
			{
				float saking = sin(100.0f * m_Time);
				Transform()->SetLocalPos(Vec3(GroundPos.x + saking, GroundPos.y, GroundPos.z + saking));
			}
			else
			{
				//GetObj()->SetActive(false);
			}
		}

	}
	else
	{
		float fFps = CTimeMgr::GetInst()->GetFPS();
		Vec3 pTarget;
		//맨처음에 프레임 낮을 때 확 튀는 거 방지
		if (fFps < 30.0f)
		{
			fFps = 30.0f;
		}
		m_fFrmSpeed += m_fSpeed;

		if (m_fFrmSpeed > 1.0f)
			m_fFrmSpeed = 0.f;

		if (m_curTarget == GROUND_TYPE::WATER)
			pTarget = m_TargetPos[GROUND_TYPE::WATER];
		else if (m_curTarget == GROUND_TYPE::GRASS)
			pTarget = m_TargetPos[GROUND_TYPE::GRASS];
		else if (m_curTarget == GROUND_TYPE::STONE)
			pTarget = m_TargetPos[GROUND_TYPE::STONE];
		else if (m_curTarget == GROUND_TYPE::WOOD)
			pTarget = m_TargetPos[GROUND_TYPE::WOOD];
		else if (m_curTarget == GROUND_TYPE::CENTER)
			pTarget = m_TargetPos[GROUND_TYPE::CENTER];


		Vec3 vPos = Vec3::Lerp(OriginPos, pTarget, m_fFrmSpeed);
		Transform()->SetLocalPos(vPos);
	}

}

MeteorScript::MeteorScript() :CScript((UINT)SCRIPT_TYPE::METEORSCRIPT), m_iDir(1)
{

#ifdef  LOCALPLAY
	m_TargetPos[GROUND_TYPE::WATER] = Vec3(-792, 10, 496);
	m_TargetPos[GROUND_TYPE::GRASS] = Vec3(796, 10, -342);
	m_TargetPos[GROUND_TYPE::STONE] = Vec3(407, 10, 706);
	m_TargetPos[GROUND_TYPE::WOOD] = Vec3(-402, 10, -699);
	m_TargetPos[GROUND_TYPE::CENTER] = Vec3(0, 0, 0);
#endif //  LOCALPLAY

}

MeteorScript::~MeteorScript()
{
}

void MeteorScript::Rotate(float value)
{
	//Vec3 vRot = Transform()->GetLocalRot();
	//if (m_iState == OBSTACLE_STATE::MOVEA)
	//{
	//	vRot.y = XMConvertToRadians(value);
	//}
	//else if (m_iState == OBSTACLE_STATE::MOVEB)
	//{
	//	vRot.z = sinf(XMConvertToRadians(value));
	//}
	//Transform()->SetLocalRot(vRot);
}

