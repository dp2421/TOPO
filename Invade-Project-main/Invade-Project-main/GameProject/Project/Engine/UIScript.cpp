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
			m_isSelected = true;
		}
		else if (m_iType == MODE_RACING)
		{
			CRenderMgr::GetInst()->SetSceneChanged(true);
			CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::RACING);

		}
		else if (m_iType == MODE_SURVIVAL)
		{
			CRenderMgr::GetInst()->SetSceneChanged(true);
			CRenderMgr::GetInst()->SetSceneType(SCENE_TYPE::METOR);
		}
		else if (m_iType == MATCHING)
		{

		}
		m_isClicked = false;

	}
	if (m_isSelected)
	{
		if (m_iType == MODE_SURVIVAL || m_iType == MODE_RACING || m_iType == WINDOW)
		{
			CGameObject* activeobj = GetObj();
			activeobj->SetActive(true);
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
