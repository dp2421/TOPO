#include "pch.h"
#include "Transform.h"
#include "RenderMgr.h"
#include "Camera.h"
#include "UIScript.h"

void CUIScript::Update()
{
	Vec2 winsize = CGameFramework::GetInst()->m_WinSize;
	mousepos = Vec3(winsize.x / 1.3 - CKeyMgr::GetInst()->GetMousePos().x, winsize.y / 1.8 - CKeyMgr::GetInst()->GetMousePos().y, 0);
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
			m_isMatching = true;
		}
		else if (m_iType == MODE_SURVIVAL)
		{
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
		Vec2 winsize = CGameFramework::GetInst()->m_WinSize;
		Vec3 mousepos = Vec3(winsize.x/1.3 - CKeyMgr::GetInst()->GetMousePos().x, winsize.y/1.8  - CKeyMgr::GetInst()->GetMousePos().y, 0);
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
				if (f_MatchingTime < 4.f)
				{
					obj->SetActive(true);
					for (CGameObject* Loadobj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
					{
						if (Loadobj->GetScript<CUIScript>()->GetType() == (UI_TYPE)((int)f_MatchingTime+6))
						{
							Loadobj->SetActive(true);
							Vec3 vRot = Transform()->GetLocalRot();
							vRot.z = XMConvertToRadians(f_MatchingTime)*50;
							//Loadobj->Transform()->SetLocalRot(vRot);

						}
					}

				}
				else
				{
					obj->SetActive(false);
					f_MatchingTime = 0.f;
					m_isMatching = false;
					CRenderMgr::GetInst()->SetSceneChanged(true);
					if (m_iType == MODE_RACING)
						CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::RACING);
					else if (m_iType == MODE_SURVIVAL)
					{
						int random = rand() % 2 + 2;
						CRenderMgr::GetInst()->SetSceneType((SCENE_TYPE)random);
					}

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
