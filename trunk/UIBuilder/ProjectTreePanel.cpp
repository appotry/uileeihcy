#include "StdAfx.h"
#include "ProjectTreePanel.h"
#include "MainFrame.h"



CProjectTreePanel::CProjectTreePanel(CMainFrame* pFrame )
{
	m_pMainFrame = pFrame;
}

CProjectTreePanel::~CProjectTreePanel(void)
{
	m_pMainFrame = NULL;
}

BOOL CProjectTreePanel::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_treeCtrlRes.Attach(GetDlgItem(IDC_TREE_RES));
	m_treeCtrlLog.Attach(GetDlgItem(IDC_TREE_LOG));
	m_tabCtrl.Attach(GetDlgItem(IDC_TAB));

	// root 工程名
	int nBufSize = 0;
	String strText;

	::UI_GetProjectName(NULL,&nBufSize);
	TCHAR* p = new TCHAR[nBufSize];
	UI_GetProjectName(p,&nBufSize);
	strText = p;
	delete[] p;
	
	HTREEITEM hTreeRoot = m_treeCtrlRes.InsertItem( strText.c_str(), TVI_ROOT, TVI_LAST );
	m_treeCtrlRes.SetItemData(hTreeRoot, MAKE_RES_DATA(0,UI_RESOURCE_PROJECT) );
	
	// 皮肤列表
	int nCount = ::UI_GetSkinCount();
	for( int i = 0; i < nCount; i++ )
	{
		HSKIN hSkin = ::UI_GetSkinHandle(i);
		if ( NULL == hSkin )
			continue;

		this->InsertSkinItem(hSkin);
	}

	m_treeCtrlRes.Expand(hTreeRoot);

	// 日志信息
	hTreeRoot = m_treeCtrlLog.InsertItem( _T("log"), TVI_ROOT, TVI_LAST );
	m_treeCtrlLog.SetItemData(hTreeRoot, UI_RESOURCE_LOG );

	nCount = g_pLog->GetLogItemCount();
	for( int i = 0; i<nCount; i++ )
	{
		ILogItem* pLogItem = g_pLog->GetLogItemByIndex(i);
		if( NULL != pLogItem )
		{
			BSTR bstrRet = pLogItem->GetName();
			_bstr_t bstr = bstrRet;
			::SysFreeString(bstrRet);
			this->SendMessage(WM_INSERT_LOG_ITEM,(WPARAM)(TCHAR*)bstr, (LPARAM)0 );
			pLogItem->Release();
		}
	}

	m_treeCtrlLog.Expand(hTreeRoot);

	// tab
	m_tabCtrl.InsertItem(0, _T("Resource") );
	m_tabCtrl.InsertItem(1, _T("Log") );

	return TRUE;
}

//
//	释放保存在tree ctrl中的数据
//
void CProjectTreePanel::OnDestroy( )
{
}
void CProjectTreePanel::OnSize(UINT nType, CSize size)
{
	this->CalcLayout( size.cx, size.cy );
}
void CProjectTreePanel::CalcLayout( int cx, int cy )
{
	m_tabCtrl.SetWindowPos( NULL, WINDOW_PADDING, cy-WINDOW_PADDING-HEIGHT_TAB, cx-2*WINDOW_PADDING, HEIGHT_TAB, SWP_NOZORDER );
	m_treeCtrlRes.SetWindowPos(NULL, WINDOW_PADDING,WINDOW_PADDING, cx-2*WINDOW_PADDING,cy-2*WINDOW_PADDING-HEIGHT_TAB, SWP_NOZORDER );
	m_treeCtrlLog.SetWindowPos(NULL, WINDOW_PADDING,WINDOW_PADDING, cx-2*WINDOW_PADDING,cy-2*WINDOW_PADDING-HEIGHT_TAB, SWP_NOZORDER );
}


LRESULT  CProjectTreePanel::OnResTreeDBClick(LPNMHDR pnmh)
{
	HTREEITEM hItem = m_treeCtrlRes.GetSelectedItem();

	if ((hItem != NULL) && !m_treeCtrlRes.ItemHasChildren(hItem))
	{
		DWORD_PTR dwData = GET_RES_TYPE(m_treeCtrlRes.GetItemData( hItem ));  // 结点类型
		DWORD_PTR dwParentData = m_treeCtrlRes.GetItemData( m_treeCtrlRes.GetParentItem( hItem ) );  // 皮肤句柄

		m_pMainFrame->SwitchToSkinResPanel( (UI_RESOURCE_TYPE)dwData, (HSKIN)dwParentData );
	}

	return 0;
}

LRESULT CProjectTreePanel::OnLogTreeDBClick(LPNMHDR pnmh)
{
	HTREEITEM hItem = m_treeCtrlLog.GetSelectedItem();

	if (hItem != NULL)
	{
		TCHAR szText[MAX_STRING_LEN];
		m_treeCtrlLog.GetItemText(hItem, szText, MAX_STRING_LEN);
		m_pMainFrame->SwitchToLogPanel( m_treeCtrlLog.GetItemData(hItem), String(szText));
	}

	return 0;
}

LRESULT CProjectTreePanel::OnTabSelChange(LPNMHDR pnmh)
{
	int nIndex = m_tabCtrl.GetCurSel();
	switch (nIndex)
	{
	case 0:
		m_treeCtrlRes.ShowWindow(SW_SHOW);
		m_treeCtrlLog.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_treeCtrlRes.ShowWindow(SW_HIDE);
		m_treeCtrlLog.ShowWindow(SW_SHOW);
		break;
	}
	return 0;
}

LRESULT CProjectTreePanel::OnLogTreeRClick(LPNMHDR pnmh)
{
	HTREEITEM hTreeItem = m_treeCtrlLog.GetSelectedItem();
	if ( NULL == hTreeItem )
		return 0;

	const int MENUID_ADD_LOG_ITEM = 1;

	HMENU hMenu = ::CreatePopupMenu();
	DWORD_PTR dwData = m_treeCtrlLog.GetItemData(hTreeItem);
	switch(dwData)
	{
	case UI_RESOURCE_LOG:
		::InsertMenu(hMenu,0,MF_BYPOSITION, MENUID_ADD_LOG_ITEM, _T("Add Log Module") );
		break;
	case UI_RESOURCE_LOGMODULE:
		break;
	}

	POINT pt;
	::GetCursorPos(&pt);
	UINT nRet = ::TrackPopupMenuEx(hMenu, TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL );
	::DestroyMenu(hMenu);

	switch(nRet)
	{
	case MENUID_ADD_LOG_ITEM:// 增加一个日志模块
		{
			m_pMainFrame->SwitchToLogPanel( UI_RESOURCE_LOG, _T("") );
		}
		break;
	}

	
	return 0;
}


//
//	通知：新增日志模块消息
//
LRESULT CProjectTreePanel::OnInsertLogItem( UINT, WPARAM wParam, LPARAM )
{
	TCHAR* szModuleName = (TCHAR*)wParam;
	if ( NULL==szModuleName )
		return 0;

	HTREEITEM hRoot = m_treeCtrlLog.GetRootItem();
	m_treeCtrlLog.SetItemData(
		m_treeCtrlLog.InsertItem( szModuleName, hRoot, TVI_LAST ),
		UI_RESOURCE_LOGMODULE );

	m_treeCtrlLog.Expand(hRoot);
	return 1;
}

LRESULT CProjectTreePanel::OnModifyLogItem( UINT, WPARAM wParam, LPARAM lParam )
{
	TCHAR* szModuleName = (TCHAR*)wParam;
	TCHAR* szOldModuleName = (TCHAR*)lParam;
	if ( NULL==szModuleName || NULL == szOldModuleName )
		return 0;

	// 查找
	HTREEITEM hRoot = m_treeCtrlLog.GetRootItem();
	HTREEITEM hItem = m_treeCtrlLog.GetChildItem(hRoot);
	while( NULL != hItem )
	{
		TCHAR szName[MAX_STRING_LEN] = _T("");
		m_treeCtrlLog.GetItemText(hItem, szName, MAX_STRING_LEN );
		if( 0 == _tcscmp(szName, szOldModuleName) )
		{
			m_treeCtrlLog.SetItemText(hItem, szModuleName);
			break;
		}

		hItem = m_treeCtrlLog.GetNextItem(hItem,TVGN_NEXT);
	}

	return 1;
}

//
//	在res tree中插入一个皮肤结点
//
//	皮肤名称结点的 data 为 皮肤句柄 HSKIN
//	资源名称结点的 data 为 是否需要保存 BOOL bDirty
//
void CProjectTreePanel::InsertSkinItem(HSKIN hSkin)
{
	if( NULL == hSkin )
		return;

	HTREEITEM hTreeRoot = m_treeCtrlRes.GetRootItem();
	if ( NULL == hTreeRoot )
		return;

	String strSkinName;
	TCHAR* pszName = NULL;
	int    nSize = 0;

	nSize = ::UI_GetSkinName( hSkin, NULL, 0 );
	pszName = new TCHAR[nSize];

	nSize = ::UI_GetSkinName( hSkin, pszName, nSize );
	strSkinName = pszName ;
	delete [] pszName;
	pszName = NULL;

	HTREEITEM hSkinTreeItem = m_treeCtrlRes.InsertItem( strSkinName.c_str(), hTreeRoot, TVI_LAST );
	m_treeCtrlRes.SetItemData( hSkinTreeItem, (DWORD_PTR)hSkin );   

	HTREEITEM hItem = NULL;

	hItem = m_treeCtrlRes.InsertItem( _T("image"), hSkinTreeItem, TVI_LAST );
	m_treeCtrlRes.SetItemData( hItem, (DWORD_PTR)MAKEWPARAM(UI_RESOURCE_IMAGE,0) );

	hItem = m_treeCtrlRes.InsertItem( _T("color"), hSkinTreeItem, TVI_LAST );
	m_treeCtrlRes.SetItemData( hItem, (DWORD_PTR)MAKEWPARAM(UI_RESOURCE_COLOR,0) );

	hItem = m_treeCtrlRes.InsertItem( _T("font"), hSkinTreeItem, TVI_LAST );
	m_treeCtrlRes.SetItemData( hItem, (DWORD_PTR)MAKEWPARAM(UI_RESOURCE_FONT,0) );

	hItem = m_treeCtrlRes.InsertItem( _T("layout"), hSkinTreeItem, TVI_LAST );
	m_treeCtrlRes.SetItemData( hItem, (DWORD_PTR)MAKEWPARAM(UI_RESOURCE_LAYOUT,0) );

	hItem = m_treeCtrlRes.InsertItem( _T("style"), hSkinTreeItem, TVI_LAST );
	m_treeCtrlRes.SetItemData( hItem, (DWORD_PTR)MAKEWPARAM(UI_RESOURCE_STYLE,0) );

	hItem = m_treeCtrlRes.InsertItem( _T("i18n"), hSkinTreeItem, TVI_LAST );
	m_treeCtrlRes.SetItemData( hItem, (DWORD_PTR)MAKEWPARAM(UI_RESOURCE_I18N,0) );

	m_treeCtrlRes.Expand( hSkinTreeItem );	
}

String CProjectTreePanel::GetResName( UI_RESOURCE_TYPE e )
{
	switch(e)
	{
	case UI_RESOURCE_IMAGE:
		return _T("image");
		break;
	case UI_RESOURCE_COLOR:
		return _T("color");
		break;
	case UI_RESOURCE_FONT:
		return _T("font");
		break;
	case UI_RESOURCE_STYLE:
		return _T("style");
		break;
	case UI_RESOURCE_LAYOUT:
		return _T("layout");
		break;
	}
	return _T("");
}
HTREEITEM CProjectTreePanel::FindSkinItem(HSKIN hSkin)
{
	HTREEITEM hRootItem = m_treeCtrlRes.GetRootItem();
	HTREEITEM hSkinItem = m_treeCtrlRes.GetChildItem(hRootItem);

	// 遍历HSKIN结点
	while (NULL != hSkinItem)
	{
		DWORD_PTR dwDataSkin = m_treeCtrlRes.GetItemData(hSkinItem);
		if( dwDataSkin == (DWORD_PTR)hSkin )
		{
			break;
		}
		hSkinItem = m_treeCtrlRes.GetNextItem(hSkinItem, TVGN_NEXT );
	}

	return hSkinItem;
}
HTREEITEM CProjectTreePanel::FindResItem(HSKIN hSkin,UI_RESOURCE_TYPE eResType)
{
	HTREEITEM hSkinItem = this->FindSkinItem(hSkin);
	if( NULL == hSkinItem )
		return NULL;

	HTREEITEM hResItem = m_treeCtrlRes.GetChildItem(hSkinItem);
	while (NULL != hResItem)
	{
		DWORD_PTR dwDataSkin = m_treeCtrlRes.GetItemData(hResItem);
		if( GET_RES_TYPE(dwDataSkin) == eResType )
		{
			break;
		}
		hResItem = m_treeCtrlRes.GetNextItem(hResItem, TVGN_NEXT );
	}

	return hResItem;
}

LRESULT CProjectTreePanel::OnGetAppDirty(UINT,WPARAM,LPARAM)
{
	HTREEITEM hRootItem = m_treeCtrlRes.GetRootItem();
	HTREEITEM hSkinItem = m_treeCtrlRes.GetChildItem(hRootItem);

	if(GET_RES_DIRTY(m_treeCtrlRes.GetItemData(hRootItem)))
		return 1;

	// 遍历HSKIN结点
	while (NULL != hSkinItem)
	{
		HTREEITEM hResItem = m_treeCtrlRes.GetChildItem(hSkinItem);
		while (NULL != hResItem)
		{
			DWORD_PTR dwDataRes = m_treeCtrlRes.GetItemData(hResItem);
			if( GET_RES_DIRTY(dwDataRes) == 1 )
			{
				return 1;
			}
			hResItem = m_treeCtrlRes.GetNextItem(hResItem, TVGN_NEXT );
		}
		hSkinItem = m_treeCtrlRes.GetNextItem(hSkinItem, TVGN_NEXT );
	}
	return 0;
}

LRESULT CProjectTreePanel::OnClearDirty(UINT,WPARAM,LPARAM)
{
	HTREEITEM hRootItem = m_treeCtrlRes.GetRootItem();
	HTREEITEM hSkinItem = m_treeCtrlRes.GetChildItem(hRootItem);

	m_treeCtrlRes.SetItemData(hRootItem,MAKE_RES_DATA(0,UI_RESOURCE_PROJECT));

	// 遍历HSKIN结点
	while (NULL != hSkinItem)
	{
		HTREEITEM hResItem = m_treeCtrlRes.GetChildItem(hSkinItem);
		while (NULL != hResItem)
		{
			DWORD_PTR dwDataRes = m_treeCtrlRes.GetItemData(hResItem);
			UI_RESOURCE_TYPE eResType = (UI_RESOURCE_TYPE)GET_RES_TYPE(dwDataRes);
			m_treeCtrlRes.SetItemData(hResItem, MAKE_RES_DATA(0, eResType));
			
			// 去除*
			String strText = this->GetResName(eResType);
			if (GET_RES_DIRTY(dwDataRes))
			{
				m_treeCtrlRes.SetItemText(hResItem, strText.c_str());
			}
			
			hResItem = m_treeCtrlRes.GetNextItem(hResItem, TVGN_NEXT );
		}
		hSkinItem = m_treeCtrlRes.GetNextItem(hSkinItem, TVGN_NEXT );
	}
	return 0;
}

LRESULT CProjectTreePanel::OnGetDirty(UINT u,WPARAM w,LPARAM l)
{
	UI_RESOURCE_TYPE eResType = (UI_RESOURCE_TYPE)w;
	HSKIN hSkin = (HSKIN)l;

	if( eResType == UI_RESOURCE_PROJECT )
	{
		HTREEITEM hRootItem = m_treeCtrlRes.GetRootItem();
		return GET_RES_DIRTY(m_treeCtrlRes.GetItemData(hRootItem));
	}

	HTREEITEM hTreeItem = this->FindResItem(hSkin,eResType);
	if (NULL == hTreeItem)
	{
		return 0;
	}

	DWORD_PTR dwData = m_treeCtrlRes.GetItemData(hTreeItem);
	bool bOldDirty = GET_RES_DIRTY(dwData)==0?false:true;

	if(bOldDirty)
		return 1;
	else
		return 0;
}
LRESULT CProjectTreePanel::OnSetDirty(UINT,WPARAM w,LPARAM l)
{
	UI_RESOURCE_TYPE eResType = (UI_RESOURCE_TYPE)GET_RES_TYPE(w);
	bool  bDirty = GET_RES_DIRTY(w)==0?false:true;
	HSKIN hSkin = (HSKIN)l;

	if( eResType == UI_RESOURCE_PROJECT )
	{
		HTREEITEM hRootItem = m_treeCtrlRes.GetRootItem();
		m_treeCtrlRes.SetItemData(hRootItem, MAKE_RES_DATA(bDirty,eResType));
		return 1;
	}

	HTREEITEM hTreeItem = this->FindResItem(hSkin,eResType);
	if (NULL == hTreeItem)
	{
		return 0;
	}

	DWORD_PTR dwData = m_treeCtrlRes.GetItemData(hTreeItem);
	bool bOldDirty = GET_RES_DIRTY(dwData)==0?false:true;

	dwData = MAKEWPARAM(eResType,bDirty);
	this->m_treeCtrlRes.SetItemData(hTreeItem,dwData);

	if(bDirty != bOldDirty)
	{
		String strText = this->GetResName(eResType);
		if (bDirty)
		{
			strText += _T("*");
		}
		m_treeCtrlRes.SetItemText(hTreeItem, strText.c_str());
	}
	return 1;
}
