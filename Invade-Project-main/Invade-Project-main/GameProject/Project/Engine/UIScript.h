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
    FEVER,
}; //MATCHING0-MATCHING3 은 무조건 6-9번 이여야 함


class CUIScript : public CScript
{
private:
    bool m_isClicked;
    bool m_isSelected;
    bool m_isMatching;
    bool m_isRoundOver;

    UI_TYPE m_iType;

    Vec3 mousepos;
    float f_MatchingTime = 0.f;
    float f_WaitFeverModeTime = 0.f; //약 3초 예정
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

