#include "pch.h"
#include "ItemScript.h"
#include "ParticleSystem.h"

void CItemScript::Awake()
{
	////파티클
	//CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	//m_pParticle = new CGameObject;
	//m_pParticle->SetName(L"Particle");
	//m_pParticle->AddComponent(new CTransform);
	//m_pParticle->AddComponent(new CParticleSystem);
	//m_pParticle->ParticleSystem()->Init(CResMgr::GetInst()->FindRes<CTexture>(L"Snow"));
	//m_pParticle->ParticleSystem()->SetStartColor(Vec4(0.8f, 1.0f, 0.5f, 1.f));//,m_vStartColor(Vec4(0.4f,0.4f,0.8f,1.4f)),m_vEndColor(Vec4(1.f,1.f,1.f,1.0f))
	//m_pParticle->ParticleSystem()->SetEndColor(Vec4(0.8f, 1.f, 1.0f, 1.0f));
	//m_pParticle->ParticleSystem()->SetMaxLifeTime(10.0f);
	//m_pParticle->ParticleSystem()->SetMinLifeTime(1.0f);
	//m_pParticle->ParticleSystem()->SetStartScale(10.f);
	//m_pParticle->ParticleSystem()->SetEndScale(6.f);
	//
	//if (m_iState == ITEM_STATE::SUPERJUMP)
	//{
	//	m_pParticle->FrustumCheck(false);
	//	m_pParticle->Transform()->SetLocalPos(Vec3(0.5f, 100.0f, 0.f));
	//	pCurScene->FindLayer(L"Default")->AddGameObject(m_pParticle, pCurScene);
	//	GetObj()->AddChild(m_pParticle);
	//}
}

void CItemScript::Update()
{
	if(!m_isCoinColl)
		CoinRotate();
	removeCoin(m_isCoinColl);

	//Vec3 pos = Transform()->GetLocalPos();
	////슈점파티클
	//if (m_iState == ITEM_STATE::SUPERJUMP)
	//{
	//	for (auto obj : CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->GetObjects())
	//	{
	//		if (obj->GetName().compare(L"SuJumParticle") == 0)
	//		{
	//			obj->Transform()->SetLocalPos(Vec3(pos.x + 0.5f, pos.y + 100.0f, pos.z));
	//			break;
	//		}
	//	}
	//}
}

CItemScript::CItemScript() :CScript((UINT)SCRIPT_TYPE::ITEMSCRIPT), m_iDir(1), m_coinIdx(-1), m_isCoinColl(false)
{
}

CItemScript::~CItemScript()
{
}

void CItemScript::CoinRotate()
{
	//코인 아이템 제자리 360도 회전
	if (m_iState == ITEM_STATE::COIN)
	{
		Vec3 vRot = Transform()->GetLocalRot();
		m_fSpeed += 5.f;
		vRot.y = XMConvertToRadians(m_fSpeed);
		//float fDegree = XMConvertToDegrees(m_fSpeed);
		if (m_fSpeed > 360) {
			m_fSpeed -= 360.f;
		}
		Transform()->SetLocalRot(vRot);
	}
}

void CItemScript::SetCollCoin(int idx)
{
	std::cout << "==========================================" << std::endl;
	std::cout << "충돌코인 : " << idx << std::endl;
	if (m_coinIdx == idx) 
		m_isCoinColl = true; 
}

void CItemScript::removeCoin(bool iscoincoll)
{
	
	if(iscoincoll)
		GetObj()->SetActive(false);
	else
	{
		//..
	}
}
