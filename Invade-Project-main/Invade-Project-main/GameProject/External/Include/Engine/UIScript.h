#pragma once
#include "Script.h"

enum UI_TYPE
{
    WINDOW,
    SELECT_BUTTON,
    MODE_RACING,
    MODE_SURVIVAL,
};

class CUIScript : public CScript
{
private:
    bool m_isClicked;
    UI_TYPE m_iType;

    Vec3 mousepos;

public:
    CUIScript();
    virtual ~CUIScript();
    virtual void Update();
    void SetType(UI_TYPE _iState) { m_iType = _iState; }
    UI_TYPE GetType() { return m_iType; }

    CLONE(CUIScript);
};

