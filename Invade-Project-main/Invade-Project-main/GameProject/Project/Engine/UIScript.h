#pragma once
#include "Script.h"
#include "GameFramework.h"

enum UI_TYPE
{
    CURSOR,
    WINDOW,
    SELECT_BUTTON,
    MODE_RACING,
    MODE_SURVIVAL,
    MATCHING,
    MATCHING0,
    MATCHING1,
    MATCHING2,
    MATCHING3,
    NUMBER,
}; //MATCHING0-MATCHING3 은 무조건 6-9번 이여야 함

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

class CUIScript : public CScript
{
private:
    bool m_isClicked;
    bool m_isSelected;
    bool m_isMatching;

    NUM_TYPE m_iNum;
    UI_TYPE m_iType;

    Vec3 mousepos;
    float f_MatchingTime = 0.f;
 

public:
    CUIScript();
    virtual ~CUIScript();
    virtual void Update();
    void UIRender();
    void MatchingComplete();
    void NumScript(int num, float offsetx, float offsety);
    void SetType(UI_TYPE _iState) { m_iType = _iState; }
    UI_TYPE GetType() { 
        return m_iType;
    }

    void SetNum(NUM_TYPE iNum) { m_iNum = iNum; }
    NUM_TYPE GetNum() { return m_iNum; }
    CLONE(CUIScript);
};

