#pragma once
#include "App/IM/include/framework_inc.h"
#include "mailbtn.h"

class CMailUI : public IM::IUI
{
public:
    virtual long ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);


private:
    MailBtn  m_btn;
};