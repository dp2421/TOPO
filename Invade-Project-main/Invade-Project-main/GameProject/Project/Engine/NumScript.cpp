#include "pch.h"
#include "NetworkMgr.h"
#include "Transform.h"
#include "RenderMgr.h"
#include "Camera.h"
#include "NumScript.h"

void CNumScript::Update()
{
	CGameObject* curObj = GetObj();
	Vec2 pos = CGameFramework::GetInst()->m_WinSize;
	f_MatchingTime += CTimeMgr::GetInst()->GetDeltaTime();
	int hundred = (int)f_MatchingTime / 100;
	int tens = (int)f_MatchingTime % 100 / 10;
	int one = (int)f_MatchingTime % 10;
	if (curObj->GetScript<CNumScript>()->GetNumInfo().type == hundred ||
		curObj->GetScript<CNumScript>()->GetNumInfo().type == tens ||
		curObj->GetScript<CNumScript>()->GetNumInfo().type == one)
	{
		curObj->GetScript<CNumScript>()->NumScript((int)f_MatchingTime, 0, pos.y / 1.75);
	}


}


void CNumScript::NumScript(int num, float offsetx, float offsety)
{
	int hundred = num / 100;
	int tens = num % 100 / 10;
	int one = num % 10;
	CGameObject* numObj = GetObj();
	for (int i = 0; i < 5; ++i)
	{
		if (numObj->GetScript<CNumScript>()->GetNumInfo().index == i)
		{
			if (numObj->IsActive() == false)
			{
				if (m_iNum == hundred)
				{
					numObj->Transform()->SetLocalPos(Vec3(offsetx + 50.f, offsety, 0));
					numObj->SetActive(true);

				}
				if (m_iNum == tens)
				{
					numObj->Transform()->SetLocalPos(Vec3(offsetx, offsety, 0));
					numObj->SetActive(true);

				}
				if (m_iNum == one)
				{
					numObj->Transform()->SetLocalPos(Vec3(offsetx - 50.f, offsety, 0));
					numObj->SetActive(true);

				}
		}
		}
	}

}

CNumScript::CNumScript(): CUIScript()
{
	m_isClicked = false;
	m_isSelected = false;
}

CNumScript::~CNumScript()
{
}
