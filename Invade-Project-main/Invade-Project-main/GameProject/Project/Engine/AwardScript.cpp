#include "pch.h"
#include "AwardScript.h"

void CAwardScript::Awake()
{
}

void CAwardScript::Update()
{
	RankedPlayer();
}

CAwardScript::CAwardScript() :CScript((UINT)SCRIPT_TYPE::AWARDSCRIPT)
{
}

CAwardScript::~CAwardScript()
{
}

void CAwardScript::RankedPlayer()
{
	//if (NetworkMgr::GetInst()->rankPLID[0] != -1 && m_ranking == RANK_TYPE::RANK1)
	if (m_ranking == RANK_TYPE::RANK1)
	{
		GetObj()->SetActive(true);
	}

	//switch (NetworkMgr::GetInst()->rankPLID[i])
	//{
	//case 0: //1��
	//	SetPlayerPos(Vec3(0.f, 10.f + 350.f, -200.f));
	//	break;
	//case 1: //2��
	//	SetPlayerPos(Vec3(475.f, 10.f + 175.f, -125.f));
	//	break;
	//case 2: //3��
	//	SetPlayerPos(Vec3(-475.f, 10.f + 175.f, -125.f));
	//	break;
	//default: //�й��ڵ� : -1���Ѿ�ö�
	//	SetPlayerPos(Vec3(0.f, 10.f + 350.f, -780.f));
	//	break;
	//}


}
