#include "pch.h"
#include "AwardScript.h"
#include "NetworkMgr.h"

void CAwardScript::Awake()
{
}

void CAwardScript::Update()
{
	for(int i=0; i<3; ++i)
		RankedPlayer(i);
}

CAwardScript::CAwardScript() :CScript((UINT)SCRIPT_TYPE::AWARDSCRIPT)
{
}

CAwardScript::~CAwardScript()
{
}

void CAwardScript::RankedPlayer(int idx)
{
	//if (NetworkMgr::GetInst()->rankPLID[0] != -1 && m_ranking == RANK_TYPE::RANK1)
	
	//1�� ���� ���� ����
	if (m_playerIdx == idx && NetworkMgr::GetInst()->rankPLID[idx] == -1) //-1�̸�ƹ������°�
	{
		GetObj()->SetActive(false);
	}

}
