#pragma once
#include "Script.h"

class CObstacleScript : public CScript
{
private:
    int m_iDir;
    float m_fSpeed;
    OBSTACLE_STATE m_iState;

public:
    virtual void Update();

    CObstacleScript();
    virtual ~CObstacleScript();

    void SetState(OBSTACLE_STATE _iState) { m_iState = _iState; }
    OBSTACLE_STATE GetState() { return m_iState; }

    void Rotate(float value);

    CLONE(CObstacleScript);
};
