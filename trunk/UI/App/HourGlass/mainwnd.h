#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "settingwnd.h"
#include "maskwnd.h"

namespace UI
{
    interface ILEDExCtrl;
    interface IButton;
    interface IEdit;
    interface ICheckButton;
}
class CMainWnd : public UI::ICustomWindow
{
public:
    CMainWnd();
    ~CMainWnd();

    UI_BEGIN_MSG_MAP_Ixxx(CMainWnd)
        UIMSG_WM_TIMER(OnTimer)
        UIMSG_WM_CONTEXTMENU(OnContextMenu)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

public:
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnInitialize();
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  OnDestroy();
    void  OnContextMenu(HWND hWnd, POINT point);

public:
    void  SetTotalTime(int  nHour, int nMinute, int nSecond);
    void  OnRing();
    void  Reset();

private:
    UI::ILEDExCtrl*  m_pLEDCtrl;
    UINT  m_nTimerId;
    int   m_nStartTickCount;
    int   m_nTotalMS;
};

