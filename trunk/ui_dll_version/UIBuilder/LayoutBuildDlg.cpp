#include "StdAfx.h"
#include "LayoutBuildDlg.h"
#include "Dialogs.h"


CLayoutCodeDlg::CLayoutCodeDlg() : CResourceCodeDlg(UI_RESOURCE_LAYOUT)
{
}
CLayoutCodeDlg::~CLayoutCodeDlg()
{
}


CLayoutBuildDlg::CLayoutBuildDlg()
{
	m_pStyleCodePanel = NULL;
	m_hSkin = NULL;
}

CLayoutBuildDlg::~CLayoutBuildDlg()
{
	if( NULL != m_pStyleCodePanel )
	{
		delete m_pStyleCodePanel;
		m_pStyleCodePanel = NULL;
	}
	m_hSkin = NULL;
}

BOOL CLayoutBuildDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_tabCtrl.Attach(GetDlgItem(IDC_TAB));
//	m_tabCtrl.InsertItem(0, _T("view") );
//	m_tabCtrl.InsertItem(1, _T("code") );
	m_tabCtrl.InsertItem(0, _T("code") );

	m_pStyleCodePanel = new CLayoutCodeDlg;
	m_pStyleCodePanel->Create(m_tabCtrl.m_hWnd);

	m_pStyleCodePanel->ShowWindow(SW_SHOW);
	return TRUE;
}
void CLayoutBuildDlg::OnSize(UINT nType, CSize size)
{
	if ( NULL == m_pStyleCodePanel )
		return;

	RECT rc = {WINDOW_PADDING,WINDOW_PADDING,size.cx-WINDOW_PADDING,size.cy-WINDOW_PADDING};
	m_tabCtrl.SetWindowPos(NULL,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
	m_tabCtrl.ModifyStyle(0,WS_CLIPCHILDREN);
	
	RECT rcItem;
	m_tabCtrl.GetItemRect(0,&rcItem);

	m_tabCtrl.GetClientRect(&rc);
//	::InflateRect(&rc, -WINDOW_PADDING, -WINDOW_PADDING );
	rc.bottom -= Util::RectH(&rcItem);

	m_pStyleCodePanel->SetWindowPos(0,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
}

LRESULT CLayoutBuildDlg::OnSelChanging(LPNMHDR pnmh)
{	
	if( NULL == m_pStyleCodePanel )
		return 0;

	return 0;
}

void CLayoutBuildDlg::AttachSkin( HSKIN hSkin )
{
	if(NULL == m_pStyleCodePanel)
		return;

	m_hSkin = hSkin;
	m_pStyleCodePanel->AttachSkin(hSkin);
}