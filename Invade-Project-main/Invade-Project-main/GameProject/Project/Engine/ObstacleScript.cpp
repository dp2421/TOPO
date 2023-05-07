#include "pch.h"
#include "ObstacleScript.h"


void CObstacleScript::Update()
{

	//state 가 MOVE이면 360도 회전
	//if (m_iState == OBSTACLE_STATE::MOVEA)
	//{
	//	m_fSpeed += 5.f;
	//	vRot.y = XMConvertToRadians(m_fSpeed);
	//	//float fDegree = XMConvertToDegrees(m_fSpeed);
	//	if (m_fSpeed > 360) {
	//		m_fSpeed -= 360.f;
	//	}
	//}
	//state가 MOVEB면 시계추처럼 회전
	//if (m_iState == OBSTACLE_STATE::MOVEB)
	//{
	//	Vec3 vRot = Transform()->GetLocalRot();
	//	m_fSpeed += 5.f;
	//	vRot.z = sinf(XMConvertToRadians(m_fSpeed));
	//	//float fDegree = XMConvertToDegrees(m_fSpeed);
	//	if (m_fSpeed > 360) {
	//		m_fSpeed -= 360.f;
	//	}
	//	Transform()->SetLocalRot(vRot);
	//}

}

CObstacleScript::CObstacleScript() :CScript((UINT)SCRIPT_TYPE::OBSTACLESCRIPT), m_iDir(1)
{
}

CObstacleScript::~CObstacleScript()
{
}

void CObstacleScript::Rotate(float value)
{
	Vec3 vRot = Transform()->GetLocalRot();
	if (m_iState == OBSTACLE_STATE::MOVEA)
	{
		vRot.y = XMConvertToRadians(value);
	}
	else if (m_iState == OBSTACLE_STATE::MOVEB)
	{
		vRot.z = sinf(XMConvertToRadians(value));
	}
	Transform()->SetLocalRot(vRot);
}

