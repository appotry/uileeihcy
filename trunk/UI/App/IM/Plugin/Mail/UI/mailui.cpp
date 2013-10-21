#include "stdafx.h"
#include "mailui.h"

long  CMailUI::ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam)
{
    if (IM::BASE_CMD_ID_INIT == nCommand)
    {
        IM::IMainUI*  pMainUI = (IM::IMainUI*)IM::GetFramework()->GetUI(MODULE_ID_MAIN);
        if (pMainUI)
        {
            pMainUI->AddToolbarPlugin(&m_btn);
        }
    }
    return 0;
}