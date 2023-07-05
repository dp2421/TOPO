#pragma once
#include "Script.h"
#include "TimeMgr.h"


enum TargetMap {
    WATER,
    GRASS,
    STONE,
    WOOD,
    CENTER,
    END
};

enum MAP_TYPE
{
    GROUND,
    METEOR
};

class MeteorScript : public CScript
{
private:
    MAP_TYPE m_iType;

    Vec3 OriginPos = Vec3(1000.f, 5000.f, 1000.f);
    Vec3 m_TargetPos[TargetMap::END];
    TargetMap m_curTarget = TargetMap::WATER;

    Vec3 GroundPos;

    int m_iDir;
    float m_fSpeed = 0.01f;
    float m_fFrmSpeed;
    float m_Time = 0.f;

public:
    virtual void Update();

    MeteorScript();
    virtual ~MeteorScript();

    void SetSpeed(float speed) { m_fSpeed = speed; }
    float GetSpeed() { return m_fSpeed; }


    void SetType(MAP_TYPE type) { m_iType = type; }
    void Rotate(float value);

    CLONE(MeteorScript);
};

