#include "Script.h"

enum class RANK_TYPE {
    RANK1,
    RANK2,
    RANK3
};

class CAwardScript : public CScript
{
private:
    RANK_TYPE m_ranking;
    int m_playerIdx; //1�� : 0 | 2�� : 1 | 3�� : 2

public:
    CAwardScript();
    virtual ~CAwardScript();
    virtual void Awake();
    virtual void Update();

    void AwardPlayerIdx(int idx) { m_playerIdx = idx; }
    void SetRanking(RANK_TYPE _iState) { m_ranking = _iState; }

    void RankedPlayer();

    CLONE(CAwardScript);
};

