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

enum NUMPOS
{
    NONE,
    ONE,
    TEN,
    HUNDRED,
};
struct Numbers {
    NUM_TYPE type;
    int index;
    NUMPOS numpos;
};

class CNumScript : public CScript
{
private:
    bool m_isClicked;
    bool m_isSelected;
    bool m_isMatching;

    NUM_TYPE m_iNum;

    Vec3 mousepos;
    float f_Count = 0.f;
    Numbers m_Numinfo;
    int f_startCountdown = -1;


public:
    CNumScript();
    virtual ~CNumScript();
    virtual void Update();
    void NumberUpdate();
    void NumScript(int num, float offsetx, float offsety);
    void CountDown(int cnt);
    void SetNums(NUM_TYPE iNum, int index) { m_Numinfo.type = iNum; m_Numinfo.index = index; }
    Numbers GetNumInfo() { return m_Numinfo; }
    void SetNumInfo(NUM_TYPE type, int index, NUMPOS pos) { m_Numinfo.type = type; m_Numinfo.index = index; m_Numinfo.numpos = pos; }

    void SetNum(NUM_TYPE iNum) { m_iNum = iNum; }
    bool IsNumberActive(int type, int index);
    bool IsPrevNumActive(int type, int index, NUMPOS pos);

    NUM_TYPE GetNum() { return m_iNum; }


    void CheckNum(int num);

    void SetCount(float count) { f_Count = count; }
    float GetCount() { return f_Count; }


    CLONE(CNumScript);
};

