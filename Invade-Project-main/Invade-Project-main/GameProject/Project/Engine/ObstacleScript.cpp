#include "pch.h"
#include "ObstacleScript.h"


void CObstacleScript::Update()
{
	Vec3 vRot = Transform()->GetLocalRot();

	//state �� MOVE�̸� 360�� ȸ��
	if (m_iState == OBSTACLE_STATE::MOVE)
	{
		m_fSpeed += 5.f;
		vRot.y = XMConvertToRadians(m_fSpeed);
		//float fDegree = XMConvertToDegrees(m_fSpeed);
		if (m_fSpeed > 360) {
			m_fSpeed -= 360.f;
		}
	}
	//state�� MOVEB�� �ð���ó�� ȸ��
	else if (m_iState == OBSTACLE_STATE::MOVEB)
	{
		m_fSpeed += 5.f;
		vRot.z = sinf(XMConvertToRadians(m_fSpeed));
		//float fDegree = XMConvertToDegrees(m_fSpeed);
		if (m_fSpeed > 360) {
			m_fSpeed -= 360.f;
		}
	}

	Transform()->SetLocalRot(vRot);
}


CObstacleScript::CObstacleScript() :CScript((UINT)SCRIPT_TYPE::OBSTACLESCRIPT), m_iDir(1)
{
}

CObstacleScript::~CObstacleScript()
{
}

