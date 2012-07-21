#include "stdafx.h"
#include "FontBuildDlg.h"
#include "Dialogs.h"

const int BTN_HEIGHT = 22;
const int BTN_WIDTH  = 50;
const int DISPLAY_HEIGHT = 100;

CFontViewDlg::CFontViewDlg()
{
	
}

CFontViewDlg::~CFontViewDlg()
{
}
void CFontViewDlg::OnDestroy()
{
	this->Clear();
}

BOOL CFontViewDlg::OnInitDialog( HWND, LPARAM )
{
	m_listctrl.Attach( GetDlgItem(IDC_FONTLIST) );
	m_listctrl.InsertColumn( 0, _T("id"), LVCFMT_LEFT, 150 );
	m_listctrl.InsertColumn( 1, _T("facename"), LVCFMT_LEFT, 150 );
	m_listctrl.InsertColumn( 2, _T("size"), LVCFMT_LEFT, 80 );
	m_listctrl.InsertColumn( 3, _T("orientation"), LVCFMT_LEFT, 80 );
	m_listctrl.InsertColumn( 4, _T("bold"), LVCFMT_LEFT, 80 );
	m_listctrl.InsertColumn( 5, _T("italic"), LVCFMT_LEFT, 80 );
	m_listctrl.InsertColumn( 6, _T("underline"), LVCFMT_LEFT, 80 );
	m_listctrl.InsertColumn( 7, _T("strikeout"), LVCFMT_LEFT, 80 );

	m_listctrl.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT );
	m_listctrl.ModifyStyle( 0, LVS_SINGLESEL|LVS_SHOWSELALWAYS );

	return TRUE;
}

void  CFontViewDlg::OnSize(UINT nType, CSize size)
{
	int x = WINDOW_PADDING;/*(size.cx - 2*100 - 1*20 )/2*/
	GetDlgItem(IDC_BUTTON_ADD).SetWindowPos    ( NULL, x,                                  WINDOW_PADDING, BTN_WIDTH, BTN_HEIGHT, SWP_NOZORDER );
	GetDlgItem(IDC_BUTTON_DELETE).SetWindowPos ( NULL, x + BTN_WIDTH   + WINDOW_PADDING,   WINDOW_PADDING, BTN_WIDTH, BTN_HEIGHT, SWP_NOZORDER );

	int y = 3*WINDOW_PADDING+DISPLAY_HEIGHT+BTN_HEIGHT;
	m_listctrl.SetWindowPos( 0, WINDOW_PADDING, y, size.cx-2*WINDOW_PADDING,size.cy-y-WINDOW_PADDING,SWP_NOZORDER );

}

void  CFontViewDlg::OnPaint(HDC hDC)
{
	PAINTSTRUCT ps;
	hDC = BeginPaint( &ps );
	::SetBkMode(hDC, TRANSPARENT );
	int nIndex = this->m_listctrl.GetSelectedIndex();
	if( -1 != nIndex )
	{
		HRFONT hRFont = (HRFONT) m_listctrl.GetItemData(nIndex);
		if( NULL != hRFont )
		{
			HFONT hFont = ((GDIRenderFont*)hRFont)->GetHFONT();
			HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
			String str = _T("12344567890 abcdedf 一二三四五六七八九十");
			CRect rcClient;
			this->GetClientRect(&rcClient);
			RECT rc = {0, 2*WINDOW_PADDING+BTN_HEIGHT, rcClient.right, 0 };
			rc.bottom = rc.top + DISPLAY_HEIGHT;
			::DrawText(hDC, str.c_str(), str.length(), &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			::SelectObject(hDC,hOldFont);
		}
	}

	EndPaint(&ps);
}

//
//	设置数据源
//
void CFontViewDlg::AttachSkin( HSKIN h )
{
	if( m_hSkin == h )
		return;

	// changed notify
	m_hSkin = h;

	this->Reload();
}


void  CFontViewDlg::OnBtnAdd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if( NULL == m_hSkin )
		return;

	CNewFontDlg  dlg(m_hSkin);
	if( IDOK == dlg.DoModal() )
	{
		TCHAR szText[32] = _T("");
		int nCount = m_listctrl.GetItemCount();

 		m_listctrl.InsertItem( nCount, dlg.m_strId.c_str() );
		m_listctrl.SetItemText(nCount,1,dlg.m_lf.lfFaceName);
		
		_stprintf(szText, _T("%d"), dlg.m_nSize );
		m_listctrl.SetItemText(nCount,2,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfOrientation );
		m_listctrl.SetItemText(nCount,3,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfWeight==FW_BOLD?1:0 );
		m_listctrl.SetItemText(nCount,4,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfItalic );
		m_listctrl.SetItemText(nCount,5,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfUnderline );
		m_listctrl.SetItemText(nCount,6,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfStrikeOut );
		m_listctrl.SetItemText(nCount,7,szText);

 
 		// 保存数据用于自绘
		
 		HRFONT hRFont = ::UI_GetFont( dlg.m_strId, RENDER_TYPE_GDI, m_hSkin );
 		m_listctrl.SetItemData( nCount, (DWORD_PTR)hRFont );  //  该 pFont 需要进行释放 

 		// 将新增的字体显示出来
 		m_listctrl.SelectItem(nCount);

		this->SET_DIRTY3(true);
	}
}
void  CFontViewDlg::OnBtnRemove(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// 1. 删除资源 
	int nIndex = this->m_listctrl.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	TCHAR szText[MAX_STRING_LEN] = _T("");
	m_listctrl.GetItemText(nIndex, 0, szText, MAX_STRING_LEN );

	if( false == ::UI_RemoveFontItem( m_hSkin, szText ) )
	{
		::MessageBox( NULL, _T("删除失败"), _T("Error"), MB_OK|MB_ICONERROR	);
		return;
	}

	this->m_listctrl.DeleteItem(nIndex);

	// 更新界面
	int nCount = m_listctrl.GetItemCount();
	if( nIndex >= nCount-1 )
	{
		nIndex = nCount-1;
	}
	this->m_listctrl.SelectItem(nIndex);	

	this->SET_DIRTY3(true);
}

bool CFontViewDlg::Clear()
{
	int nCount = m_listctrl.GetItemCount();
	for (int i = 0; i < nCount; i++ )
	{
		HRFONT hRFont = (HRFONT)m_listctrl.GetItemData(i);
		if( NULL != hRFont )
		{
			UI_ReleaseFont(hRFont);
		}
	}

	m_listctrl.DeleteAllItems();
	return true;
}
bool CFontViewDlg::Reload()
{
	m_listctrl.DeleteAllItems();
	this->InvalidateDisplayArea();

	// 获取该皮肤中的所有图片
	int nCount = ::UI_GetSkinFontCount( m_hSkin );
	for( int i = 0; i < nCount; i++ )
	{
		IFontItemInfo*  pInfo = NULL;
		if( ::UI_GetSkinFontItemInfo( m_hSkin, i, &pInfo ) && pInfo!=NULL )
		{
			int nItemCount = m_listctrl.GetItemCount();
			m_listctrl.InsertItem( nItemCount, pInfo->GetIDRef().c_str() );
			LOGFONT* pLogFont = pInfo->GetLogFont();
			if( NULL != pLogFont )
			{
				TCHAR szText[256] = _T("");
				m_listctrl.SetItemText(nItemCount,1, pLogFont->lfFaceName );
				_stprintf(szText, _T("%d"), Util::FontHeight2Size(pLogFont->lfHeight) );
				m_listctrl.SetItemText(nItemCount,2,szText);
				_stprintf(szText, _T("%d"), pLogFont->lfOrientation );
				m_listctrl.SetItemText(nItemCount,3,szText);
				_stprintf(szText, _T("%d"), pLogFont->lfWeight==FW_BOLD?1:0 );
				m_listctrl.SetItemText(nItemCount,4,szText);
				_stprintf(szText, _T("%d"), pLogFont->lfItalic );
				m_listctrl.SetItemText(nItemCount,5,szText);
				_stprintf(szText, _T("%d"), pLogFont->lfUnderline );
				m_listctrl.SetItemText(nItemCount,6,szText);
				_stprintf(szText, _T("%d"), pLogFont->lfStrikeOut );
				m_listctrl.SetItemText(nItemCount,7,szText);

				// 保存数据用于自绘
				HRFONT hFont = ::UI_GetFont( pInfo->GetIDRef(), RENDER_TYPE_GDI, m_hSkin );
				m_listctrl.SetItemData( nItemCount, (DWORD_PTR)hFont );  //  该 pFont 需要进行释放 
			}
		}

		if( NULL != pInfo )
			pInfo->Release();
	}

	// 将新增的显示出来
	m_listctrl.SelectItem(0);

	return true;
}

void CFontViewDlg::InvalidateDisplayArea()
{
	CRect rcClient;
	this->GetClientRect(&rcClient);
	RECT rc = {0, 2*WINDOW_PADDING+BTN_HEIGHT, rcClient.right, 0 };
	rc.bottom = rc.top + DISPLAY_HEIGHT;
	this->InvalidateRect(&rc);
	this->UpdateWindow();
}

//
//	显示当前被选中的字体
//
LRESULT  CFontViewDlg::OnItemChanged(LPNMHDR pnmh)
{
	LPNMLISTVIEW lpnmh = (LPNMLISTVIEW)pnmh;

	// 被选中，或者被取消选中
	if( (!(lpnmh->uOldState & LVIS_SELECTED ) && ( lpnmh->uNewState & LVIS_SELECTED ) ) ||
		((lpnmh->uOldState & LVIS_SELECTED ) &&	!( lpnmh->uNewState & LVIS_SELECTED ) )
		)
	{
		this->InvalidateDisplayArea();
	}
	return 0;
}
LRESULT  CFontViewDlg::OnItemDBClick(LPNMHDR pnmh)
{
	LRESULT lret = this->DefWindowProc();

	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE )pnmh;
	if( -1 == p->iItem )
		return lret;

	HRFONT hRFont = (UIFont*)m_listctrl.GetItemData(p->iItem);
	if( NULL == hRFont )
		return lret;

	TCHAR szText[MAX_STRING_LEN]=_T("");
	m_listctrl.GetItemText(p->iItem, 0, szText, MAX_STRING_LEN );
	String strID = szText;

	CNewFontDlg dlg(m_hSkin,true);
	dlg.SetFontInfo( strID, pFont->GetLogFont() );
	if( IDOK == dlg.DoModal() )
	{
		int nIndex = p->iItem;
		TCHAR szText[32] = _T("");

		m_listctrl.SetItemText(nIndex,1,dlg.m_lf.lfFaceName);
		_stprintf(szText, _T("%d"), dlg.m_nSize );
		m_listctrl.SetItemText(nIndex,2,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfOrientation );
		m_listctrl.SetItemText(nIndex,3,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfWeight==FW_BOLD?1:0 );
		m_listctrl.SetItemText(nIndex,4,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfItalic );
		m_listctrl.SetItemText(nIndex,5,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfUnderline );
		m_listctrl.SetItemText(nIndex,6,szText);
		_stprintf(szText, _T("%d"), dlg.m_lf.lfStrikeOut );
		m_listctrl.SetItemText(nIndex,7,szText);


		// 保存数据用于自绘
		HRFONT hRFont = ::UI_GetFont( dlg.m_strId, RENDER_TYPE_GDI, m_hSkin );
		m_listctrl.SetItemData( nIndex, (DWORD_PTR)hRFont );  //  该 pFont 需要进行释放 

		// 字体刷新 
		this->InvalidateDisplayArea();

		this->SET_DIRTY3(true);
	}
	return lret;
}



//////////////////////////////////////////////////////////////////////////

CFontCodeDlg::CFontCodeDlg() : CResourceCodeDlg(UI_RESOURCE_FONT)
{
}
CFontCodeDlg::~CFontCodeDlg()
{
}





CFontBuildDlg::CFontBuildDlg()
{
	m_pFontViewPanel = NULL;
	m_pFontCodePanel = NULL;
	m_hSkin = NULL;
}

CFontBuildDlg::~CFontBuildDlg()
{
	if( NULL != m_pFontViewPanel )
	{
		delete m_pFontViewPanel;
		m_pFontViewPanel = NULL;
	}
	if( NULL != m_pFontCodePanel )
	{
		delete m_pFontCodePanel;
		m_pFontCodePanel = NULL;
	}
	m_hSkin = NULL;
}

BOOL CFontBuildDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_tabCtrl.Attach(GetDlgItem(IDC_TAB));
	m_tabCtrl.InsertItem(0, _T("view") );
	m_tabCtrl.InsertItem(1, _T("code") );

	m_pFontViewPanel = new CFontViewDlg;
	m_pFontViewPanel->Create(m_tabCtrl.m_hWnd);
	m_pFontCodePanel = new CFontCodeDlg;
	m_pFontCodePanel->Create(m_tabCtrl.m_hWnd);

	m_pFontViewPanel->ShowWindow(SW_SHOW);
	return TRUE;
}
void CFontBuildDlg::OnSize(UINT nType, CSize size)
{
	if (NULL == m_pFontViewPanel || NULL == m_pFontCodePanel )
		return;

	RECT rc = {WINDOW_PADDING,WINDOW_PADDING,size.cx-WINDOW_PADDING,size.cy-WINDOW_PADDING};
	m_tabCtrl.SetWindowPos(NULL,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
	m_tabCtrl.ModifyStyle(0,WS_CLIPCHILDREN);

	RECT rcItem;
	m_tabCtrl.GetItemRect(0,&rcItem);

	m_tabCtrl.GetClientRect(&rc);
//	::InflateRect(&rc, -WINDOW_PADDING, -WINDOW_PADDING );
	rc.bottom -= Util::RectH(&rcItem);

	m_pFontViewPanel->SetWindowPos(0,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
	m_pFontCodePanel->SetWindowPos(0,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
}

LRESULT CFontBuildDlg::OnSelChanging(LPNMHDR pnmh)
{	
	if( NULL == m_pFontCodePanel || NULL == m_pFontViewPanel )
		return 0;

	int nSel = m_tabCtrl.GetCurSel();
	switch(nSel)
	{
	case 1:
		m_pFontCodePanel->Commit_2_TempFile();
		if( false == ::UI_ReloadResource(m_hSkin,UI_RESOURCE_FONT) )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}

		m_pFontViewPanel->ShowWindow(SW_SHOW);	
		m_pFontCodePanel->ShowWindow(SW_HIDE);
		m_pFontViewPanel->Reload();

		break;

	case 0:
		if( false == m_pFontCodePanel->Update_From_TempFile() )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}

		m_pFontCodePanel->ShowWindow(SW_SHOW);
		m_pFontViewPanel->ShowWindow(SW_HIDE);	
		m_pFontViewPanel->Clear();
		break;
	}
	return 0;
}

void CFontBuildDlg::AttachSkin( HSKIN hSkin )
{
	if(NULL == m_pFontViewPanel)
		return;

	m_hSkin = hSkin;
	m_pFontViewPanel->AttachSkin(hSkin);
	m_pFontCodePanel->AttachSkin(hSkin);
}