#pragma once
#include "Script.h"
#include "UIScript.h"
#include "GameFramework.h"

enum NUM_TYPE
{
    NUM_0,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
};

struct Numbers {
    NUM_TYPE type;
    int index;
};

class CNumScript : public CScript
{
private:
    bool m_isClicked;
    bool m_isSelected;
    bool m_isMatching;

    NUM_TYPE m_iNum;

    Vec3 mousepos;
    float f_MatchingTime = 0.f;
    Numbers m_Numinfo;

public:
    CNumScript();
    virtual ~CNumScript();
    virtual void Update();
    void NumberUpdate();
    void NumScript(int num, float offsetx, float offsety);
    void SetNums(NUM_TYPE iNum, int index) { m_Numinfo.type = iNum; m_Numinfo.index = index; }
    Numbers GetNumInfo() { return m_Numinfo; }
    void SetNumInfo(NUM_TYPE type, int index) { m_Numinfo.type = type; m_Numinfo.index = index; }

    void SetNum(NUM_TYPE iNum) { m_iNum = iNum; }
    NUM_TYPE GetNum() { return m_iNum; }
    CLONE(CNumScript);
};

