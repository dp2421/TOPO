#pragma once
#include "Script.h"
#include "TimeMgr.h"
#include "Tile.h"


enum GROUND_TYPE {
    CENTER,
    WOOD,
    WATER,
    STONE,
    GRASS,
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
    MAP_TYPE m_iType; // 맵인지 메테오인지
    GROUND_TYPE m_iMapType; // 맵 중 속성

    Vec3 OriginPos = Vec3(1000.f, 5000.f, 1000.f);
    Vec3 m_TargetPos[GROUND_TYPE::END];
    GROUND_TYPE m_curTarget = GROUND_TYPE::WATER;

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
    void SetMapType(GROUND_TYPE type) { m_iMapType = type; }
    void Rotate(float value);

    CLONE(MeteorScript);
};

