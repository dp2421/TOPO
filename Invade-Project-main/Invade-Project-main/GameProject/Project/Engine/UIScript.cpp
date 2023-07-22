#include "pch.h"
#include "NetworkMgr.h"
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
		Vec3 mousepos = Vec3(winsize.x / 1.3 - CKeyMgr::GetInst()->GetMousePos().x, winsize.y / 1.8 - CKeyMgr::GetInst()->GetMousePos().y, 0);
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
		f_MatchingTime += CTimeMgr::GetInst()->GetDeltaTime() / 0.75f;
		std::cout << f_MatchingTime << std::endl;
		for (CGameObject* obj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
		{
			if (obj->GetScript<CUIScript>()->GetType() == UI_TYPE::MATCHING)
			{
				if (CRenderMgr::GetInst()->GetMatchComplete()==false) // 매칭여부bool값으로 변경
				{
					obj->SetActive(true);
					for (CGameObject* Loadobj : CRenderMgr::GetInst()->GetCamera(1)->GetUIObj())
					{

						int temp = (int)f_MatchingTime + 6;
						if (temp > 9)
							temp -= 3;
						switch (temp)
						{
						case 6:
							if (Loadobj->GetScript<CUIScript>()->GetType() == 6)
							{
								Loadobj->SetActive(true);
							}
							else if (Loadobj->GetScript<CUIScript>()->GetType() == 7 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 8 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 9)
								Loadobj->SetActive(false);
							break;
						case 7:
							if (Loadobj->GetScript<CUIScript>()->GetType() == 7)
							{
								Loadobj->SetActive(true);
							}
							else if (Loadobj->GetScript<CUIScript>()->GetType() == 6 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 8 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 9)
								Loadobj->SetActive(false);

							break;
						case 8:
							if (Loadobj->GetScript<CUIScript>()->GetType() == 8)
							{
								Loadobj->SetActive(true);
							}
							else if (Loadobj->GetScript<CUIScript>()->GetType() == 7 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 6 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 9)
								Loadobj->SetActive(false);
							break;
						case 9:
							if (Loadobj->GetScript<CUIScript>()->GetType() == 9)
							{
								Loadobj->SetActive(true);
							}
							else if (Loadobj->GetScript<CUIScript>()->GetType() == 7 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 8 ||
								Loadobj->GetScript<CUIScript>()->GetType() == 6)
								Loadobj->SetActive(false);
							break;
						default:
							break;
						}

					}

				}
				else
				{
					obj->SetActive(false);
					f_MatchingTime = 0.f;
					CRenderMgr::GetInst()->SetMatchComplete(true);
					MatchingComplete();

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
		Vec2 pos = CGameFramework::GetInst()->m_WinSize;
		f_MatchingTime += CTimeMgr::GetInst()->GetDeltaTime();
		int hundred = (int)f_MatchingTime / 100;
		int tens = (int)f_MatchingTime % 100 / 10;
		int one = (int)f_MatchingTime % 10;
		if (curObj->GetScript<CUIScript>()->GetNum() == hundred ||
			curObj->GetScript<CUIScript>()->GetNum() == tens ||
			curObj->GetScript<CUIScript>()->GetNum() == one)
		{
			curObj->GetScript<CUIScript>()->NumScript((int)f_MatchingTime, 0, pos.y / 1.75);
		}

	}
}

void CUIScript::MatchingComplete()
{
	CRenderMgr::GetInst()->SetSceneChanged(true);
#if LOCALPLAY
	if (m_iType == MODE_RACING)
		CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::RACING);
	else if (m_iType == MODE_SURVIVAL)
	{
		int random = rand() % 2 + 1;
		CRenderMgr::GetInst()->SetSceneType((SCENE_TYPE)random);
	}
#else // !LOCALPLAY
	switch ((MapType)CRenderMgr::GetInst()->GetMatchMapType())
	{
	case MapType::Racing:
		CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::RACING);
		break;
	case MapType::Meteo:
		CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::METOR);
		break;
	case MapType::Obstacle:
		CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::JUMP);
		break;
	default:
		break;
	}
#endif
}

void CUIScript::NumScript(int num, float offsetx, float offsety)
{
	int hundred = num / 100;
	int tens = num % 100 / 10;
	int one = num % 10;
	CGameObject* numObj = GetObj();
	for (int i = 0; i < 5; ++i)
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

CUIScript::CUIScript() :CScript((UINT)SCRIPT_TYPE::UISCRIPT)
{
	m_isClicked = false;
	m_isSelected = false;
}

CUIScript::~CUIScript()
{
}
