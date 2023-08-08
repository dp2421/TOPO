#include "pch.h"
#include "ObstacleScript.h"
#include "TimeMgr.h"

void CObstacleScript::Update()
{
	Vec3 vRot = Transform()->GetLocalRot();
	float fFps = CTimeMgr::GetInst()->GetFPS();

	//��ó���� ������ ���� �� Ȯ Ƣ�� �� ����
	if (fFps < 30.0f)
	{
		fFps = 30.0f;
	}
	m_fFrmSpeed += m_fSpeed / fFps;

	if (m_fFrmSpeed > 360)
		m_fFrmSpeed -= 360.f;

	//state �� MOVE�̸� 360�� ȸ��
	if (m_iState == OBSTACLE_STATE::MOVEA)
	{
		vRot.y = XMConvertToRadians(m_fFrmSpeed);
	}
	//state�� MOVEB�� �ð���ó�� ȸ��
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

