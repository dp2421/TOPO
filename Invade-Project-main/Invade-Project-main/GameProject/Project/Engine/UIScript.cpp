#include "pch.h"
#include "Transform.h"
#include "RenderMgr.h"
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
			m_isClicked = false;
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
	}

}

CUIScript::CUIScript() :CScript((UINT)SCRIPT_TYPE::UISCRIPT)
{
	m_isClicked = false;
}

CUIScript::~CUIScript()
{
}
