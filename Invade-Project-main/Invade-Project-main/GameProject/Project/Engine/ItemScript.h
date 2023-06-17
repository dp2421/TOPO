#include "Script.h"


class CItemScript : public CScript
{
private:
    int m_iDir;
    float m_fSpeed;
    ITEM_STATE m_iState;
    CGameObject* m_pParticle;

public:
    CItemScript();
    virtual ~CItemScript();
    virtual void Awake();
    virtual void Update();

    void SetState(ITEM_STATE _iState) { m_iState = _iState; }
    ITEM_STATE GetState() { return m_iState; }

    CLONE(CItemScript);
};

