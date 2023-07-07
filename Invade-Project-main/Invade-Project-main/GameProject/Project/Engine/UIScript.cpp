#include "pch.h"
#include "Transform.h"
#include "UIScript.h"

void CUIScript::Update()
{
	mousepos = Vec3(540 - CKeyMgr::GetInst()->GetMousePos().x, 540 + 310 - CKeyMgr::GetInst()->GetMousePos().y, 0);
	if (Transform()->IsCasting(mousepos) && CKeyMgr::GetInst()->GetClicked() == true)
		m_isClicked = true;
	std::cout << "UI Type: " << m_iType << std::endl;

	if (m_iType = SELECT_BUTTON)
	{
		m_isClicked = false;
	}
	else if (m_iType = MODE_RACING)
	{
		CSceneMgr::GetInst()->ChangeScene(SCENE_TYPE::RACING);
		m_isClicked = false;

	}
	else if (m_iType = MODE_SURVIVAL)
	{
		CSceneMgr::GetInst()->ChangeScene(SCENE_TYPE::METOR);
		m_isClicked = false;
	}

	if (m_isClicked == true)
	{


	}
}

CUIScript::CUIScript() :CScript((UINT)SCRIPT_TYPE::UISCRIPT)
{
	m_isClicked = false;
}

CUIScript::~CUIScript()
{
}
