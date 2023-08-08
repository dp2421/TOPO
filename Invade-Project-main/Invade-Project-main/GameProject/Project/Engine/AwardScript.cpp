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
	
	//1등 유무 끌지 말지
	if (m_playerIdx == idx && NetworkMgr::GetInst()->rankPLID[idx] == -1) //-1이면아무도없는거
	{
		GetObj()->SetActive(false);
	}

}
