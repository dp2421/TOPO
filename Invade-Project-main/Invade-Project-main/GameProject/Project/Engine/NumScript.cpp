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
	f_Count += CTimeMgr::GetInst()->GetDeltaTime();
	int hundred = (int)f_Count / 100;
	int tens = (int)f_Count % 100 / 10;
	int one = (int)f_Count % 10;
	if (curObj->GetScript<CNumScript>()->GetNumInfo().type == hundred ||
		curObj->GetScript<CNumScript>()->GetNumInfo().type == tens ||
		curObj->GetScript<CNumScript>()->GetNumInfo().type == one)
	{
		curObj->GetScript<CNumScript>()->NumScript((int)f_Count, 0, pos.y / 1.75);
	}
}

void CNumScript::NumberUpdate()
{
	CGameObject* curObj = GetObj();
	Vec2 pos = CGameFramework::GetInst()->m_WinSize;

	if (f_Count > 1000)
		f_Count = 0;

	NumScript((int)f_Count, 0, pos.y / 1.75);
}

bool CNumScript::IsNumberActive(int type, int index)
{
	for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
	{
		if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::NUMBER &&
			obj->GetScript<CNumScript>()->m_Numinfo.type == type &&
			obj->GetScript<CNumScript>()->m_Numinfo.index == index &&
			obj->GetScript<CNumScript>()->m_Numinfo.numpos != NONE)
		{
			return true;
		}
	}

	return false;
}

bool CNumScript::IsPrevNumActive(int type, int index, NUMPOS pos)
{
	if (index < 0)
	{
		return true;
	}
	for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
	{
		if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::NUMBER &&
			obj->GetScript<CNumScript>()->m_Numinfo.type == type &&
			obj->GetScript<CNumScript>()->m_Numinfo.index == index &&
			obj->GetScript<CNumScript>()->m_Numinfo.numpos == pos)
		{
			return true;
		}
	}

	return false;
}


void CNumScript::NumScript(int num, float offsetx, float offsety)
{
	//for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
	//{
	//	if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::NUMBER)
	//	{
	//		obj->SetActive(false);
	//	}
	//}
	//std::cout << num << std::endl;
	int hundred = num / 100;
	int tens = num % 100 / 10;
	int one = num % 10;
	CGameObject* numObj = GetObj();

	if (m_Numinfo.type == hundred && !IsNumberActive(hundred, m_Numinfo.index) && !IsPrevNumActive(hundred, m_Numinfo.index - 1, HUNDRED))
	{
		GetObj()->SetActive(true);
		Transform()->SetLocalPos(Vec3(offsetx + 50.f, offsety, 0));
		m_Numinfo.numpos = HUNDRED;

		return;
	}
	if (m_Numinfo.type == tens && !IsNumberActive(tens, m_Numinfo.index) && !IsPrevNumActive(tens, m_Numinfo.index - 1, TEN))
	{
		GetObj()->SetActive(true);
		Transform()->SetLocalPos(Vec3(offsetx, offsety, 0));
		m_Numinfo.numpos = TEN;
		return;
	}
	if (m_Numinfo.type == one && !IsNumberActive(one, m_Numinfo.index) && !IsPrevNumActive(one, m_Numinfo.index - 1, ONE))
	{
		GetObj()->SetActive(true);
		Transform()->SetLocalPos(Vec3(offsetx - 50.f, offsety, 0));
		m_Numinfo.numpos = ONE;
		return;
	}
}

void CNumScript::CountDown(int cnt)
{
	if (cnt == 0)
	{
		GetObj()->SetActive(false);
	}
	else
	{
		std::cout << cnt << std::endl;
		if (cnt == m_Numinfo.type)
		{
			GetObj()->Transform()->SetLocalPos(Vec3(0, 580, 10));
			GetObj()->SetActive(true);
		}
		else
			GetObj()->SetActive(false);
	}
}

CNumScript::CNumScript() : CScript((UINT)SCRIPT_TYPE::NUMSCRIPT)
{
	m_isClicked = false;
	m_isSelected = false;
}

CNumScript::~CNumScript()
{
}
