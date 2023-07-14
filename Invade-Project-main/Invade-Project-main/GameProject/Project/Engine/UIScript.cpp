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
	std::cout << "UI Type: " << m_iType << std::endl;

	if (m_isClicked == true)
	{
		if (m_iType == SELECT_BUTTON)
		{
			CGameObject* curobj = GetObj();
			curobj->SetActive(false);
			CRenderMgr::GetInst()->GetCamera(1)->SetClicked(true);
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
		else if (m_iType == MATCHING)
		{
			// 시간 지나면 false 되도록

		}
		m_isClicked = false;

	}


	if (m_isMatching)
	{
		CRenderMgr::GetInst()->GetCamera(1)->SetMatching(true);
		if (m_iType == MODE_SURVIVAL || m_iType == MODE_RACING || m_iType == WINDOW)
		{
			CGameObject* activeobj = GetObj();
			activeobj->SetActive(false);
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
