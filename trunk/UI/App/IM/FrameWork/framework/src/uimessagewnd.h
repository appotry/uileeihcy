#pragma once
#include <atlbase.h>
#include <atlwin.h>
class CFramework;

#define UI_MSG_TASK               (WM_USER)
#define UI_MSG_FUNCTION           (WM_USER+1)


// 用于UI线程接受其它线程的消息，如在业务线程中显示窗口
class CUIMessageWnd : public CWindowImpl<CUIMessageWnd>
{
public:
    CUIMessageWnd(CFramework* p);

    BEGIN_MSG_MAP(CUIMessageWnd)
        MESSAGE_HANDLER(UI_MSG_TASK, OnTaskMessage)
        MESSAGE_HANDLER(UI_MSG_FUNCTION, OnFunctionMessage)
    END_MSG_MAP()

    LRESULT  OnTaskMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  OnFunctionMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
    CFramework*  m_pFramework;
};