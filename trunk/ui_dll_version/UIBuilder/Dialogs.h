#pragma once
#include "resource.h"
class CMainFrame;


//
//	��ҳ������/�򿪹���
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
//	�½�����/Ƥ���Ի���
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
	HSKIN       m_hSkin;               // ������������Ƥ�����,�����ⲿ����

private:
	CString     m_strProjectFolder;    // ��������Ŀ¼ 
	bool        m_bCreateProject;      // ��ǰ�Ǵ������̻��ǽ����һ��Ƥ��
};

//
//	����һ��image�Ի���
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

	String    m_strDataSourceFolder;    // image.xml����·�������ڿ���ͼƬ����Ŀ¼��
	String    m_strId;
	String    m_strPath;

private:
	bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
	bool      m_bFirstTime;   // 
};


//
//	����һ��color�Ի���
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
	bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
};


//
//	����һ��font�Ի���
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
	bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
};

//
//	���һ����ʽ����
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

	map<String, bool> m_mapAllStyle;   // true��ʾ�Լ��̳��˵ģ�false��ʾû�б��̳е�

private:
	HSKIN     m_hSkin;
	bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
};

//
//	���һ��Style���Դ���
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
	bool      m_bModifyMode;  //  ��ǰ���������ڴ��������Ǳ������޸�
};

//
//	������Դ���ڵ�һЩ����������������������
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

	// ��tmp�ļ�����ȡ��edit��
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

		Util::UTF82U(strContent.c_str(), buffer);  // ����������
		::SetWindowTextW(m_edit.m_hWnd, buffer );
		return true;
	}

	// ��edit�����ύ��tmp�ļ���
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
		Util::U2UTF8(szContent, utf8);     // xml��utf8�ļ�
		//f << '\xef' << '\xbb' << '\xbf' ;  // utf8 �ļ���־<-- ����Ҫ����ΪszContent���Ѿ������ñ�־
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

		// ��ȡxxx.xml�����ļ�·��
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
	//	Ϊ��ʵ�ֹر�childframe���ٴ�ʱ���ᶪʧ�༭������
	//	�ڴ��ڹر�ʱ���������ύ��tmp�ļ���
	//  ֱ���û������������ʱ���ٱ��浽xml�ļ���
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
		//1. �ύ��tmp�ļ���
		if ( false == this->Commit_2_TempFile() )
			return;

		//2. ��tmp�ļ�copyΪ��ʽ�ļ�
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

