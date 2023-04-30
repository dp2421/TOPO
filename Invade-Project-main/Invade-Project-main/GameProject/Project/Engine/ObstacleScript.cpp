#include "pch.h"
#include "ObstacleScript.h"


void CObstacleScript::Update()
{

	//�Ʒ�ó�� ���̾��̸� �޾ƿͼ� ���� ó�����ٱ� �ƴϸ� �� �ƿ� �и��� �ұ�
	//if (L"ObstacleMove" == _pOther->GetObj()->GetName())

	Vec3 vRot = Transform()->GetLocalRot();
	m_fSpeed += 5.f;
	vRot.y = XMConvertToRadians(m_fSpeed);
	//float fDegree = XMConvertToDegrees(m_fSpeed);
	if (m_fSpeed > 360) {
		m_fSpeed -= 360.f;
	}
	Transform()->SetLocalRot(vRot);
}


CObstacleScript::CObstacleScript() :CScript((UINT)SCRIPT_TYPE::OBSTACLESCRIPT), m_iDir(1)
{
}

CObstacleScript::~CObstacleScript()
{
}

