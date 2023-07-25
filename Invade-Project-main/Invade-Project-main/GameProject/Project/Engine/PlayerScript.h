#pragma once
#include "Script.h"

enum class ELEMENT_TYPE {
    FROZEN=0,
    FIRE=1,
    DARK=2,
    THUNDER=3,
    WIND=4
};

enum class PARTICLE_TYPE {
    COLLPARICLE=0,
    RUNPARTICLE
};

enum class Direction : int
{
    None = 0,
    Front = 1 << 0,
    Back = 1 << 1,
    Left = 1 << 2,
    Right = 1 << 3,
    END
};

class CTexture;
class CPlayerScript :
    public CScript
{
private:

    bool m_bCheckStartMousePoint;
    Vec3 m_vZoomPos;
    Vec3 m_vRestorePos;
    Vec3 prePosition;
    CGameObject* m_pArrow[20];
    UINT m_iCurArrow;
    Ptr<CTexture>pBlackTex;
    int m_iPower;
    ELEMENT_TYPE m_iType;
    PLAYER_STATE m_iState;
    float m_fArrowSpeed;

    float m_fArcherLocation;

    int moveState = 0;
    bool isPlayable = false;
    bool isColl = false;
    bool isGoal = false;

    bool m_isColl = false; //부딪혔을때 나오는 파티클
    bool m_isRun = true;  //뛸 때 나오는 파티클
    int testCount = 0;

    CGameObject* m_pParticle;
public:
    CGameObject* runPlayer;
    CGameObject* IdlePlayer;

    virtual void Awake();
    virtual void Update();
    void SetType(ELEMENT_TYPE _iType) { m_iType = _iType; }
    void SetState(PLAYER_STATE _iState) { m_iState = _iState; }
    PLAYER_STATE GetState() { return m_iState; }
    void SetPlayable(bool value);
    bool GetPlayable() { return isPlayable; } const

    void SetPlayerMoveState(KEY_TYPE key, KEY_STATE state, Vec3& dir);
    void SetPlayerPos(
        Vec3 pos,
        float degree = 0,
        bool isMove = false,
        bool isColl = false,
        bool isGoal = false
    );
    void StartParticle(Vec3 pos, PARTICLE_TYPE type);
    void EndParticle();

    CPlayerScript();
    virtual ~CPlayerScript();

    void SetRunPlayer(CGameObject* obj) { 
        runPlayer = obj; 
    }
    void SetIdlePlayer(CGameObject* obj) { 
        IdlePlayer = obj; 
    }

    CLONE(CPlayerScript);
};

