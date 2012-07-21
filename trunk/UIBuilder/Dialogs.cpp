#include "StdAfx.h"
#include "Dialogs.h"
#include "MainFrame.h"

void Split( const String& str, TCHAR szSep, vector<String>& vRet );

CFirstPageDlg::CFirstPageDlg( CMainFrame* pMainFrame )
{
	m_pMainFrame = pMainFrame;
}

void CFirstPageDlg::OnBtnNewProject(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if( NULL == m_pMainFrame )
		return;

	m_pMainFrame->PostMessage( WM_COMMAND, ID_NEW_PROJECT );
}

void CFirstPageDlg::OnBtnOpenProject(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_pMainFrame->PostMessage( WM_COMMAND, ID_OPEN_PROJECT );
}

//////////////////////////////////////////////////////////////////////////
//
//   CNewProjectDlg

CNewProjectDlg::CNewProjectDlg(bool bCreateProject):m_bCreateProject(bCreateProject)
{
	m_hSkin = NULL;
	m_strProjectFolder = _T("C:");
}
CNewProjectDlg::~CNewProjectDlg()
{

}
BOOL CNewProjectDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	this->CenterWindow();
	
	if(m_bCreateProject)
	{
#ifdef _DEBUG
	m_strProjectFolder = _T("d:\\我的文档\\桌面\\test");//_T("C:\\Users\\Administrator\\Desktop\\test");
	SetDlgItemText( IDC_EDIT_NEW_PROJECT_NAME, _T("ttplayer"));
	SetDlgItemText( IDC_EDIT_NEW_SKIN_NAME, _T("mediaplayer"));
#endif

		this->SetWindowText(_T("新建工程"));
		GetDlgItem(IDC_EDIT_NEW_PROJECT_NAME).SetFocus();
	}
	else  // 为这个工程创建一个皮肤
	{
		//
		// 获取工程名，并disable
		//

		TCHAR* pszProjName = NULL;
		int    nSize = 0;
		::UI_GetProjectName(NULL, &nSize);
		pszProjName = new TCHAR[nSize];
		::UI_GetProjectName(pszProjName,&nSize);
		SetDlgItemText( IDC_EDIT_NEW_PROJECT_NAME, pszProjName );
		delete[] pszProjName;
		pszProjName = NULL;

		TCHAR pszProjDir[MAX_PATH] = _T("");
		::UI_GetProjectDirectory(pszProjDir);
		m_strProjectFolder = pszProjDir;

		this->SetWindowText(_T("新建皮肤"));
		GetDlgItem(IDC_EDIT_NEW_SKIN_NAME).SetFocus();
	}

	GetDlgItem(IDC_EDIT_NEW_PROJECT_NAME).SendMessage(EM_SETREADONLY, m_bCreateProject? FALSE:TRUE, 0L );

	this->SetDlgItemText( IDC_EDIT_BROWSE, m_strProjectFolder ); 
	CString strLogPath = m_strProjectFolder + _T("\\log.xml");
	this->SetDlgItemText( IDC_EDIT_LOG_XML_FILE, strLogPath );

	return FALSE;
}

void CNewProjectDlg::OnClose()
{
	this->EndDialog(IDCANCEL);
}
void CNewProjectDlg::OnOk( UINT,int,HWND )
{
	CString cstrProjName;
	this->GetDlgItemText( IDC_EDIT_NEW_PROJECT_NAME, cstrProjName );
	if( cstrProjName.IsEmpty() )
	{
		::MessageBox( NULL,_T("工程名称不能为空"), _T(""), 0);
		return;
	}

	CString  cstrSkinName;
	this->GetDlgItemText( IDC_EDIT_NEW_SKIN_NAME, cstrSkinName );
	if( cstrSkinName.IsEmpty() )
	{
		::MessageBox( NULL,_T("皮肤名称不能为空"), _T(""), 0);
		return;
	}

	String  strProjName( (LPCTSTR)cstrProjName );
	String  strSkinName( (LPCTSTR)cstrSkinName );
	String  strProjFolder( (LPCTSTR) m_strProjectFolder );

	if( m_bCreateProject )
	{
		// 1. 创建工程
		if( false == ::UI_CreateProject( strProjFolder, strProjName)  )
		{
			::MessageBox( NULL,_T("创建工程失败"), _T(""), MB_OK|MB_ICONERROR);
			return;
		}

		// 2. 创建日志文件
		//CString cstrLogPath;
		//this->GetDlgItemText( IDC_EDIT_LOG_XML_FILE, cstrLogPath );
		if( /*!cstrLogPath.IsEmpty() &&*/ NULL != g_pLog )
		{
			//_bstr_t bstrLogPath = cstrLogPath.GetString();
			TCHAR szPath[MAX_PATH] = _T("");
			::UI_GetProjectPath(szPath);
			_bstr_t bstrLogPath = szPath;
			if( 0 == g_pLog->Create( bstrLogPath ) )
			{
				::MessageBox( NULL,_T("创建日志失败"), _T(""), MB_OK|MB_ICONERROR);
			}
		}

	}

	// 3. 添加皮肤文件
	CString cstrXmlPath;
	GetDlgItemText( IDC_EDIT_IMAGE_XML_FILE, cstrXmlPath );
	String  strImageXmlPath( (LPCTSTR) cstrXmlPath );
	GetDlgItemText( IDC_EDIT_COLOR_XML_FILE, cstrXmlPath );
	String  strColorXmlPath = (LPCTSTR)cstrXmlPath;
	GetDlgItemText( IDC_EDIT_FONT_XML_FILE, cstrXmlPath );
	String  strFontXmlPath = (LPCTSTR)cstrXmlPath;
	GetDlgItemText( IDC_EDIT_LAYOUT_XML_FILE, cstrXmlPath );
	String  strLayoutXmlPath = (LPCTSTR)cstrXmlPath;
	GetDlgItemText( IDC_EDIT_STYLE_XML_FILE, cstrXmlPath );
	String  strStyleXmlPath = (LPCTSTR)cstrXmlPath;
	

	HSKIN hSkinXml = NULL;
	if( NULL == (hSkinXml = ::UI_CreateSkin( strSkinName, strImageXmlPath, strColorXmlPath, strFontXmlPath, strLayoutXmlPath, strStyleXmlPath )) )
	{
		::MessageBox( NULL,_T("创建皮肤失败"), _T(""), MB_OK|MB_ICONERROR);
		return;
	}
	
	m_hSkin = hSkinXml;
	// 设置为当前皮肤
	if( m_bCreateProject )
	{
		::UI_SetActiveSkin( hSkinXml );
	}

	this->EndDialog(IDOK);
}
void CNewProjectDlg::OnCancel( UINT,int,HWND )
{
	this->EndDialog(IDCANCEL);
}

void CNewProjectDlg::OnBrowse( UINT, int, HWND )
{
	CFolderDialog  dlg;
	if( IDCANCEL == dlg.DoModal( ))
		return;

	m_strProjectFolder = dlg.GetFolderPath();
	::SetDlgItemText(m_hWnd, IDC_EDIT_BROWSE, m_strProjectFolder );

	// 同步路径
	this->OnEnChange_NewSkinName(0,0,0);
	return;

}

// 修改了皮肤名称
void CNewProjectDlg::OnEnChange_NewSkinName(UINT, int, HWND )
{
	CString  strSkinName;
	this->GetDlgItemText( IDC_EDIT_NEW_SKIN_NAME, strSkinName );

	CString  strBase, strName;
	strBase = m_strProjectFolder + _T("\\") + strSkinName + _T("\\");
	
	strName = strBase + _T("UI.xml");
	this->SetDlgItemText( IDC_EDIT_UI_XML_FILE, strName );
	
	strName = strBase + _T("image.xml");
	this->SetDlgItemText( IDC_EDIT_IMAGE_XML_FILE, strName );

	strName = strBase + _T("font.xml");
	this->SetDlgItemText( IDC_EDIT_FONT_XML_FILE, strName );

	strName = strBase + _T("color.xml");
	this->SetDlgItemText( IDC_EDIT_COLOR_XML_FILE, strName );

	strName = strBase + _T("layout.xml");
	this->SetDlgItemText( IDC_EDIT_LAYOUT_XML_FILE, strName );

	strName = strBase + _T("style.xml");
	this->SetDlgItemText( IDC_EDIT_STYLE_XML_FILE, strName );
}

//////////////////////////////////////////////////////////////////////////
//
//
CNewImageDlg::CNewImageDlg( HSKIN hSkin, bool bModifyMode )
{
	m_hSkin = hSkin;
	m_bModifyMode = bModifyMode;
	m_bFirstTime = true;
}
CNewImageDlg::~CNewImageDlg()
{
	m_editID.Detach();
	m_editPath.Detach();
}

void CNewImageDlg::OnPaint(HDC)
{
	if( m_bFirstTime )
	{
		m_bFirstTime = false;
		this->PostMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_BROWSE,BN_CLICKED), (LPARAM)(HWND)GetDlgItem(IDC_BUTTON_BROWSE) );
	}
	SetMsgHandled(FALSE);
}
BOOL CNewImageDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_editID.Attach( GetDlgItem(IDC_EDIT_ID) );
	m_editID.SetLimitText(MAX_STRING_LEN);

	m_editPath.Attach( GetDlgItem(IDC_EDIT_PATH) );
	m_editPath.SetLimitText(MAX_PATH);

	// 获取xml所在路径，用于拷贝文件
	String strDataSource;
	TCHAR* pszDataSource = NULL;
	int    nSize = 0;

	nSize = ::UI_GetResourceDataSourcePath( m_hSkin, UI_RESOURCE_IMAGE, NULL, 0 );
	pszDataSource = new TCHAR[nSize];

	nSize = ::UI_GetResourceDataSourcePath( m_hSkin, UI_RESOURCE_IMAGE, pszDataSource, nSize );
	strDataSource = pszDataSource ;
	delete [] pszDataSource;
	pszDataSource = NULL;

	if( nSize > 0 )
	{
		String::size_type  pos = strDataSource.rfind( _T('\\') );
		m_strDataSourceFolder = strDataSource.substr( 0, pos );

		GetDlgItem(IDC_EDIT_XML_PATH).SetWindowText( m_strDataSourceFolder.c_str() );
	}
	else
	{
		::CheckDlgButton( m_hWnd, IDC_CHECK_COPYFILE, BST_UNCHECKED );
		GetDlgItem(IDC_CHECK_COPYFILE).EnableWindow(FALSE);
	}

	if( m_bModifyMode )
	{
		m_editID.SetWindowText(m_strId.c_str());
		m_editPath.SetWindowText(m_strPath.c_str());

		m_editID.SetReadOnly(TRUE);
	}
	this->CenterWindow();
	return TRUE;
}
void CNewImageDlg::SetImageInfo( const String& strID, const String& strPath )
{
	m_strId = strID;
	m_strPath = strPath;
}

void CNewImageDlg::OnCancel( UINT,int,HWND	)
{
	EndDialog(IDCANCEL);
}
void CNewImageDlg::OnOk( UINT,int,HWND	)
{
	TCHAR  szID[MAX_STRING_LEN]    = _T("");
	TCHAR  szPath[MAX_PATH]  = _T("");

	::GetDlgItemText( m_hWnd, IDC_EDIT_ID, szID, MAX_STRING_LEN );
	::GetDlgItemText( m_hWnd, IDC_EDIT_PATH, szPath, MAX_PATH );
	
	if ( _tcslen(szID) == 0 || _tcslen(szPath) == 0 )
	{
		return;
	}
	if( ! ::PathFileExists( szPath) )
	{
		::MessageBox( NULL, _T("文件不存在"), _T("Error"), MB_OK|MB_ICONERROR );
		return;
	}

	m_strId = szID;
	m_strPath = szPath;

	if ( BST_CHECKED == ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_COPYFILE ) )
	{
		String::size_type  pos = m_strPath.rfind( _T('\\') );
		String    strFileName  = m_strPath.substr( pos, m_strPath.length()-pos );
		String    strNewPath   = m_strDataSourceFolder + strFileName;

		if( FALSE == ::CopyFile( szPath, strNewPath.c_str(), TRUE ) )
		{
			::MessageBox( NULL, _T("拷贝文件失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}

		m_strPath = strNewPath;
	}

//	if( false == g_uiBuilderMgr.InsertImageItem( m_hSkin, m_strId, m_strPath ) )
	if( m_bModifyMode )
	{
		if( false == ::UI_ModifyImageItem( m_hSkin, m_strId, m_strPath ))
		{
			::MessageBox( NULL, _T("修改失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	else
	{
		if( false == ::UI_InsertImageItem( m_hSkin, m_strId, m_strPath ))
		{
			::MessageBox( NULL, _T("添加失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	EndDialog(IDOK);
}

void CNewImageDlg::OnBrowse( UINT, int, HWND )
{
	CFileDialog  dlg(TRUE, NULL, NULL, 4|2,  
		_T("Support Image Format(*.png,*.bmp,*.jpg,*.ico)\0*.png;*.bmp;*.jpg;*.ico\0")
		_T("Icon (*.ico)\0*.ico\0Bitmap (*.bmp)\0*.bmp\0jpeg (*.jpg)\0*.jpg\0png (*.png)\0*.png\0\0")
		, m_hWnd);

	if( IDCANCEL == dlg.DoModal() )
		return;

	::SetDlgItemText( m_hWnd, IDC_EDIT_PATH, dlg.m_szFileName );

	// 默认ID值
// 	TCHAR szText[MAX_STRING_LEN];
// 	m_editID.GetWindowText(szText, MAX_STRING_LEN);
// 	if( 0 == _tcslen(szText) )
	{
		if( !m_bModifyMode )   // 修改模式下，禁止同步ID 
			m_editID.SetWindowText( dlg.m_szFileTitle );
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
CNewColorDlg::CNewColorDlg( HSKIN hSkin, bool bModifyMode )
{
	m_hSkin = hSkin;
	m_bModifyMode = bModifyMode;
}
CNewColorDlg::~CNewColorDlg()
{
}

BOOL CNewColorDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	this->CenterWindow();
	if( m_bModifyMode )
	{
		// 修改模式下，暂时不能修改其ID
		CEdit edit;
		edit.Attach(GetDlgItem(IDC_EDIT_ID));
		edit.SetWindowText(m_strId.c_str());
		edit.SetReadOnly(TRUE);
		edit.Detach();

		::SetDlgItemText( m_hWnd, IDC_EDIT_ID, m_strId.c_str() );
		::SetDlgItemText( m_hWnd, IDC_EDIT_COLOR, m_strColor.c_str() );
	}
	else
	{

	}
	return TRUE;
}

void CNewColorDlg::SetColorInfo( TCHAR* szColorName, TCHAR* szColorValue )
{
	m_strId = szColorName;
	m_strColor = szColorValue;
}

void CNewColorDlg::OnChooseColor( UINT,int,HWND	)
{
	CColorDialog  dlg;
	if( IDOK == dlg.DoModal() )
	{
		TCHAR szText[32] = _T("");
		COLORREF c = dlg.GetColor();

		_stprintf(szText, _T("%d,%d,%d"), GetRValue(c),GetGValue(c),GetBValue(c));
		::SetDlgItemText( m_hWnd, IDC_EDIT_COLOR, szText );
	}
}

void CNewColorDlg::OnOk( UINT,int,HWND	)
{
	TCHAR  szID[MAX_STRING_LEN]    = _T("");
	TCHAR  szColor[MAX_STRING_LEN]  = _T("");

	::GetDlgItemText( m_hWnd, IDC_EDIT_ID, szID, MAX_STRING_LEN );
	::GetDlgItemText( m_hWnd, IDC_EDIT_COLOR, szColor, MAX_STRING_LEN );

	m_strId = szID;
	m_strColor = szColor;

	if( m_strId.empty() || m_strColor.empty() )
	{
		return;
	}
	if( m_bModifyMode )
	{
		if( false == UI_ModifyColorItem( m_hSkin, m_strId, m_strColor ))
		{
			::MessageBox( NULL, _T("修改失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	else
	{
		if( false == ::UI_InsertColorItem( m_hSkin, m_strId, m_strColor ))
		{
			::MessageBox( NULL, _T("添加失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	EndDialog(IDOK);

	return;
}
void CNewColorDlg::OnCancel( UINT,int,HWND	)
{
	EndDialog(IDCANCEL);
}


//////////////////////////////////////////////////////////////////////////
//
//
CNewFontDlg::CNewFontDlg( HSKIN hSkin, bool bModifyMode/*=false*/ )
{
	m_hSkin = hSkin;
	m_nSize = 0;
	m_nOrientation = 0;
	m_bBold = m_bItalic = m_bUnderline = m_bStrikeout = false;
	m_bModifyMode = bModifyMode;
}
CNewFontDlg::~CNewFontDlg()
{
	m_btnBold.Detach();
	m_btnItalic.Detach();
	m_btnUnderline.Detach();
	m_btnStrikeout.Detach();
}

BOOL CNewFontDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	
	m_btnBold.Attach(GetDlgItem(IDC_CHECK_BOLD));
	m_btnItalic.Attach(GetDlgItem(IDC_CHECK_ITALIC));
	m_btnUnderline.Attach(GetDlgItem(IDC_CHECK_UNDERLINE));
	m_btnStrikeout.Attach(GetDlgItem(IDC_CHECK_STRIKEOUT));

	if(m_bModifyMode)
	{
		// 修改模式下，暂时不能修改其ID
		CEdit edit;
		edit.Attach(GetDlgItem(IDC_EDIT_ID));
		edit.SetWindowText(m_strId.c_str());
		edit.SetReadOnly(TRUE);
		edit.Detach();

		::SetDlgItemText( m_hWnd, IDC_EDIT_FACENAME, m_strFaceName.c_str() );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTSIZE, m_nSize, TRUE );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTORIENTATION, m_nOrientation, TRUE );
		if(m_bBold)m_btnBold.SetCheck(BST_CHECKED);
		if(m_bItalic)m_btnItalic.SetCheck(BST_CHECKED);
		if(m_bUnderline)m_btnUnderline.SetCheck(BST_CHECKED);
		if(m_bStrikeout)m_btnStrikeout.SetCheck(BST_CHECKED);
	}
	else
	{
		::SetDlgItemText( m_hWnd, IDC_EDIT_FACENAME, _T("Arial") );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTSIZE, 9, TRUE );
		::SetDlgItemInt( m_hWnd, IDC_EDIT_FONTORIENTATION, 0, TRUE );
	}
	this->CenterWindow();
	return TRUE;
}

void CNewFontDlg::OnOk( UINT,int,HWND	)
{
	TCHAR  szID[MAX_STRING_LEN]    = _T("");
	TCHAR  szFaceName[32]  = _T("");

	::GetDlgItemText( m_hWnd, IDC_EDIT_ID, szID, MAX_STRING_LEN );
	::GetDlgItemText( m_hWnd, IDC_EDIT_FACENAME, szFaceName, 32 );
	m_nSize = ::GetDlgItemInt( m_hWnd, IDC_EDIT_FONTSIZE, NULL, FALSE );
	m_nOrientation = ::GetDlgItemInt( m_hWnd, IDC_EDIT_FONTORIENTATION, NULL, FALSE );

	m_strId = szID;
	m_strFaceName = szFaceName;
	m_bBold = m_btnBold.GetCheck() == BST_CHECKED;
	m_bItalic = m_btnItalic.GetCheck() == BST_CHECKED;
	m_bUnderline = m_btnUnderline.GetCheck() == BST_CHECKED;
	m_bStrikeout = m_btnStrikeout.GetCheck() == BST_CHECKED;

	if( m_strId.empty() || m_strFaceName.empty() )
	{
		return;
	}

	::ZeroMemory(&m_lf,sizeof(LOGFONT));
	
	// 将size转换成height
	_tcsncpy( m_lf.lfFaceName, m_strFaceName.c_str(), 31 );
	m_lf.lfFaceName[31] = 0;
	m_lf.lfOrientation = m_nOrientation;
	m_lf.lfHeight = Util::FontSize2Height(m_nSize);
	m_lf.lfWeight = m_bBold?FW_BOLD:FW_NORMAL;
	m_lf.lfItalic = m_bItalic?TRUE:FALSE;
	m_lf.lfUnderline = m_bUnderline?TRUE:FALSE;
	m_lf.lfStrikeOut = m_bStrikeout?TRUE:FALSE;
	

	if( m_bModifyMode )
	{
		if( false == ::UI_ModifyFontItem( m_hSkin, szID, &m_lf) )
		{
			::MessageBox( NULL, _T("修改失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	else
	{
		if( false == ::UI_InsertFontItem( m_hSkin, szID, &m_lf ) )
		{
			::MessageBox( NULL, _T("添加失败"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}
	}
	EndDialog(IDOK);

	return;
}
void CNewFontDlg::OnCancel( UINT,int,HWND	)
{
	EndDialog(IDCANCEL);
}
//
//	在修改模式下，通过设置该结构体，迅速赋值
//	
void CNewFontDlg::SetFontInfo( const String& strID, LOGFONT* plf)
{
	if (NULL == plf )
		return;

	m_strId = strID;
	m_strFaceName = plf->lfFaceName;
	m_nSize = Util::FontHeight2Size(plf->lfHeight);
	m_nOrientation = plf->lfOrientation;
	m_bBold = plf->lfWeight==FW_BOLD?true:false;
	m_bItalic = plf->lfItalic==1?true:false;
	m_bUnderline = plf->lfUnderline==1?true:false;
	m_bStrikeout = plf->lfStrikeOut==1?true:false;

}

//////////////////////////////////////////////////////////////////////////


CNewStyleDlg::CNewStyleDlg(HSKIN hSkin, bool bModifyMode/* =false */)
{
	m_hSkin = hSkin;
	m_bModifyMode = bModifyMode;
}
CNewStyleDlg::~CNewStyleDlg()
{
	m_comboType.Detach();
	m_editID.Detach();
	m_editInherit.Detach();
	m_listbox_own.Detach();
	m_listbox_not.Detach();
}


BOOL CNewStyleDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_comboType.Attach(GetDlgItem(IDC_COMBO_TYPE));
	m_editID.Attach(GetDlgItem(IDC_EDIT_ID));
	m_editInherit.Attach(GetDlgItem(IDC_EDIT_INHERIT));
	m_listbox_own.Attach(GetDlgItem(IDC_LISTBOX_STYLEITEM_OWN));
	m_listbox_not.Attach(GetDlgItem(IDC_LISTBOX_STYLEITEM_NOT));

	m_comboType.AddString(_T("ID"));
	m_comboType.AddString(_T("CLASS"));
	m_comboType.AddString(_T("TAG"));
	m_comboType.SetItemData(0, (DWORD_PTR)STYLE_SELECTOR_TYPE_ID);
	m_comboType.SetItemData(1, (DWORD_PTR)STYLE_SELECTOR_TYPE_CLASS);
	m_comboType.SetItemData(2, (DWORD_PTR)STYLE_SELECTOR_TYPE_TAG);
	m_comboType.SetCurSel(0);

	// 获取所有的style id进行填充
	int nCount = ::UI_GetSkinStyleCount( m_hSkin );

	TCHAR szText[256] = _T("");
	for( int i = 0; i < nCount; i++ )
	{
		IStyleItemInfo*  pInfo = NULL;
		if( ::UI_GetSkinStyleItemInfo( m_hSkin, i, &pInfo ) && pInfo!=NULL )
		{
			if(m_bModifyMode && pInfo->GetSelectorType() == m_type && pInfo->GetIDRef() == m_strID )
			{
				// 不添加自己本身
			}
			else
			{
				TCHAR szInherit[MAX_STRING_LEN] = _T("");
				IStyleItemInfo::MakeInheritString(pInfo->GetSelectorType(), pInfo->GetIDRef(), szInherit );
				m_mapAllStyle[szInherit] = false;
			}
		}
	}

	//	
	//	
	//
	if( m_bModifyMode )
	{
		m_comboType.EnableWindow(FALSE);
		m_editID.SetReadOnly(TRUE);

		switch(m_type)
		{
		case STYLE_SELECTOR_TYPE_ID:
			m_comboType.SetCurSel(0);
			break;
		case STYLE_SELECTOR_TYPE_CLASS:
			m_comboType.SetCurSel(1);
			break;
		default:
			m_comboType.SetCurSel(2);
			break;
		}

		m_editID.SetWindowText(m_strID.c_str());

		vector<String> vStrArray;
		Split(m_strInherit, XML_SEPERATOR, vStrArray );
		int nCount = (int)vStrArray.size(); 
		if( nCount > 0 )
		{
			for( int i = 0; i < nCount; i++ )
			{
				m_listbox_own.AddString( vStrArray[i].c_str() );
				if( 0 != m_mapAllStyle.count(vStrArray[i]) )
				{
					m_mapAllStyle[vStrArray[i]] = true;
				}
			}
		}
	}

	this->FillAllStyleListBox();

	this->CenterWindow();
	return TRUE;
}

void CNewStyleDlg::SetStyleInfo( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit )
{
	m_type = type;
	m_strID = strID;
	m_strInherit = strInherit;
}

void CNewStyleDlg::FillAllStyleListBox()
{
	m_listbox_not.SetRedraw(FALSE);
	m_listbox_not.ResetContent();

	map<String, bool>::iterator iter = m_mapAllStyle.begin();
	map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
	for (; iter != iterEnd; iter ++ )
	{
		if( false == iter->second )
		{
			m_listbox_not.AddString(iter->first.c_str());
		}
	}
	m_listbox_not.SetRedraw(TRUE);
}

void CNewStyleDlg::OnOk( UINT,int,HWND )
{
	if( m_editInherit.m_hWnd == GetFocus() )
	{
		this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_ADD_INHERIT,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_ADD_INHERIT).m_hWnd);
		return;
	}

	int nCurSel = m_comboType.GetCurSel();
	if( -1 == nCurSel )
		return;

	
	CString strID, strInherit;
	m_editID.GetWindowText(strID);
	if( strID.IsEmpty() )
		return;

	if( strID[0] == STYLE_CLASS_PREFIX || strID[0] == STYLE_ID_PREFIX )
	{
		::MessageBox(m_hWnd, _T("id非法"), _T("Error"), MB_OK|MB_ICONWARNING );
		m_editID.SetFocus();
		return;
	}

	m_type = (STYLE_SELECTOR_TYPE) m_comboType.GetItemData(nCurSel);
	m_strID =  strID.GetString();
	m_strInherit = _T("");

	int nCount = m_listbox_own.GetCount();
	for (int i = 0; i < nCount; i++ )
	{
		CString str;
		m_listbox_own.GetText(i, str);

		if( ! m_strInherit.empty() )
			m_strInherit += XML_SEPERATOR;

		m_strInherit.append(str.GetString());
	}

	if( m_bModifyMode )
	{
		if( !::UI_ModifyStyleItem(m_hSkin, m_type, m_strID, m_strInherit ) )
		{
			::MessageBox(NULL, _T("修改失败"), _T("Error"), MB_OK|MB_ICONWARNING );
		}
		else
		{
			EndDialog(IDOK);
		}
	}
	else
	{
		if( !::UI_InsertStyleItem(m_hSkin, m_type, m_strID, m_strInherit ) )
		{
			::MessageBox(NULL, _T("添加失败"), _T("Error"), MB_OK|MB_ICONWARNING );
		}
		else
		{
			EndDialog(IDOK);
		}
	}
}
void CNewStyleDlg::OnCancel( UINT,int,HWND )
{
	EndDialog(IDCANCEL);
}

void CNewStyleDlg::OnListBoxNotDBClick(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	this->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BUTTON_ADD_INHERIT,BN_CLICKED), (LPARAM)GetDlgItem(IDC_BUTTON_ADD_INHERIT).m_hWnd);
}

void CNewStyleDlg::OnBtnAddInherit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	TCHAR szText[MAX_STRING_LEN] = _T("");

	int nIndex = m_listbox_not.GetCurSel();
	if( -1 != nIndex )
	{
		m_listbox_not.GetText(nIndex, szText);
		m_listbox_not.DeleteString(nIndex);
	}
	// 检查直接从EDIT添加的情况
	else
	{
		CString str;
		m_editInherit.GetWindowText(str);
		str.Trim();
		if( str.IsEmpty() )
			return;

		m_editInherit.SetWindowText(_T(""));
		
		_tcsncpy(szText, str.GetString(), MAX_STRING_LEN-1 );
		szText[MAX_STRING_LEN-1] = _T('');
	}

	if( _tcslen(szText) != 0 )
	{
		m_listbox_own.AddString(szText);

		map<String, bool>::iterator iter = m_mapAllStyle.begin();
		map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
		for (; iter != iterEnd; iter ++ )
		{
			if( 0 == iter->first.compare(szText) )
			{
				iter->second = true;
				break;
			}
		}
		this->FillAllStyleListBox();
	}
}

void CNewStyleDlg::OnBtnDeleteInherit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nIndex = m_listbox_own.GetCurSel();
	if( -1 == nIndex )
		return;

	TCHAR szText[MAX_STRING_LEN] = _T("");
	m_listbox_own.GetText(nIndex, szText);
	m_listbox_own.DeleteString(nIndex);

	map<String, bool>::iterator iter = m_mapAllStyle.begin();
	map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
	for (; iter != iterEnd; iter ++ )
	{
		if( 0 == iter->first.compare(szText) )
		{
			iter->second = false;
			break;
		}
	}
	this->FillAllStyleListBox();
}
void CNewStyleDlg::OnBtnClearInherit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if( 0 == m_listbox_own.GetCount() )
		return;

	m_listbox_own.ResetContent();

	map<String, bool>::iterator iter = m_mapAllStyle.begin();
	map<String, bool>::iterator iterEnd = m_mapAllStyle.end();
	for (; iter != iterEnd; iter ++ )
	{
		iter->second = false;
	}

	this->FillAllStyleListBox();

}
//////////////////////////////////////////////////////////////////////////

CNewStyleAttributeDlg::CNewStyleAttributeDlg( HSKIN hSkin, bool bModifyMode )
{
	m_hSkin = hSkin;
	m_bModifyMode = bModifyMode;
}
CNewStyleAttributeDlg::~CNewStyleAttributeDlg()
{
	m_editKey.Detach();
	m_editValue.Detach();
}

BOOL CNewStyleAttributeDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_editKey.Attach(GetDlgItem(IDC_EDIT_KEY).m_hWnd);
	m_editValue.Attach(GetDlgItem(IDC_EDIT_VALUE).m_hWnd);

	if( m_bModifyMode )
	{
		m_editKey.SetReadOnly(TRUE);

		m_editKey.SetWindowText( m_strKey.c_str() );
		m_editValue.SetWindowText( m_strValue.c_str() );
	}
	else
	{
	}

	this->CenterWindow();
	return TRUE;
}

void CNewStyleAttributeDlg::SetStyleInfo(STYLE_SELECTOR_TYPE type, const String strStyleID)
{
	m_eType = type;
	m_strStyleID = strStyleID;
}
void CNewStyleAttributeDlg::SetAttributeInfo( const String& strKey, const String& strValue )
{
	m_strKey = strKey;
	m_strValue = strValue;
}

void CNewStyleAttributeDlg::OnOk( UINT,int,HWND	)
{
	CString strEditKey, strEditValue;

	m_editKey.GetWindowText(strEditKey);
	m_editValue.GetWindowText(strEditValue);

	strEditKey.Trim();
	strEditValue.Trim();

	if( strEditKey.IsEmpty() || strEditValue.IsEmpty() )
		return;

	m_strKey = strEditKey.GetString();
	m_strValue = strEditValue.GetString();

	if( m_bModifyMode )
	{
		if( false == ::UI_ModifyStyleAttribute(m_hSkin, m_eType, m_strStyleID, m_strKey, m_strValue ) )
		{
			::MessageBox(m_hWnd, _T("修改失败"), _T("Error"), MB_OK|MB_ICONWARNING );
			return;
		}
	}
	else
	{
		if( false == ::UI_InsertStyleAttribute(m_hSkin, m_eType, m_strStyleID, m_strKey, m_strValue ) )
		{
			::MessageBox(m_hWnd, _T("添加失败"), _T("Error"), MB_OK|MB_ICONWARNING );
			return;
		}
	}

	EndDialog(IDOK);
}
void CNewStyleAttributeDlg::OnCancel( UINT,int,HWND	)
{
	EndDialog(IDCANCEL);
}

//////////////////////////////////////////////////////////////////////////


void Split( const String& str, TCHAR szSep, vector<String>& vRet )
{
	int nIndex = 0;

	while( true )
	{
		int nResult = (int)str.find( szSep, nIndex );
		if( -1 == nResult )
		{
			vRet.push_back( str.substr( nIndex, str.length()-nIndex ) );
			break;
		}
		else
		{
			vRet.push_back( str.substr( nIndex, nResult-nIndex ) );
			nIndex = ++nResult;
		}
	}
}