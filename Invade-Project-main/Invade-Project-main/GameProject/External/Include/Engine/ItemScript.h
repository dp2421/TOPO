#include "Script.h"


class CItemScript : public CScript
{
private:
    int m_iDir;
    float m_fSpeed;
    ITEM_STATE m_iState;
    CGameObject* m_pParticle;

    int m_coinIdx; //�ƹ��� �ȸԾ��� �� : -1 | �����ִ����� : 1 | �ǵڿ��ִ����� : 0
    bool m_isCoinColl=false;
public:
    CItemScript();
    virtual ~CItemScript();
    virtual void Awake();
    virtual void Update();
    void CoinRotate();
    void SetState(ITEM_STATE _iState) { m_iState = _iState; }
    void CoinIdx(int idx) { m_coinIdx = idx; }
    void SetCollCoin(int idx);
    void removeCoin(bool iscoincoll);

    ITEM_STATE GetState() { return m_iState; }

    CLONE(CItemScript);
};

