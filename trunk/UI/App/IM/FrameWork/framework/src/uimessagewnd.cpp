#include "stdafx.h"
#include "uimessagewnd.h"
#include "framework.h"
#include "App\IM\include\task.h"

CUIMessageWnd::CUIMessageWnd(CFramework* p)
{
    m_pFramework = p;
}

LRESULT  CUIMessageWnd::OnTaskMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    IM::ITask* pTask = (IM::ITask*)wParam;
    m_pFramework->OnTask(pTask, lParam);

    return 0;
}

LRESULT  CUIMessageWnd::OnFunctionMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    IM::fun_pkg* p = (IM::fun_pkg*)wParam;
    m_pFramework->OnFunction(p, lParam);

    return 0;
}