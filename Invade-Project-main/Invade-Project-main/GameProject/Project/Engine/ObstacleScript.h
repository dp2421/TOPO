#pragma once
#include "Script.h"
#include "TimeMgr.h"

class CObstacleScript : public CScript
{
private:
    int m_iDir;
    float m_fSpeed;
    float m_fFrmSpeed;
    OBSTACLE_STATE m_iState;

public:
    virtual void Update();

    CObstacleScript();
    virtual ~CObstacleScript();

    void SetState(OBSTACLE_STATE _iState) { m_iState = _iState; }
    OBSTACLE_STATE GetState() { return m_iState; }

    void SetSpeed(float speed) { m_fSpeed = speed; }
    float GetSpeed() { return m_fSpeed; }

    void Rotate(float value);

    CLONE(CObstacleScript);
};

