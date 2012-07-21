#pragma once
#include "resource.h"

class CLogViewDlg : public CDialogImpl<CLogViewDlg>
{
public:
	CLogViewDlg(HWND);
	~CLogViewDlg();

	enum{ IDD=IDD_DIALOG_LOG };
	BEGIN_MSG_MAP_EX( CImageDispalyView )
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_HANDLER_EX(IDOK,BN_CLICKED,OnOk)
		COMMAND_HANDLER_EX(IDC_BTN_BROWNSE,BN_CLICKED,OnBtnPath)
		COMMAND_HANDLER_EX(IDC_BTN_LOG_SWITCH,BN_CLICKED,OnBtnLogSwitch )
		COMMAND_HANDLER_EX(IDC_CHECK_LOGPATH_RELATIVE,BN_CLICKED, OnChangeLogPathRelative )
		COMMAND_RANGE_CODE_HANDLER_EX(IDC_RADIO_FILEFALG_DONOTWRITEFILE,
		        IDC_RADIO_FILEFLAG_LOOP, BN_CLICKED, OnRadioBtnFileLog )
	END_MSG_MAP()

protected:
	BOOL     OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void     OnOk(UINT,int,HWND);
	void     OnBtnPath(UINT,int,HWND);
	void     OnBtnLogSwitch(UINT,int,HWND);
	void     OnChangeLogPathRelative(UINT,int,HWND);
	void     OnRadioBtnFileLog(UINT,int,HWND);
	void     clear();

public:
	void     AttachLogItem( const String& strLogItem );
	void     SetMode(bool bModify);

private:
	CButton     m_btnLogGlobalSwitch;

	CEdit       m_editModuleName;
	CButton     m_btnLogItemSwitch;
	CComboBox   m_comboLogLevel;
	CEdit       m_editLogPath;
	CButton     m_btnLogPathRelative;
	CButton     m_btnLF_Write2Console;
	CButton     m_btnLF_OutputDebugString;
	CButton     m_btnLF_PrintThreadId;
	CButton     m_btnFF_NoFileOutput;
	CButton     m_btnFF_OverWrite;
	CButton     m_btnFF_AddTimeExt;
	CButton     m_btnFF_Loop;
	CEdit       m_editLoopCount;
	CStatic     m_staticGroupBoxLogItem;
	
	bool        m_bModifyMode;   //  当前窗口是用于创建，还是被用于修改
	HWND        m_hWndTreePanel; //  日志树控件窗口句柄，用于发送消息
	String      m_strModuleName; //  保存模块名称，用于在修改模块名称时传递原模块名
};