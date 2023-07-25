#pragma once
#include "Script.h"
#include "NumScript.h"
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
    ROUNDOVER,
}; //MATCHING0-MATCHING3 은 무조건 6-9번 이여야 함


class CUIScript : public CScript
{
private:
    bool m_isClicked;
    bool m_isSelected;
    bool m_isMatching;
    bool m_isRoundOver = false;

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
    CLONE(CUIScript);
};

