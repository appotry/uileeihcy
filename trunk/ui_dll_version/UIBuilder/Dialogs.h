#pragma once
#include "resource.h"
class CMainFrame;


//
//	首页，创建/打开工程
//
class CFirstPageDlg : public CDialogImpl<CFirstPageDlg>
{
public:
	enum{ IDD = IDD_FIRST_PAGE };

	CFirstPageDlg( CMainFrame* pMainFrame );

	BEGIN_MSG_MAP_EX(CFirstPageDlg)
		COMMAND_HANDLER_EX( IDC_BUTTON_NEW_PROJECT, BN_CLICKED, OnBtnNewProject )
		COMMAND_HANDLER_EX( IDC_BUTTON_OPEN_PROJEC, BN_CLICKED, OnBtnOpenProject )
	END_MSG_MAP()

public:
	void OnBtnNewProject(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtnOpenProject(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	CMainFrame*   m_pMainFrame;
};

//
//	新建工程/皮肤对话框
//
class CNewProjectDlg : public CDialogImpl<CNewProjectDlg>
{
public:
	enum {IDD = IDD_NEW_PROJECT };
	CNewProjectDlg(bool bCreateProject);
	~CNewProjectDlg();

	BEGIN_MSG_MAP_EX(CNewProjectDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_CLOSE( OnClose )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_BROWSE, OnBrowse )
		COMMAND_HANDLER_EX( IDC_EDIT_NEW_SKIN_NAME, EN_CHANGE, OnEnChange_NewSkinName )
	END_MSG_MAP()

public:
	void   OnClose();
	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void   OnBrowse( UINT, int, HWND );
	void   OnEnChange_NewSkinName(UINT, int, HWND );

public:
	HSKIN       m_hSkin;               // 返回所创建的皮肤句柄,便于外部引用

private:
	CString     m_strProjectFolder;    // 工程所在目录 
	bool        m_bCreateProject;      // 当前是创建工程还是仅添加一个皮肤
};

//
//	插入一个image对话框
//
class CNewImageDlg : public CDialogImpl<CNewImageDlg>
{
public:
	enum {IDD = IDD_NEW_IMAGE };
	CNewImageDlg( HSKIN hSkin, bool bModifyMode=false );
	~CNewImageDlg();

	BEGIN_MSG_MAP_EX(CNewImageDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_BROWSE, OnBrowse )
		MSG_WM_PAINT( OnPaint )
	END_MSG_MAP()

	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void   OnBrowse( UINT, int, HWND );
	void   OnPaint(HDC);

	void   SetImageInfo( const String& strID, const String& strPath );

public:
	CEdit     m_editID;
	CEdit     m_editPath;

	HSKIN     m_hSkin;

	String    m_strDataSourceFolder;    // image.xml所在路径，用于拷贝图片到本目录中
	String    m_strId;
	String    m_strPath;

private:
	bool      m_bModifyMode;  //  当前窗口是用于创建，还是被用于修改
	bool      m_bFirstTime;   // 
};


//
//	插入一个color对话框
//
class CNewColorDlg : public CDialogImpl<CNewColorDlg>
{
public:
	enum {IDD = IDD_NEW_COLOR };
	CNewColorDlg( HSKIN hSkin, bool bModifyMode=false );
	~CNewColorDlg();

	BEGIN_MSG_MAP_EX(CNewColorDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_ID_HANDLER_EX( IDC_BTN_CHOOSECOL, OnChooseColor )
	END_MSG_MAP()
	
	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	void   OnChooseColor( UINT,int,HWND	);
	
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void   SetColorInfo( TCHAR* szColorName, TCHAR* szColorValue );

public:
	String    m_strId;
	String    m_strColor;

	HSKIN     m_hSkin;

private:
	bool      m_bModifyMode;  //  当前窗口是用于创建，还是被用于修改
};


//
//	插入一个font对话框
//
class CNewFontDlg : public CDialogImpl<CNewFontDlg>
{
public:
	enum {IDD = IDD_NEW_FONT };
	CNewFontDlg( HSKIN hSkin, bool bModifyMode=false);
	~CNewFontDlg();

	BEGIN_MSG_MAP_EX(CNewFontDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
	END_MSG_MAP()

	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void   SetFontInfo( const String& strID, LOGFONT* plf);
public:
	String    m_strId;
	String    m_strFaceName;
	int       m_nSize;
	int       m_nOrientation;
	bool      m_bBold;
	bool      m_bItalic;
	bool      m_bUnderline;
	bool      m_bStrikeout;

	CButton   m_btnBold;
	CButton   m_btnItalic;
	CButton   m_btnUnderline;
	CButton   m_btnStrikeout;

	LOGFONT   m_lf;
	HSKIN     m_hSkin;
	
private:
	bool      m_bModifyMode;  //  当前窗口是用于创建，还是被用于修改
};

//
//	添加一个样式窗口
//
class CNewStyleDlg : public CDialogImpl<CNewStyleDlg>
{
public:
	enum {IDD = IDD_NEW_STYLE };
	CNewStyleDlg( HSKIN hSkin, bool bModifyMode=false);
	~CNewStyleDlg();

	BEGIN_MSG_MAP_EX(CNewStyleDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_ADD_INHERIT, OnBtnAddInherit )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_DELETE_INHERIT, OnBtnDeleteInherit )
		COMMAND_ID_HANDLER_EX( IDC_BUTTON_CLEAR_INHERIT, OnBtnClearInherit )
		COMMAND_HANDLER_EX( IDC_LISTBOX_STYLEITEM_NOT, LBN_DBLCLK, OnListBoxNotDBClick )
	END_MSG_MAP()

public:
	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void   OnBtnAddInherit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void   OnBtnDeleteInherit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void   OnBtnClearInherit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void   OnListBoxNotDBClick(UINT uNotifyCode, int nID, CWindow wndCtl);

	void   SetStyleInfo( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit );

protected:
	void   FillAllStyleListBox();

public:
	CComboBox m_comboType;
	CEdit     m_editID;
	CEdit     m_editInherit;
	CListBox  m_listbox_own;
	CListBox  m_listbox_not;

	STYLE_SELECTOR_TYPE  m_type;
	String    m_strID;
	String    m_strInherit;

	map<String, bool> m_mapAllStyle;   // true表示自己继承了的，false表示没有被继承的

private:
	HSKIN     m_hSkin;
	bool      m_bModifyMode;  //  当前窗口是用于创建，还是被用于修改
};

//
//	添加一个Style属性窗口
//
class CNewStyleAttributeDlg : public CDialogImpl<CNewStyleAttributeDlg>
{
public:
	enum {IDD = IDD_NEW_STYLE_ATTRIBUTE };
	CNewStyleAttributeDlg( HSKIN hSkin, bool bModifyMode=false);
	~CNewStyleAttributeDlg();

	BEGIN_MSG_MAP_EX(CNewStyleAttributeDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		COMMAND_ID_HANDLER_EX( IDOK, OnOk )
		COMMAND_ID_HANDLER_EX( IDCANCEL, OnCancel )
	END_MSG_MAP()

public:
	void   OnOk( UINT,int,HWND	);
	void   OnCancel( UINT,int,HWND	);
	BOOL   OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void   SetStyleInfo(STYLE_SELECTOR_TYPE type, const String strStyleID);
	void   SetAttributeInfo( const String& strKey, const String& strValue );

public:
	CEdit  m_editKey;
	CEdit  m_editValue;

	String m_strKey;
	String m_strValue;
	STYLE_SELECTOR_TYPE m_eType;
	String m_strStyleID;

private:
	HSKIN     m_hSkin;
	bool      m_bModifyMode;  //  当前窗口是用于创建，还是被用于修改
};

//
//	各种资源窗口的一些公共方法，在这里抽象出来
//
template <class T>
class CResourceCodeDlg
{
public:
	CResourceCodeDlg(UI_RESOURCE_TYPE eResType){ m_eResType = eResType; m_hSkin=NULL; }
	virtual ~CResourceCodeDlg() { m_hSkin = NULL; }

	BEGIN_MSG_MAP_EX(CImageCodeDlg)
		COMMAND_CODE_HANDLER_EX(EN_CHANGE, OnEnChange)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SIZE( OnSize )
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SHOWWINDOW( OnShowWindow )
		COMMAND_HANDLER_EX(IDC_BTN_SAVE,BN_CLICKED,OnBtnSave);
	END_MSG_MAP()

public:

	// 将tmp文件中提取到edit中
	bool     Update_From_TempFile()
	{
		if (m_strDataSource.empty())
			return false;

		String strTempFile = m_strDataSource + TEMP_FILE_EXT;
		if( !::PathFileExists(strTempFile.c_str()) )
			return false;

		ifstream f;
		f.open(strTempFile.c_str(), ios_base::in );
		if( f.fail() )
			return false;

		string strContent;
		string strLine;
		while( f.good() )
		{
			::getline(f, strLine);
			strContent.append(strLine);
			strContent.append("\r\n");
		}
		f.close();

		Util::CBufferT<wchar_t> buffer;

		Util::UTF82U(strContent.c_str(), buffer);  // 防中文乱码
		::SetWindowTextW(m_edit.m_hWnd, buffer );
		return true;
	}

	// 将edit内容提交到tmp文件中
	bool     Commit_2_TempFile()
	{
		if (m_strDataSource.empty())
			return false;

		String strTempFile = m_strDataSource + TEMP_FILE_EXT;
		if( !::PathFileExists(strTempFile.c_str()) )
			return false;

		ofstream f;
		f.open(strTempFile.c_str(), ios_base::out|ios_base::binary );
		if( f.fail() )
			return false;

		string strContent;
		string strLine;
		int nLength = m_edit.GetWindowTextLength();
		wchar_t* szContent = new wchar_t[nLength+1];
		wcscpy( szContent, L"" );
		::GetWindowTextW(m_edit.m_hWnd, szContent,nLength);
		szContent[nLength] = L'';

		Util::CBufferT<char> utf8;
		Util::U2UTF8(szContent, utf8);     // xml中utf8文件
		//f << '\xef' << '\xbb' << '\xbf' ;  // utf8 文件标志<-- 不需要，因为szContent中已经包括该标志
		//f << utf8;
		f.write(utf8, utf8.GetBufSize());

		delete[] szContent;
		f.close();

		return true;
	}

	void     AttachSkin( HSKIN hSkin )
	{
		if( m_hSkin == hSkin )
			return;

		m_hSkin = hSkin;

		// 获取xxx.xml配置文件路径
		TCHAR* pszDataSource = NULL;
		int    nSize = 0;

		nSize = ::UI_GetResourceDataSourcePath( m_hSkin, m_eResType, NULL, 0 );
		pszDataSource = new TCHAR[nSize];

		nSize = ::UI_GetResourceDataSourcePath( m_hSkin, m_eResType, pszDataSource, nSize );
		m_strDataSource = pszDataSource ;
		delete [] pszDataSource;
		pszDataSource = NULL;

		Update_From_TempFile();
	}

	HSKIN    GetSkin() { return m_hSkin; }

protected:
	BOOL     OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		m_edit.Attach( static_cast<T*>(this)->GetDlgItem(IDC_EDIT) );
		m_btnSave.Attach(static_cast<T*>(this)->GetDlgItem(IDC_BTN_SAVE) );
		m_btnSave.EnableWindow(FALSE);
		return TRUE;
	}
	void     OnSize(UINT nType, CSize size)
	{
		m_btnSave.SetWindowPos(NULL, WINDOW_PADDING, WINDOW_PADDING, WIDTH_TOOLBAR_BTN, HEIGHT_TOOLBAR_BTN, SWP_NOZORDER );
		m_edit.SetWindowPos(0,WINDOW_PADDING,2*WINDOW_PADDING+HEIGHT_TOOLBAR_BTN,size.cx-2*WINDOW_PADDING,size.cy-3*WINDOW_PADDING-HEIGHT_TOOLBAR_BTN,SWP_NOZORDER);

		SetMsgHandled(FALSE);
	}
	//
	//	为了实现关闭childframe，再打开时不会丢失编辑的内容
	//	在窗口关闭时，将内容提交到tmp文件中
	//  直到用户真正点击保存时，再保存到xml文件中
	//
	void     OnDestroy()
	{
		this->Commit_2_TempFile();

		SetMsgHandled(FALSE);

	}
	void     OnEnChange(UINT,int,HWND)
	{
		T* pThis = static_cast<T*>(this);
		pThis->SET_DIRTY3(true);
		m_btnSave.EnableWindow(TRUE);

		SetMsgHandled(FALSE);
	}
	void     OnBtnSave(UINT,int,HWND)
	{
		//1. 提交到tmp文件中
		if ( false == this->Commit_2_TempFile() )
			return;

		//2. 将tmp文件copy为正式文件
		::DeleteFile(m_strDataSource.c_str());
		::CopyFile((m_strDataSource+TEMP_FILE_EXT).c_str(), m_strDataSource.c_str(), FALSE );

		T* pThis = static_cast<T*>(this);
		pThis->SET_DIRTY3(false);
		m_btnSave.EnableWindow(FALSE);

		SetMsgHandled(FALSE);
	}

	void     OnShowWindow(BOOL bShow, UINT nStatus)
	{
		T* pThis = static_cast<T*>(this);
		if(pThis->IS_DIRTY3())
		{
			m_btnSave.EnableWindow(TRUE);
		}
		else
		{
			m_btnSave.EnableWindow(FALSE);
		}
		SetMsgHandled(FALSE);
	}

private:
	HSKIN    m_hSkin;
	CEdit    m_edit;
	CButton  m_btnSave;
	String   m_strDataSource;
	UI_RESOURCE_TYPE m_eResType;
};

