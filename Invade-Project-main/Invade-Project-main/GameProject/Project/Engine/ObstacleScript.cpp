#include "pch.h"
#include "ObstacleScript.h"
#include "TimeMgr.h"

void CObstacleScript::Update()
{
	Vec3 vRot = Transform()->GetLocalRot();
	float fFps = CTimeMgr::GetInst()->GetFPS();

	//맨처음에 프레임 낮을 때 확 튀는 거 방지
	if (fFps < 30.0f)
	{
		fFps = 30.0f;
	}
	m_fFrmSpeed += m_fSpeed / fFps;

	if (m_fFrmSpeed > 360)
		m_fFrmSpeed -= 360.f;

	//state 가 MOVE이면 360도 회전
	if (m_iState == OBSTACLE_STATE::MOVEA)
	{
		vRot.y = XMConvertToRadians(m_fFrmSpeed);
	}
	//state가 MOVEB면 시계추처럼 회전
	if (m_iState == OBSTACLE_STATE::MOVEB)
	{
		vRot.z = sinf(XMConvertToRadians(m_fFrmSpeed));
	}
	Transform()->SetLocalRot(vRot);
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

