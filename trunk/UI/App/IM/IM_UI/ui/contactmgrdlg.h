#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
namespace UI
{
    interface IListView;
}

class CContactMgrDlg : public UI::ICustomWindow
{
public:
    CContactMgrDlg();
    ~CContactMgrDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CContactMgrDlg)
        UIMSG_WM_SYSCOMMAND(OnSysCommand)
        UIMSG_WM_INITIALIZE(OnInitWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

protected:
    void    OnInitWindow();
    void    OnSysCommand(UINT nID, CPoint point);


private:
    void  InitListView();

private:
    UI::IListView*  m_pListView;

};