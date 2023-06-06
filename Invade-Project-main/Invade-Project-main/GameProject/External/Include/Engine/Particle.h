#pragma once
#include "Script.h"

class CParticle : public CScript
{
private:
    int m_iDir;
    float m_fSpeed;
    CGameObject* m_pParticle;

public:
    CParticle();
    virtual ~CParticle();

    virtual void Awake();
    virtual void Update();
    //void SetType(UINT _iType) { m_iType = (ELEMENT_TYPE)_iType; }
    void Init();

    CLONE(CParticle);
};

