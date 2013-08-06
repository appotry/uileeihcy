#include "stdafx.h"
#include "contactmgrdlg.h"

CContactMgrDlg::CContactMgrDlg()
{
    m_pListView = NULL;
}
CContactMgrDlg::~CContactMgrDlg()
{

}

void CContactMgrDlg::OnInitWindow()
{
    ICustomWindow::xProcessMessage(GetCurMsg(), 0, 0);
    this->SetWindowResizeType(WRSB_ALL);

    m_pListView = (UI::IListView*)FindChildObject(_T("listivew"));
    if (NULL == GetMaterialRender())
    {
        SetMaterialRender(g_pShareSkinRender);
    }
}

void  CContactMgrDlg::OnSysCommand(UINT nID, CPoint point)
{
    SetMsgHandled(FALSE);

    if (nID == SC_CLOSE)
    {
        SetMsgHandled(TRUE);
        HideWindow();
        return;
    }
}   

void  CContactMgrDlg::InitListView()
{
    if (!m_pListView)
        return;

//    m_pListView->InsertColumn()
}
