#include "StdAfx.h"
#include "StyleBuildDlg.h"
#include "Dialogs.h"


CStyleViewDlg::CStyleViewDlg(void)
{
}

CStyleViewDlg::~CStyleViewDlg(void)
{
}

void CStyleViewDlg::OnDestroy()
{
	int nCount = m_listctrl.GetItemCount();
	for (int i = 0; i < nCount; i++ )
	{
		IStyleItemInfo* pInfo = (IStyleItemInfo*)m_listctrl.GetItemData(i);
		if( NULL != pInfo )
		{
			pInfo->Release();
			pInfo = NULL;
		}
	}
	m_listctrl.DeleteAllItems();
}
BOOL CStyleViewDlg::OnInitDialog( HWND, LPARAM )
{
	m_listctrl.Attach( GetDlgItem(IDC_STYLELIST) );
	m_listctrl.InsertColumn( 0, _T("type"), LVCFMT_LEFT, 100 );
	m_listctrl.InsertColumn( 1, _T("id"), LVCFMT_LEFT, 100 );
	m_listctrl.InsertColumn( 2, _T("inherit"), LVCFMT_LEFT, 300 );

	m_listctrl_prop.Attach( GetDlgItem(IDC_LIST_STYLE_PROP) );
	m_listctrl_prop.InsertColumn( 0, _T("key"), LVCFMT_LEFT, 110 );
	m_listctrl_prop.InsertColumn( 1, _T("value"), LVCFMT_LEFT, 110 );

	m_listctrl.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );
	m_listctrl.ModifyStyle( 0, LVS_SINGLESEL|LVS_SHOWSELALWAYS );
	m_listctrl_prop.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );
	m_listctrl_prop.ModifyStyle( 0, LVS_SINGLESEL|LVS_SHOWSELALWAYS );

	return TRUE;
}

void  CStyleViewDlg::OnSize(UINT nType, CSize size)
{
	const int PROP_WND_WIDTH = 230;
	const int BTN_HEIGHT = 22;
	const int BTN_WIDTH  = 100;
	const int PROP_BTN_WIDTH = 70;

	HDWP hdwp = ::BeginDeferWindowPos(8);

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_ADD_SELECTOR).m_hWnd, 0, WINDOW_PADDING, WINDOW_PADDING,
		BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_MODIFY_SELECTOR).m_hWnd, 0, WINDOW_PADDING*2+BTN_WIDTH, WINDOW_PADDING,
		BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_DELETE_SELECTOR).m_hWnd, 0, WINDOW_PADDING*3+BTN_WIDTH*2, WINDOW_PADDING,
		BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_ADD_PROP).m_hWnd, 0, size.cx-WINDOW_PADDING*3-PROP_BTN_WIDTH*3, WINDOW_PADDING,
		PROP_BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_MODIFY_PROP).m_hWnd, 0, size.cx-WINDOW_PADDING*2-PROP_BTN_WIDTH*2, WINDOW_PADDING,
		PROP_BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,GetDlgItem(IDC_BUTTON_REMOVE_PROP).m_hWnd, 0, size.cx-WINDOW_PADDING-PROP_BTN_WIDTH, WINDOW_PADDING,
		PROP_BTN_WIDTH,BTN_HEIGHT,SWP_NOZORDER );

	::DeferWindowPos(hdwp,m_listctrl.m_hWnd, 0, WINDOW_PADDING, 
		WINDOW_PADDING*2+BTN_HEIGHT, 
		size.cx-PROP_WND_WIDTH-3*WINDOW_PADDING,
		size.cy-BTN_HEIGHT-3*WINDOW_PADDING,SWP_NOZORDER );

	::DeferWindowPos(hdwp,m_listctrl_prop.m_hWnd, 0, size.cx-PROP_WND_WIDTH-WINDOW_PADDING,
		WINDOW_PADDING*2+BTN_HEIGHT,
		PROP_WND_WIDTH,
		size.cy-BTN_HEIGHT-3*WINDOW_PADDING,SWP_NOZORDER );

	::EndDeferWindowPos(hdwp);
}
//
//	设置数据源
//
void CStyleViewDlg::AttachSkin( HSKIN h )
{
	if( m_hSkin == h )
		return;

	// changed notify
	m_hSkin = h;

	this->Reload();
}

bool CStyleViewDlg::Reload()
{
	if( NULL == m_hSkin )
		return false;

	// 获取该皮肤中的所有图片
	m_listctrl.DeleteAllItems();
	m_listctrl_prop.DeleteAllItems();

	int nCount = ::UI_GetSkinStyleCount( m_hSkin );

	TCHAR szText[256] = _T("");
	for( int i = 0; i < nCount; i++ )
	{
		IStyleItemInfo*  pInfo = NULL;
		if( ::UI_GetSkinStyleItemInfo( m_hSkin, i, &pInfo ) && pInfo!=NULL )
		{
			int nItemCount = m_listctrl.GetItemCount();
			
			STYLE_SELECTOR_TYPE type = pInfo->GetSelectorType();
			switch(type)
			{
			case STYLE_SELECTOR_TYPE_ID:
				_stprintf(szText, _T("ID"));
				break;
			case STYLE_SELECTOR_TYPE_CLASS:
				_stprintf(szText, _T("CLASS"));
				break;
			default:
				_stprintf(szText, _T("TAG"));
				break;
			}
			m_listctrl.InsertItem( nItemCount, szText );
			m_listctrl.SetItemText( nItemCount, 1, pInfo->GetIDRef().c_str() );
			m_listctrl.SetItemText( nItemCount, 2, pInfo->GetInheritsRef().c_str() );

			m_listctrl.SetItemData(nItemCount, (DWORD_PTR)pInfo);
		}
	}

	m_listctrl.SelectItem(0);
	return true;
}

void  CStyleViewDlg::OnBtnAddSelector(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if( NULL == m_hSkin )
		return;

	CNewStyleDlg dlg(m_hSkin);
	if( IDOK == dlg.DoModal() )
	{
		int nCount = m_listctrl.GetItemCount();
		
		TCHAR szText[256] = _T("");
		IStyleItemInfo*  pInfo = NULL;
		if( ::UI_GetSkinStyleItemInfo( m_hSkin, nCount, &pInfo ) && pInfo!=NULL )
		{
			STYLE_SELECTOR_TYPE type = pInfo->GetSelectorType();
			switch(type)
			{
			case STYLE_SELECTOR_TYPE_ID:
				_stprintf(szText, _T("ID"));
				break;
			case STYLE_SELECTOR_TYPE_CLASS:
				_stprintf(szText, _T("CLASS"));
				break;
			default:
				_stprintf(szText, _T("TAG"));
				break;
			}
			m_listctrl.InsertItem( nCount, szText );
			m_listctrl.SetItemText( nCount, 1, pInfo->GetIDRef().c_str() );
			m_listctrl.SetItemText( nCount, 2, pInfo->GetInheritsRef().c_str() );

			m_listctrl.SetItemData(nCount, (DWORD_PTR)pInfo);

			this->SET_DIRTY3(true);
		}
	}
}

void  CStyleViewDlg::OnBtnModifySelector(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listctrl.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	IStyleItemInfo* pInfo = (IStyleItemInfo*)m_listctrl.GetItemData(nIndex);
	if( NULL == pInfo )
		return;

	CNewStyleDlg dlg(m_hSkin, true);
	dlg.SetStyleInfo( pInfo->GetSelectorType(), pInfo->GetIDRef(), pInfo->GetInheritsRef() );
	if( IDOK == dlg.DoModal() )
	{
		m_listctrl.SetItemText(nIndex, 2, pInfo->GetInheritsRef().c_str() );

		this->SET_DIRTY3(true);
	}
}
void  CStyleViewDlg::OnBtnDeleteSelector(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listctrl.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	IStyleItemInfo* pInfo = (IStyleItemInfo*)m_listctrl.GetItemData(nIndex);
	if( NULL == pInfo )
		return;

	// 不要直接传递pInfo->GetIDRef()给UI_RemoveStyleItem，pInfo会被销毁的
	String strID = pInfo->GetIDRef();
	STYLE_SELECTOR_TYPE type = pInfo->GetSelectorType();

	if( ::UI_RemoveStyleItem(m_hSkin, type, strID) )
	{
		m_listctrl.DeleteItem(nIndex);

		this->SET_DIRTY3(true);
	}
	else
	{
		::MessageBox(NULL, _T("删除失败"), _T("Error"), MB_OK|MB_ICONWARNING );
	}
}

void CStyleViewDlg::OnBtnAddProp(UINT uNotifyCode, int nID, CWindow wndCtl)
{	
	int nIndex2 = m_listctrl.GetSelectedIndex();
	if( -1 == nIndex2 )
		return;

	IStyleItemInfo* pInfo = (IStyleItemInfo*)m_listctrl.GetItemData(nIndex2);
	if(NULL == pInfo)
		return;

	CNewStyleAttributeDlg dlg(m_hSkin,false);
	dlg.SetStyleInfo(pInfo->GetSelectorType(), pInfo->GetIDRef());
	if( IDOK == dlg.DoModal() )
	{
		int nCount = m_listctrl_prop.GetItemCount();
		m_listctrl_prop.InsertItem(nCount, dlg.m_strKey.c_str() );
		m_listctrl_prop.SetItemText(nCount, 1, dlg.m_strValue.c_str() );

		this->SET_DIRTY3(true);
	}
}
void CStyleViewDlg::OnBtnModifyProp(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listctrl_prop.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	int nIndex2 = m_listctrl.GetSelectedIndex();
	if( -1 == nIndex2 )
		return;

	IStyleItemInfo* pInfo = (IStyleItemInfo*)m_listctrl.GetItemData(nIndex2);
	if(NULL == pInfo)
		return ;

	CString strKey, strValue;
	m_listctrl_prop.GetItemText(nIndex,0,strKey);
	m_listctrl_prop.GetItemText(nIndex,1,strValue);

	String key = strKey.GetString();
	String value = strValue.GetString();

	CNewStyleAttributeDlg dlg(m_hSkin,true);
	dlg.SetStyleInfo(pInfo->GetSelectorType(), pInfo->GetIDRef());
	dlg.SetAttributeInfo(key,value);
	if( IDOK == dlg.DoModal() )
	{
		m_listctrl_prop.SetItemText(nIndex, 1, dlg.m_strValue.c_str() );

		this->SET_DIRTY3(true);
	}
}
void CStyleViewDlg::OnBtnDeleteProp(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listctrl_prop.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	int nIndex2 = m_listctrl.GetSelectedIndex();
	if( -1 == nIndex2 )
		return ;

	IStyleItemInfo* pInfo = (IStyleItemInfo*)m_listctrl.GetItemData(nIndex2);
	if(NULL == pInfo)
		return ;

	CString strKey, strValue;
	m_listctrl_prop.GetItemText(nIndex,0,strKey);
	m_listctrl_prop.GetItemText(nIndex,1,strValue);

	String key = strKey.GetString();
	String value = strValue.GetString();

	if( ::UI_RemoveStyleAttribute( m_hSkin, pInfo->GetSelectorType(), pInfo->GetIDRef(), key ) )
	{
		m_listctrl_prop.DeleteItem(nIndex);

		this->SET_DIRTY3(true);
	}
	else
	{
		::MessageBox(m_hWnd, _T("删除失败"), _T("Error"), MB_OK|MB_ICONWARNING );
	}
}

LRESULT CStyleViewDlg::OnItemDBClick(LPNMHDR pnmh)
{
	LRESULT lret = this->DefWindowProc();

	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE )pnmh;
	if( -1 == p->iItem )
		return lret;

	this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MODIFY_SELECTOR,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_MODIFY_SELECTOR).m_hWnd);

	return lret;
}
LRESULT CStyleViewDlg::OnPropItemDBClick(LPNMHDR pnmh)
{
	LRESULT lret = this->DefWindowProc();

	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE )pnmh;
	if( -1 == p->iItem )
		return lret;

	this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_MODIFY_PROP,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_MODIFY_PROP).m_hWnd);

	return lret;
}

LRESULT CStyleViewDlg::OnStyleItemChanged(LPNMHDR pnmh)
{
	LPNMLISTVIEW lpnmh = (LPNMLISTVIEW)pnmh;

	// 被选中，或者被取消选中
	if( (!(lpnmh->uOldState & LVIS_SELECTED ) && ( lpnmh->uNewState & LVIS_SELECTED ) ) ||
		((lpnmh->uOldState & LVIS_SELECTED ) &&	!( lpnmh->uNewState & LVIS_SELECTED ) )
		)
	{
		m_listctrl_prop.DeleteAllItems();

		int nIndex = m_listctrl.GetSelectedIndex();
		if( -1 == nIndex )
			return 0;

		IStyleItemInfo* pInfo = (IStyleItemInfo*)m_listctrl.GetItemData(nIndex);
		if(NULL == pInfo)
			return 0;

		int nCount = pInfo->GetAttributesCount();
		TCHAR szText[32] = _T("");

		for ( int i = 0; i < nCount; i++ )
		{
			TCHAR szKey[MAX_STRING_LEN] = _T("");
			TCHAR szValue[MAX_STRING_LEN] = _T("");
			pInfo->GetAttribute(i, szKey, szValue );
			m_listctrl_prop.InsertItem( i, szKey );
			m_listctrl_prop.SetItemText( i, 1, szValue );
		}
	}
	return 0;
}	

//////////////////////////////////////////////////////////////////////////



CStyleCodeDlg::CStyleCodeDlg() : CResourceCodeDlg(UI_RESOURCE_STYLE)
{
}
CStyleCodeDlg::~CStyleCodeDlg()
{
}


CStyleBuildDlg::CStyleBuildDlg()
{
	m_pStyleViewPanel = NULL;
	m_pStyleCodePanel = NULL;
	m_hSkin = NULL;
}

CStyleBuildDlg::~CStyleBuildDlg()
{
	if( NULL != m_pStyleViewPanel )
	{
		delete m_pStyleViewPanel;
		m_pStyleViewPanel = NULL;
	}
	if( NULL != m_pStyleCodePanel )
	{
		delete m_pStyleCodePanel;
		m_pStyleCodePanel = NULL;
	}
	m_hSkin = NULL;
}

BOOL CStyleBuildDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_tabCtrl.Attach(GetDlgItem(IDC_TAB));
	m_tabCtrl.InsertItem(0, _T("view") );
	m_tabCtrl.InsertItem(1, _T("code") );

	m_pStyleViewPanel = new CStyleViewDlg;
	m_pStyleViewPanel->Create(m_tabCtrl.m_hWnd);
	m_pStyleCodePanel = new CStyleCodeDlg;
	m_pStyleCodePanel->Create(m_tabCtrl.m_hWnd);

	m_pStyleViewPanel->ShowWindow(SW_SHOW);
	return TRUE;
}
void CStyleBuildDlg::OnSize(UINT nType, CSize size)
{
	if (NULL == m_pStyleViewPanel || NULL == m_pStyleCodePanel )
		return;

	RECT rc = {WINDOW_PADDING,WINDOW_PADDING,size.cx-WINDOW_PADDING,size.cy-WINDOW_PADDING};
	m_tabCtrl.SetWindowPos(NULL,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
	m_tabCtrl.ModifyStyle(0,WS_CLIPCHILDREN);
	
	RECT rcItem;
	m_tabCtrl.GetItemRect(0,&rcItem);

	m_tabCtrl.GetClientRect(&rc);
//	::InflateRect(&rc, -WINDOW_PADDING, -WINDOW_PADDING );
	rc.bottom -= Util::RectH(&rcItem);

	m_pStyleViewPanel->SetWindowPos(0,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
	m_pStyleCodePanel->SetWindowPos(0,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
}

LRESULT CStyleBuildDlg::OnSelChanging(LPNMHDR pnmh)
{	
	if( NULL == m_pStyleCodePanel || NULL == m_pStyleViewPanel )
		return 0;

	int nSel = m_tabCtrl.GetCurSel();
	switch(nSel)
	{
	case 1:
		m_pStyleCodePanel->Commit_2_TempFile();
		if( false == UI_ReloadResource(m_hSkin, UI_RESOURCE_STYLE) )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}

		m_pStyleViewPanel->ShowWindow(SW_SHOW);	
		m_pStyleCodePanel->ShowWindow(SW_HIDE);
		m_pStyleViewPanel->Reload();

		break;

	case 0:
		if( false == m_pStyleCodePanel->Update_From_TempFile() )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}

		m_pStyleCodePanel->ShowWindow(SW_SHOW);
		m_pStyleViewPanel->ShowWindow(SW_HIDE);	
		break;
	}
	return 0;
}

void CStyleBuildDlg::AttachSkin( HSKIN hSkin )
{
	if(NULL == m_pStyleViewPanel)
		return;

	m_hSkin = hSkin;
	m_pStyleViewPanel->AttachSkin(hSkin);
	m_pStyleCodePanel->AttachSkin(hSkin);
}