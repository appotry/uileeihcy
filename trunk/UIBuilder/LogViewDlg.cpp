#include "stdafx.h"
#include "LogViewDlg.h"
#include "projecttreepanel.h"

CLogViewDlg::CLogViewDlg(HWND hWnd)
{
	m_bModifyMode = false;
	m_hWndTreePanel = hWnd;
}

CLogViewDlg::~CLogViewDlg()
{

}

//
//	清除界面数据
//
void CLogViewDlg::clear()
{
	m_editModuleName.SetWindowText(_T(""));
	m_btnLogItemSwitch.SetCheck(BST_CHECKED);
	m_comboLogLevel.SetCurSel(0);
	m_btnLF_Write2Console.SetCheck(BST_UNCHECKED);
	m_btnLF_OutputDebugString.SetCheck(BST_UNCHECKED);;
	m_btnLF_PrintThreadId.SetCheck(BST_UNCHECKED);;
	m_btnFF_NoFileOutput.SetCheck(BST_UNCHECKED);;
	m_btnFF_OverWrite.SetCheck(BST_UNCHECKED);;
	m_btnFF_AddTimeExt.SetCheck(BST_UNCHECKED);;
	m_btnFF_Loop.SetCheck(BST_UNCHECKED);;
	m_editLoopCount.SetWindowText(_T("") );
	m_staticGroupBoxLogItem.SetWindowText(_T("Log_Item_Operator:"));
	::EnableWindow(GetDlgItem(IDC_EDIT_LOOPCOUNT), FALSE );

	TCHAR szProjDir[MAX_PATH] = _T("");
	::UI_GetProjectDirectory(szProjDir);
	_tcscat(szProjDir, _T("\\log.xml"));
	m_editLogPath.SetWindowText(szProjDir);
	m_btnLogPathRelative.SetCheck(BST_CHECKED);
}

BOOL CLogViewDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_btnLogGlobalSwitch.Attach(GetDlgItem(IDC_CHECK_LOGSWITCH));

	m_editModuleName.Attach(GetDlgItem(IDC_EDIT_MODULENAME));
	m_btnLogItemSwitch.Attach(GetDlgItem(IDC_CHECK_LOGITEM_SWITCH));
	m_comboLogLevel.Attach(GetDlgItem(IDC_COMBO_LOGLEVEL));
	m_editLogPath.Attach(GetDlgItem(IDC_EDIT_LOGPATH));
	m_btnLogPathRelative.Attach(GetDlgItem(IDC_CHECK_LOGPATH_RELATIVE ));
	m_btnLF_Write2Console.Attach(GetDlgItem(IDC_CHECK_LOGFLAG_CONSOLE));
	m_btnLF_OutputDebugString.Attach(GetDlgItem(IDC_CHECK_LOGFLAG_DEBUG));
	m_btnLF_PrintThreadId.Attach(GetDlgItem(IDC_CHECK_LOGFLAG_THREADID));
	m_btnFF_NoFileOutput.Attach(GetDlgItem(IDC_RADIO_FILEFALG_DONOTWRITEFILE));
	m_btnFF_OverWrite.Attach(GetDlgItem(IDC_RADIO_FILEFLAG_OVERWRITE));
	m_btnFF_AddTimeExt.Attach(GetDlgItem(IDC_RADIO_FILEFLAG_ADDTIMEEXT));
	m_btnFF_Loop.Attach(GetDlgItem(IDC_RADIO_FILEFLAG_LOOP));
	m_editLoopCount.Attach(GetDlgItem(IDC_EDIT_LOOPCOUNT));
	m_staticGroupBoxLogItem.Attach(GetDlgItem(IDC_STATIC_GROPBOX));
	
	m_comboLogLevel.AddString(_T("DEBUG"));
	m_comboLogLevel.AddString(_T("INFO"));
	m_comboLogLevel.AddString(_T("WARN"));
	m_comboLogLevel.AddString(_T("ERROR"));
	m_comboLogLevel.AddString(_T("FATAL"));

	if( NULL != g_pLog )
	{
		if(g_pLog->IsEnable())
		{
			m_btnLogGlobalSwitch.SetCheck( BST_CHECKED );
		}
	}
	

	if( ! m_bModifyMode )
	{
		this->clear();
	}

	this->SetMode(m_bModifyMode);

	return TRUE;
}

//
//	关联确认显示哪个模块数据
//
void CLogViewDlg::AttachLogItem( const String& strLogItemName )
{
	this->clear();
	if( strLogItemName.empty() )
	{
		SetMode(false);
		return;
	}

	if( NULL != g_pLog )
	{
		m_strModuleName = strLogItemName.c_str();
		_bstr_t bstr(strLogItemName.c_str());
		ILogItem* pLogItem = g_pLog->GetLogItemByName( bstr );
		if(NULL != pLogItem)
		{
			BSTR bstr = pLogItem->GetName();
			m_editModuleName.SetWindowText((TCHAR*)bstr);
			::SysFreeString(bstr);
		
			long lVal = pLogItem->GetSwitch();
			if( lVal )
				m_btnLogItemSwitch.SetCheck(BST_CHECKED);
			else
				m_btnLogItemSwitch.SetCheck(BST_UNCHECKED);


			lVal = pLogItem->GetLevel();
				m_comboLogLevel.SetCurSel(lVal);
		
			bstr = pLogItem->GetPath();
			m_editLogPath.SetWindowText((TCHAR*)bstr);
			if( Util::IsFullPath((TCHAR*)bstr) )
				m_btnLogPathRelative.SetCheck(BST_UNCHECKED);				
			else
				m_btnLogPathRelative.SetCheck(BST_CHECKED);
			SysFreeString(bstr);

			lVal = pLogItem->GetLogFlag();
			if( lVal & LOG_FLAG_CONSOLE )
				m_btnLF_Write2Console.SetCheck(BST_CHECKED);
			if( lVal & LOG_FALG_DBGVIEW )
				m_btnLF_OutputDebugString.SetCheck(BST_CHECKED);
			if( lVal & LOG_FLAG_THREADID )
				m_btnLF_PrintThreadId.SetCheck(BST_CHECKED);

			lVal = pLogItem->GetFileFlag();
			if( lVal & LOG_FLAG_FILE_DISABLE )
				m_btnFF_NoFileOutput.SetCheck(BST_CHECKED);
			if( lVal & LOG_FLAG_FILE_SIMPLE )
				m_btnFF_OverWrite.SetCheck(BST_CHECKED);
			if( lVal & LOG_FLAG_FILE_TIME )
				m_btnFF_AddTimeExt.SetCheck(BST_CHECKED);

			
			if( lVal & LOG_FLAG_FILE_LOOP )
			{
				m_btnFF_Loop.SetCheck(BST_CHECKED);
				m_editLoopCount.EnableWindow(TRUE);
				int nCount = lVal>>24;
				::SetDlgItemInt(m_hWnd, IDC_EDIT_LOOPCOUNT, nCount, TRUE);
			}
			else
				m_editLoopCount.EnableWindow(FALSE);
	
			pLogItem->Release();
		}
	}
	
	this->SetMode(true);
}

void CLogViewDlg::SetMode(bool bModify)
{
	if(bModify)
	{
		TCHAR szInfo[256] = _T("");
		_stprintf(szInfo, _T("Log_Item_Modify: %s"), m_strModuleName.c_str() );
		::SetWindowText(::GetDlgItem(m_hWnd,IDC_STATIC_GROPBOX), szInfo );
		::SetWindowText(::GetDlgItem(m_hWnd,IDOK),_T("Modify") );

	}
	else
	{
		::SetWindowText(::GetDlgItem(m_hWnd,IDC_STATIC_GROPBOX),_T("Log_Item_Create") );
		::SetWindowText(::GetDlgItem(m_hWnd,IDOK),_T("Create") );
	}

	m_bModifyMode = bModify;
}

void CLogViewDlg::OnOk(UINT,int,HWND)
{
	if ( NULL == g_pLog )
	{
		return;
	}


	TCHAR  szTemp[MAX_STRING_LEN] = _T("");
	UINT   nTemp = 0;

	m_editModuleName.GetWindowText(szTemp, MAX_STRING_LEN);
	String strModuleName = szTemp;
	if (strModuleName.empty())
	{
		::MessageBox( NULL, _T("Module name is empty"), _T("Warning"), MB_OK|MB_ICONWARNING );
		return;
	}

	nTemp = m_btnLogItemSwitch.GetCheck();
	LONG lSwitch = nTemp==BST_CHECKED? 1:0;

	nTemp = m_comboLogLevel.GetCurSel();
	LONG lLevel = nTemp;

	m_editLogPath.GetWindowText(szTemp, MAX_PATH);
	String strPath = szTemp;
	if( strPath.empty())
	{
		::MessageBox( NULL, _T("Module path is empty"), _T("Warning"), MB_OK|MB_ICONWARNING );
		return;
	}

	long lLogFlag = 0;
	nTemp = m_btnLF_Write2Console.GetCheck();
	if( nTemp == BST_CHECKED )
		lLogFlag |= LOG_FLAG_CONSOLE;
	nTemp = m_btnLF_OutputDebugString.GetCheck();
	if( nTemp == BST_CHECKED )
		lLogFlag |= LOG_FALG_DBGVIEW;
	nTemp = m_btnLF_PrintThreadId.GetCheck();
	if( nTemp == BST_CHECKED )
		lLogFlag |= LOG_FLAG_THREADID;

	long lFileFlag = 0;
	nTemp = m_btnFF_NoFileOutput.GetCheck();
	if( nTemp == BST_CHECKED )
		lFileFlag = LOG_FLAG_FILE_DISABLE;
	nTemp = m_btnFF_OverWrite.GetCheck();
	if( nTemp == BST_CHECKED )
		lFileFlag = LOG_FLAG_FILE_SIMPLE;
	nTemp = m_btnFF_AddTimeExt.GetCheck();
	if( nTemp == BST_CHECKED )
		lFileFlag = LOG_FLAG_FILE_TIME;
	nTemp = m_btnFF_Loop.GetCheck();
	if( nTemp == BST_CHECKED )
	{
		lFileFlag = LOG_FLAG_FILE_LOOP;
		nTemp = ::GetDlgItemInt( m_hWnd, IDC_EDIT_LOOPCOUNT, NULL, TRUE );
		nTemp = nTemp << 24;
		lFileFlag += nTemp;
	}
	
	if (m_bModifyMode)
	{
		_bstr_t bstrOldModuleName = m_strModuleName.c_str();
		_bstr_t bstrModuleName = strModuleName.c_str();
		_bstr_t bstrPath = strPath.c_str();

		if( FALSE == g_pLog->ModifyLogItem( bstrOldModuleName, bstrModuleName , lSwitch, lLevel, 
				bstrPath, lLogFlag, lFileFlag ))
		{
			::MessageBox( NULL, _T("Modify LogItem Failed"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}

		// 更新到左侧结点
		::SendMessage(m_hWndTreePanel, WM_MODIFY_LOG_ITEM, (WPARAM)strModuleName.c_str(), (LPARAM)m_strModuleName.c_str() );

		m_strModuleName = strModuleName;
		this->SetMode(true);

		::MessageBox( NULL, _T("Modify Module Success"), _T("Tip"), MB_OK|MB_ICONINFORMATION );
	}
	else
	{
		_bstr_t bstrModuleName = strModuleName.c_str();
		_bstr_t bstrPath = strPath.c_str();

		if( FALSE == g_pLog->AddLogItem( bstrModuleName , lSwitch, lLevel, 
				bstrPath, lLogFlag, lFileFlag ) )
		{
			::MessageBox( NULL, _T("Add LogItem Failed"), _T("Error"), MB_OK|MB_ICONERROR );
			return;
		}

		// 添加成功
		// 1. 插入到左侧结点
		::SendMessage(m_hWndTreePanel, WM_INSERT_LOG_ITEM, (WPARAM)strModuleName.c_str(), 0 );
		m_strModuleName = strModuleName;

		// 2. 更新数据界面
		this->SetMode(true);

		::MessageBox( NULL, _T("Add Module Success"), _T("Tip"), MB_OK|MB_ICONINFORMATION );
	}
}
void CLogViewDlg::OnBtnPath(UINT,int,HWND)
{
	CFileDialog  dlg(FALSE);

	TCHAR szDir[MAX_PATH] = _T("");
	m_editLogPath.GetWindowText(szDir, MAX_PATH);

	dlg.m_ofn.lpstrInitialDir = szDir;
	if( IDOK == dlg.DoModal() )
	{
		m_editLogPath.SetWindowText(dlg.m_szFileName);

		this->OnChangeLogPathRelative(0,0,0);
	}
}

void CLogViewDlg::OnBtnLogSwitch(UINT,int,HWND)
{
	if( NULL == g_pLog )
		return ;

	UINT nRet = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGSWITCH );
	if( BST_CHECKED == nRet )
	{
		g_pLog->EnableLog( TRUE );	
	}
	else
	{
		g_pLog->EnableLog( FALSE );
	}
}

void CLogViewDlg::OnRadioBtnFileLog(UINT ,int nID, HWND)
{
	UINT nCheck = ::IsDlgButtonChecked(m_hWnd, IDC_RADIO_FILEFLAG_LOOP );
	if( BST_CHECKED == nCheck )
	{
		::EnableWindow(GetDlgItem(IDC_EDIT_LOOPCOUNT), TRUE );
	}
	else
	{
		::EnableWindow(GetDlgItem(IDC_EDIT_LOOPCOUNT), FALSE );
	}
}

void CLogViewDlg::OnChangeLogPathRelative(UINT,int,HWND)
{
	TCHAR szPath[MAX_PATH] = _T("");
	m_editLogPath.GetWindowText(szPath, MAX_PATH);
	if( 0 == _tcslen(szPath) )
		return;


	UINT uCheck = ::IsDlgButtonChecked(m_hWnd, IDC_CHECK_LOGPATH_RELATIVE );
	if( uCheck == BST_CHECKED )
	{
		// 切换为相对路径
		if( Util::IsFullPath(szPath) )
		{
			TCHAR szProjDir[MAX_PATH] = _T("");
			::UI_GetProjectDirectory(szProjDir);

			TCHAR szResult[MAX_PATH] = _T("");
			Util::CalcRelativePathToDir(szProjDir, szPath, szResult );

			this->m_editLogPath.SetWindowText(szResult);
		}
	}
	else
	{
		// 使用绝对路径
		if( !Util::IsFullPath(szPath) )
		{
			TCHAR szProjDir[MAX_PATH] = _T("");
			::UI_GetProjectDirectory(szProjDir);

			TCHAR szResult[MAX_PATH] = _T("");
			Util::CalcFullPathByRelative(szProjDir, szPath, szResult);

			this->m_editLogPath.SetWindowText(szResult);
		}
	}

}