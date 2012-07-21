#include "StdAfx.h"
#include "MainFrame.h"
#include "Dialogs.h"
#include "colorbuilddlg.h"
#include "FontBuildDlg.h"
#include "StyleBuildDlg.h"
#include "LogViewDlg.h"
#include "ImageBuildDlg.h"
#include "LayoutBuildDlg.h"
#include "WorkAreaPanel.h"

CMainFrame::CMainFrame(void)
{
	m_pPanelProjectTree = NULL;
	m_hWndMDIClient     = NULL;
	m_pLogViewDlg       = NULL;
	m_pDlgFirstPage     = NULL;
	m_bProjectOpened   = false;
}

CMainFrame::~CMainFrame(void)
{
	if( NULL != m_pPanelProjectTree )
	{
		delete m_pPanelProjectTree;
		m_pPanelProjectTree = NULL;
	}
 	if ( NULL != m_pDlgFirstPage )
	{
		delete m_pDlgFirstPage;
		m_pDlgFirstPage = NULL;
	}
	if( NULL != m_pLogViewDlg )
	{
		delete m_pLogViewDlg;
		m_pLogViewDlg = NULL;
	}

	::UI_Exit();
}


LRESULT CMainFrame::OnCreate( LPCREATESTRUCT lpcs )
{
	// 加载主窗口菜单
	HMENU hMenu = ::LoadMenu( NULL, MAKEINTRESOURCE(IDC_UIBUILDER) );
	this->SetMenu( hMenu );
	DestroyMenu(hMenu);

	// 允许拖拽文件打开工程
	::DragAcceptFiles(m_hWnd,TRUE);

	// 快捷键消息拦截
	_Module.GetMessageLoop()->AddMessageFilter(this);

	// 显示首页窗口
	m_pDlgFirstPage = new CFirstPageDlg(this);
	m_pDlgFirstPage->Create( m_hWnd );
	m_pDlgFirstPage->CenterWindow();
	m_pDlgFirstPage->ShowWindow(SW_SHOW);

	// 不刷新子窗口处背景
	this->ModifyStyle( 0, WS_CLIPCHILDREN );

	// 创建多文档窗口MDIClient
	this->_InitMDIClient();

	// 设置皮肤编辑模式
	::UI_SetDesignMode(true);

	return 1;
}

//
// 创建多文档窗口MDIClient，注册相应类型的子窗口
//
void CMainFrame::_InitMDIClient()
{
	//////////////////////////////////////////////////////////////////////////
	// 创建MDIClient

	CLIENTCREATESTRUCT ccs = { NULL, 1 };
	this->m_hWndMDIClient = CreateWindow( _T("MDICLIENT"), (LPCTSTR) NULL, 
		WS_CHILD | WS_CLIPCHILDREN , 
		0, 0, 0, 0, m_hWnd, (HMENU) NULL, _Module.m_hInst, (LPSTR) &ccs); 

	::ShowWindow(m_hWndMDIClient, SW_SHOW); 

	//////////////////////////////////////////////////////////////////////////
	// 注册child windows

	// 1. image builder
	
// 	WNDCLASS wc; 
// 
// 	wc.style         = 0; 
// 	wc.cbClsExtra    = 0; 
// 	wc.cbWndExtra    = 0; 
// 	wc.hInstance     = _Module.m_hInst; 
// 	wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
// 	wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1); 
// 	wc.lpfnWndProc   = (WNDPROC) ::DefWindowProc; 
// 	wc.hIcon         = NULL;//LoadIcon(hInst, IDNOTE); 
// 	wc.lpszMenuName  = (LPCTSTR) NULL; 
// 	wc.cbWndExtra    = sizeof(void*); 
// 	wc.lpszClassName = CHILD_WINDOW_CLASS_IMAGE; 
// 
// 	if (!RegisterClass(&wc)) 
// 		return; 

}
void CMainFrame::OnMenuExit(UINT,int,HWND)
{
	this->OnClose();
}
void CMainFrame::OnClose()
{
//	if( (NULL != g_pLog && g_pLog->IsDirty()) )
	if( NULL != m_pPanelProjectTree && this->m_pPanelProjectTree->SendMessage(WM_GET_APP_DIRTY,0,0) )
	{
		if( IDOK == ::MessageBox( NULL, _T("是否保存所有文件?"), _T("关闭"), MB_OKCANCEL|MB_ICONQUESTION ) )
		{
			::UI_Save();

			if( NULL != g_pLog && g_pLog->IsDirty() )
			{
				if(  0 == g_pLog->Save() )
				{
					::MessageBox( NULL, _T("log save failed."), _T("Error"), MB_OK|MB_ICONERROR );
				}
			}
		}
	}

	::OutputDebugString( _T("TODO:日志对象未考虑保存") );
	
	this->DestroyWindow();
}
void CMainFrame::OnDestroy()
{
	_Module.GetMessageLoop()->RemoveMessageFilter(this);
	PostQuitMessage(0);
}
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 修改为使用 acce 快捷键来实现
// 	if( pMsg->message == WM_KEYDOWN )
// 	{
// 		UINT nChar = pMsg->wParam;
// 		if( nChar == 'S' )
// 		{
// 			SHORT s = GetKeyState(VK_CONTROL);
// 			int bCtrlDown = (s >> 8)& 0xff;
// 			if( bCtrlDown )
// 			{
// 				this->OnMenuSave(0,0,0);
//	 			return TRUE;
// 			}
// 		} 
// 	}

	return FALSE;
}
//
//	设置菜单有有效性
//
void CMainFrame::OnInitMenuPopup(HMENU hMenuPopup, UINT nIndex, BOOL bSysMenu)
{
	int nCount = ::GetMenuItemCount(hMenuPopup);
	for (int i = 0; i < nCount; i++)
	{
		int nCommand = ::GetMenuItemID(hMenuPopup,i);
		switch(nCommand)
		{
		case ID_NEW_SKIN:
		case ID_NEW_FILE:
			{
				if( m_bProjectOpened )
					::EnableMenuItem( hMenuPopup, nCommand, MF_ENABLED|MF_BYCOMMAND );
				else
					::EnableMenuItem( hMenuPopup, nCommand, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			}
			break;

		case ID_NEW_PROJECT:
			{
				if( m_bProjectOpened )
					::EnableMenuItem( hMenuPopup, nCommand, MF_GRAYED|MF_DISABLED );
				else
					::EnableMenuItem( hMenuPopup, nCommand, MF_ENABLED );
			}
			break;

		case IDM_SAVE:
			{
				UINT nFlag = MF_ENABLED;
				if(false == m_bProjectOpened) 
					nFlag = MF_GRAYED|MF_DISABLED;
				else
				{
					if( NULL != m_pPanelProjectTree && 0 == m_pPanelProjectTree->SendMessage(WM_GET_APP_DIRTY,0,0) )
						nFlag = MF_GRAYED|MF_DISABLED;
				}

				::EnableMenuItem(hMenuPopup, nCommand, nFlag);
			}
			break;

		case ID_MENU_MDI_MINIMIZE:
		case ID_MENU_MDI_RESTORE:
		case ID_MENU_MDI_CLOSE:
			{
				BOOL bMax = FALSE; 
				HWND hWnd = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax);
				if( NULL != hWnd && bMax )
					::EnableMenuItem( hMenuPopup, nCommand, MF_ENABLED );
				else
					::EnableMenuItem( hMenuPopup, nCommand, MF_GRAYED|MF_DISABLED );
			}
			break;

		case ID_WINDOW_ARRANGE:
		case ID_WINDOW_CASCADE:
		case ID_WINDOW_TILE_HORZ:
			break;
		}
	}

}

void CMainFrame::OnMenuNewProject( UINT,int,HWND )
{
	if( m_bProjectOpened )
	{
		::MessageBox(NULL, _T("请先关闭当前工程"), _T("Error"), MB_OK|MB_ICONWARNING );
		return;
	}

	CNewProjectDlg dlg(true);
	if( IDCANCEL == dlg.DoModal() )
		return;

	if( NULL != m_pPanelProjectTree )
	{
		delete m_pPanelProjectTree;
		m_pPanelProjectTree = NULL;
	}
	CRect  rc;
	this->GetClientRect(&rc);

	if( NULL != m_pDlgFirstPage )
	{
		m_pDlgFirstPage->ShowWindow(SW_HIDE);
		m_pDlgFirstPage->DestroyWindow();
		delete m_pDlgFirstPage;
		m_pDlgFirstPage = NULL;
	}

	if( NULL == m_pPanelProjectTree )
	{
		m_pPanelProjectTree= new CProjectTreePanel(this);   
		m_pPanelProjectTree->Create(m_hWnd);                // 在窗口的oninitialize中会初始化树
	}

	this->CalcLayout(rc.Width(),rc.Height());
	m_pPanelProjectTree->ShowWindow(SW_SHOW);

	this->m_bProjectOpened = true;

//	在创建的时候，默认已经保存一份
//	m_pPanelProjectTree->SendMessage(WM_SET_DIRTY, MAKE_RES_DATA(1,UI_RESOURCE_PROJECT),NULL );
}

void CMainFrame::OnMenuOpenProject( UINT,int,HWND )
{
	if (m_bProjectOpened)
	{
		::MessageBox(NULL, _T("请先关闭当前工程"), _T("Error"), MB_OK|MB_ICONWARNING );
		return;
	}

	CFileDialog  dlg(TRUE, NULL, NULL, 4|2,  
		_T("ui project file(*.uiproj)\0*.uiproj\0")
		, m_hWnd);
	if( IDCANCEL == dlg.DoModal() )
		return;

	String strProjPath = dlg.m_szFileName;
	this->_OpenProject(strProjPath);
}

void CMainFrame::_OpenProject(String strProjPath)
{
	if( false == ::UI_OpenProject(strProjPath) )
	{
		::MessageBox(NULL, _T("打开工程失败"), _T("Error"), MB_OK|MB_ICONERROR );
		return ;
	}

	// 加载日志
	TCHAR szPath[MAX_PATH] = _T("");
	::UI_GetProjectPath( szPath );
	if( NULL != g_pLog )
	{
		if( FALSE == g_pLog->Load( _bstr_t(szPath) ) )
		{
			::MessageBox( NULL, _T("加载日志模块失败"), _T("Error"), MB_OK|MB_ICONERROR );
		}
	}


	CRect  rc;
	this->GetClientRect(&rc);

	if( NULL != m_pPanelProjectTree )
	{
		delete m_pPanelProjectTree;
		m_pPanelProjectTree = NULL;
	}
	if( NULL != m_pDlgFirstPage )
	{
		m_pDlgFirstPage->ShowWindow(SW_HIDE);
		m_pDlgFirstPage->DestroyWindow();
		delete m_pDlgFirstPage;
		m_pDlgFirstPage = NULL;
	}

	if( NULL == m_pPanelProjectTree )
	{
		m_pPanelProjectTree= new CProjectTreePanel(this);
		m_pPanelProjectTree->Create(m_hWnd);
	}

	this->CalcLayout(rc.Width(),rc.Height());
	m_pPanelProjectTree->ShowWindow(SW_SHOW);

	this->m_bProjectOpened = true;
}

void CMainFrame::OnMenuNewSkin( UINT,int,HWND )
{
	CNewProjectDlg dlg(false);
	if( IDCANCEL == dlg.DoModal() )
		return;

	m_pPanelProjectTree->InsertSkinItem(dlg.m_hSkin);
}

void CMainFrame::OnMenuSave(UINT,int,HWND)
{
	if( !m_bProjectOpened )
		return;

	assert(0 && _T("未处理共享XML时的保存问题")); // TODO:
	return;
	UI_Save();

	// 清除dirty
	if( NULL != m_pPanelProjectTree )
	{
		m_pPanelProjectTree->SendMessage(WM_CLEAR_DIRTY,0,0);
	}
}
void CMainFrame::OnMenuMDIWindow( UINT,int id,HWND )
{
	switch(id)
	{
	case ID_WINDOW_CASCADE:
		::SendMessage(m_hWndMDIClient, WM_MDICASCADE, MDITILE_ZORDER, 0);
		break;
	case ID_WINDOW_TILE_HORZ:
		::SendMessage(m_hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
		break;
	case ID_WINDOW_ARRANGE:
		::SendMessage(m_hWndMDIClient, WM_MDIICONARRANGE, 0, 0);
		break;
	case ID_MENU_MDI_CLOSE:
		{
			BOOL bMaximized = FALSE;
			HWND hWndChild = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);
			if( NULL != hWndChild )
			{
				::SendMessage(hWndChild, WM_SYSCOMMAND, SC_CLOSE, 0L);
			}
		}
		break;
	case ID_MENU_MDI_MINIMIZE:
		{
			BOOL bMaximized = FALSE;
			HWND hWndChild = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);
			if( NULL != hWndChild )
			{
				::SendMessage(hWndChild, WM_SYSCOMMAND, SC_MINIMIZE, 0L);
			}
		}
		break;
	case ID_MENU_MDI_RESTORE:
		{
			BOOL bMaximized = FALSE;
			HWND hWndChild = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);
			if( NULL != hWndChild )
			{
				::SendMessage(hWndChild, WM_SYSCOMMAND, SC_RESTORE, 0L);
			}
		}
		break;
	}
}

void CMainFrame::OnSize(UINT nType, CSize size)
{
	this->CalcLayout( size.cx, size.cy );
}
void CMainFrame::CalcLayout( int cx, int cy )
{
	int nLeftW = 0;
	if( NULL != m_pPanelProjectTree )
	{
		m_pPanelProjectTree->SetWindowPos( NULL, 0,0, LEFT_TREE_WINDOW_WIDTH, cy, SWP_NOZORDER );
		nLeftW = LEFT_TREE_WINDOW_WIDTH;
	}
	if( NULL != m_hWndMDIClient )
	{
		::SetWindowPos( m_hWndMDIClient, NULL, nLeftW, 0, cx-nLeftW, cy ,SWP_NOZORDER );
	}
	if ( NULL != m_pDlgFirstPage )
	{
		m_pDlgFirstPage->CenterWindow();
	}
}

LRESULT CMainFrame::OnEraseBkgnd( HDC hDC )
{
// 	HBRUSH hBrush = (HBRUSH)::GetStockObject( GRAY_BRUSH );
// 	CRect  rc;
// 	this->GetClientRect(&rc);
// 	::FillRect(hDC, &rc, hBrush );
// 	return TRUE;

	if(m_hWndMDIClient != NULL)   // view will paint itself instead
		return 1;

	SetMsgHandled(FALSE);
	return 0;
}

LRESULT CMainFrame::OnSetDirty(UINT u,WPARAM w,LPARAM l)
{
	if( NULL != m_pPanelProjectTree )
		return this->m_pPanelProjectTree->SendMessage(u,w,l);

	return 0;
}
LRESULT CMainFrame::OnGetDirty(UINT u,WPARAM w,LPARAM l)
{
	if( NULL != m_pPanelProjectTree )
		return this->m_pPanelProjectTree->SendMessage(u,w,l);

	return 0;
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int nCount = ::DragQueryFile(hDropInfo, UINT(-1), NULL, 0 );
	if( nCount > 1)
		return;

	TCHAR szPath[256] = _T("");
	::DragQueryFile(hDropInfo, 0, szPath, 256 );

	// 文件格式判断
	String strExt  = XML_PROJECT_XMLFILE_EXT;
	String strPath = szPath;
	int    nLength = strExt.length();

	if( strPath.substr(strPath.length()-nLength,nLength) != strExt )
		return;

	this->_OpenProject(szPath);
}
//
//	切换到皮肤资源窗口
//
//	Parameter
//		nType
//			[in]	资源类型
//
//		hSkin
//			[in]	当前资源所属皮肤 
//
//
void CMainFrame::SwitchToSkinResPanel( UI_RESOURCE_TYPE nType, HSKIN hSkin )
{
	TCHAR* pszDataSource = NULL;
	int    nSize = 0;
	nSize = ::UI_GetResourceDataSourcePath( hSkin, nType, NULL, 0 );
	pszDataSource = new TCHAR[nSize];
	nSize = ::UI_GetResourceDataSourcePath( hSkin, nType, pszDataSource, nSize );

	list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
	list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
	for ( ; iter != iterEnd; iter++ )
	{
		CChildFrame* p = *iter;
		if( p->GetHSkin() == hSkin && p->GetResourceType() == nType )
		{
			// 该窗口已经存在，激活它
			::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)p->m_hWnd, NULL );
			return;
		}

		// 如果这个类型的资源xml和其它已打开的资源共享了同一个文件，则不允许打开
		TCHAR* pszDataSourceThis = NULL;
		int    nSize = 0;
		nSize = ::UI_GetResourceDataSourcePath( p->GetHSkin(), p->GetResourceType(), NULL, 0 );
		pszDataSourceThis = new TCHAR[nSize];
		nSize = ::UI_GetResourceDataSourcePath( p->GetHSkin(), p->GetResourceType(), pszDataSourceThis, nSize );

		if( 0 == _tcscmp( pszDataSource, pszDataSourceThis) )
		{
			delete [] pszDataSourceThis;
			pszDataSourceThis = NULL;

			delete [] pszDataSource;
			pszDataSource = NULL;
			
			::MessageBox( m_hWnd, _T("文件冲突，无法打开"), _T("Warning"), MB_OK|MB_ICONASTERISK );
			return;
		}
		delete [] pszDataSourceThis;
		pszDataSourceThis = NULL;
	}
	delete [] pszDataSource;
	pszDataSource = NULL;

	switch( nType )
	{
	case UI_RESOURCE_IMAGE:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CImageBuildDlg* pView = new CImageBuildDlg;          // 自释放,OnFinalMessage

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->AttachSkin(hSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetView(pView->m_hWnd, hSkin, nType);
			this->AddChildFrame(pChildFrame);
		}
		break;
	case UI_RESOURCE_COLOR:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);      // 自释放,OnFinalMessage
			CColorBuildDlg* pView = new CColorBuildDlg;            // 自释放,OnFinalMessage

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->AttachSkin(hSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetView(pView->m_hWnd, hSkin, nType);
			this->AddChildFrame(pChildFrame);
		}
		break;
	case UI_RESOURCE_FONT:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CFontBuildDlg* pView = new CFontBuildDlg;            // 自释放,OnFinalMessage

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->AttachSkin(hSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetView(pView->m_hWnd, hSkin, nType);
			this->AddChildFrame(pChildFrame);
		}
		break;

 	case UI_RESOURCE_STYLE:
 		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CStyleBuildDlg* pView = new CStyleBuildDlg;          // 自释放,OnFinalMessage

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->AttachSkin(hSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetView(pView->m_hWnd, hSkin, nType);
			this->AddChildFrame(pChildFrame);
 		}
 		break;
	case UI_RESOURCE_LAYOUT:
		{
			CChildFrame* pChildFrame = new CChildFrame(this);    // 自释放,OnFinalMessage
			CLayoutBuildDlg* pView = new CLayoutBuildDlg;        // 自释放,OnFinalMessage

			pChildFrame->Create(m_hWndMDIClient);
			pView->Create(pChildFrame->m_hWnd);
			pView->AttachSkin(hSkin);
			pView->ShowWindow(SW_SHOW);

			pChildFrame->SetView(pView->m_hWnd, hSkin, nType);
			this->AddChildFrame(pChildFrame);
		}
		break;
	}
}
void CMainFrame::RemoveChildFrame( CChildFrame* pChildFrame )
{
	list<CChildFrame*>::iterator iter = m_listChildFrame.begin();
	list<CChildFrame*>::iterator iterEnd = m_listChildFrame.end();
	for ( ; iter != iterEnd; iter++ )
	{
		CChildFrame* pThis = *iter;
		if( pThis == pChildFrame )
		{
			m_listChildFrame.erase(iter);
			break;
		}
	}
}
void CMainFrame::AddChildFrame( CChildFrame* pChildFrame )
{
	m_listChildFrame.push_back(pChildFrame);
}

//
//	切换到日志窗口
//
//	Parameter
//		nType
//			[in]	切换切换，如创建，修改
//
//		strModuleName
//			[in]	日志模块
//
void CMainFrame::SwitchToLogPanel( int nType, const String& strModuleName )
{
	//switch(nType)
	{
// 	case UI_RESOURCE_LOG:        // 新建一个模块
// 	case UI_RESOURCE_LOGMODULE:  // 修改一个
// 		{
// 			if (NULL == m_pLogViewDlg)
// 			{
// 				m_pLogViewDlg = new CLogViewDlg(m_pPanelProjectTree->m_hWnd);
// 				m_pLogViewDlg->Create(m_hWnd);
// 			}
// 			if( m_pLogViewDlg->m_hWnd == m_hPanelCurVisible )
// 				break;
// 
// 			CRect  rc;
// 			this->GetClientRect( &rc );
// 			m_pLogViewDlg->SetWindowPos( 0, LEFT_TREE_WINDOW_WIDTH, rc.top, rc.Width()-LEFT_TREE_WINDOW_WIDTH, rc.Height() , SWP_NOZORDER|SWP_SHOWWINDOW );
// 			if( UI_RESOURCE_LOG == nType )
// 				m_pLogViewDlg->AttachLogItem( _T("") );
// 			else
// 				m_pLogViewDlg->AttachLogItem( strModuleName );
// 
// 			if( NULL != m_hPanelCurVisible )
// 				::ShowWindow( m_hPanelCurVisible, SW_HIDE );
// 			m_hPanelCurVisible = m_pLogViewDlg->m_hWnd;
// 		}
// 		break;

	
	}
}

CChildFrame::CChildFrame(CMainFrame* pMainFrame):m_pMainFrame(pMainFrame)
{
	m_hSkin = NULL;
	m_eResType = UI_RESOURCE_NULL;

	m_bDirty = false;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	return 0;
}

void CChildFrame::OnDestroy()
{
	m_pMainFrame->RemoveChildFrame(this);
	return;
}
void CChildFrame::SetDirty(bool b)
{
	bool bOld = m_bDirty;
	m_bDirty = b;
	if( bOld != m_bDirty )
	{
		this->UpdateTitle();
	}

	// ->MDIClient,->MAINFRAME
	this->GetParent().GetParent().SendMessage( WM_SET_DIRTY, MAKEWPARAM(m_eResType,b?1:0), (LPARAM)m_hSkin );  // 通知
}


void CChildFrame::SetView(HWND hView, HSKIN hSkin, UI_RESOURCE_TYPE eResType )
{
	m_hWndClient = hView;
	m_eResType = eResType;
	m_hSkin = hSkin;

	// ->MDIClient,->MAINFRAME
	LRESULT lr = this->GetParent().GetParent().SendMessage( WM_GET_DIRTY, (WPARAM)m_eResType, (LPARAM)m_hSkin );  // 通知
	m_bDirty = (lr==1?true:false);

	this->UpdateTitle();
}

//////////////////////////////////////////////////////////////////////////
//
// 设置标题栏
//
void CChildFrame::UpdateTitle()
{
	if( NULL == m_hSkin || m_eResType == UI_RESOURCE_NULL )
		return;

	String strTitle;

	switch(m_eResType)
	{
	case UI_RESOURCE_IMAGE:
		strTitle.append(_T("image builder - "));
		break;

	case UI_RESOURCE_COLOR:
		strTitle.append(_T("color builder - "));
		break;

	case UI_RESOURCE_FONT:
		strTitle.append(_T("font builder - "));
		break;

	case UI_RESOURCE_LAYOUT:
		strTitle.append(_T("layout builder - "));
		break;

	case UI_RESOURCE_STYLE:
		strTitle.append(_T("style builder - "));
		break;

	case UI_RESOURCE_I18N:
		strTitle.append(_T("i18n builder - "));
		break;
	}

	TCHAR* pszName = NULL;
	int    nSize = 0;

	nSize = ::UI_GetSkinName( m_hSkin, NULL, 0 );
	pszName = new TCHAR[nSize];

	nSize = ::UI_GetSkinName( m_hSkin, pszName, nSize );
	strTitle.append(pszName);
	delete [] pszName;
	pszName = NULL;

	if( m_bDirty )
		strTitle += _T("*");

	this->SetWindowText(strTitle.c_str());
	this->UpdateLayout();
}

LRESULT CChildFrame::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LPMSG pMsg = (LPMSG)lParam;

	if(CMDIChildWindowImpl<CChildFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return FALSE/*m_view.PreTranslateMessage(pMsg)*/;
}

LRESULT CChildFrame::OnSetDirty(UINT, WPARAM w, LPARAM)
{
	this->SetDirty( w == 0 ? false:true );
	return 0;
}
LRESULT CChildFrame::OnGetDirty(UINT, WPARAM w, LPARAM)
{
	if (m_bDirty)
		return 1;
	else
		return 0;
}
