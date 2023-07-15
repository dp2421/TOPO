#include "pch.h"
#include "Transform.h"
#include "RenderMgr.h"
#include "Camera.h"
#include "UIScript.h"

void CUIScript::Update()
{
	mousepos = Vec3(540 - CKeyMgr::GetInst()->GetMousePos().x, 540 + 310 - CKeyMgr::GetInst()->GetMousePos().y, 0);
	if (Transform()->IsCasting(mousepos) && CKeyMgr::GetInst()->GetClicked() == true)
		m_isClicked = true;

	if (m_isClicked == true)
	{
		if (m_iType == SELECT_BUTTON)
		{
			m_isSelected = true;
			CGameObject* curobj = GetObj();
			curobj->SetActive(false);
		}
		else if (m_iType == MODE_RACING)
		{
			//CRenderMgr::GetInst()->SetSceneChanged(true);
			//CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::RACING);
			m_isMatching = true;
		}
		else if (m_iType == MODE_SURVIVAL)
		{
			//CRenderMgr::GetInst()->SetSceneChanged(true);
			//CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::METOR);
			m_isMatching = true;
		}
		m_isClicked = false;

	}

	if (m_isMatching)
	{
		if (m_iType == MODE_SURVIVAL || m_iType == MODE_RACING || m_iType == WINDOW)
		{
			CGameObject* activeobj = GetObj();
			activeobj->SetActive(false);
		}
	}
}

void CUIScript::UIRender()
{
	CGameObject* curObj = GetObj();
	if (curObj->GetName() == (L"Cursor Object"))
	{
		Vec3 mousepos = Vec3(540 - CKeyMgr::GetInst()->GetMousePos().x, 540 + 310 - CKeyMgr::GetInst()->GetMousePos().y, 0);
		curObj->Transform()->SetLocalPos(Vec3(mousepos.x, mousepos.y, 0));
	}

	if (m_isSelected == true)
	{
		for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
		{
			if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::MODE_SURVIVAL ||
				obj->GetScript<CUIScript>()->GetType() == UI_TYPE::MODE_RACING ||
				obj->GetScript<CUIScript>()->GetType() == UI_TYPE::WINDOW)
			{
				obj->SetActive(true);
			}
		}
		m_isSelected = false;
	}
	if (m_isMatching == true)
	{
		m_isClicked = false;
		f_MatchingTime += CTimeMgr::GetInst()->GetDeltaTime()/0.75f;
		std::cout << f_MatchingTime << std::endl;
		for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
		{
			if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::MATCHING)
			{
				if (f_MatchingTime < 10.f)
					obj->SetActive(true);
				else
				{
					obj->SetActive(false);
					f_MatchingTime = 0.f;
					m_isMatching = false;
				}
			}

			if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::MODE_SURVIVAL ||
				obj->GetScript<CUIScript>()->GetType() == UI_TYPE::MODE_RACING ||
				obj->GetScript<CUIScript>()->GetType() == UI_TYPE::WINDOW)
			{
				obj->SetActive(false);
			}
		}
	}
	if (curObj->GetScript<CUIScript>()->GetType() == UI_TYPE::NUMBER)
	{
		int hundred = 34 / 100;
		int tens = 34 % 100 / 10;
		int one = 34 % 10;
		if (curObj->GetScript<CUIScript>()->GetNum() == hundred ||
			curObj->GetScript<CUIScript>()->GetNum() == tens ||
			curObj->GetScript<CUIScript>()->GetNum() == one)
		{
			curObj->SetActive(true);
			curObj->GetScript<CUIScript>()->NumScript(34, -100.f, 700);
		}

	}
}

void CUIScript::NumScript(int num, float offsetx, float offsety)
{
	int hundred = num / 100;
	int tens = num % 100/10;
	int one = num % 10;
	CGameObject* numObj = GetObj();
	if (m_iNum == hundred)
	{
		numObj->Transform()->SetLocalPos(Vec3(offsetx+50.f, offsety, 0));
	} 
	if (m_iNum == tens)
	{
		numObj->Transform()->SetLocalPos(Vec3(offsetx, offsety, 0));
	}
	if (m_iNum == one)
	{
		numObj->Transform()->SetLocalPos(Vec3(offsetx-50.f, offsety, 0));
	}

}

CUIScript::CUIScript() :CScript((UINT)SCRIPT_TYPE::UISCRIPT)
{
	m_isClicked = false;
	m_isSelected = false;
}

CUIScript::~CUIScript()
{
}
