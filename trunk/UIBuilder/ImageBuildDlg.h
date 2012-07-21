#pragma once
#include "resource.h"
#include "Dialogs.h"


//
//	ר�Ÿ�����ʾһ��ͼƬ�Ĵ���view
//	
class CImageDispalyView : public CWindowImpl<CImageDispalyView>
{
public:
	CImageDispalyView();
	~CImageDispalyView();

	BEGIN_MSG_MAP_EX( CImageDispalyView )
		MSG_WM_CREATE( OnCreate )
		MSG_WM_ERASEBKGND( OnEraseBkgnd )
		MSG_WM_PAINT( OnPaint )
	END_MSG_MAP()

		void OnPaint( HDC hDC );
	BOOL OnEraseBkgnd( HDC hDC );
	void DoPaint(CDCHandle);
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	void     Attach( UIImage* pImage );

	enum IMAGE_DISPLAY_TYPE
	{
		IMAGE_DISPLAY_TYPE_CENTER_BITBLT,
		IMAGE_DISPLAY_TYPE_CENTER_STRETCH,
		IMAGE_DISPLAY_TYPE_LEFTTOP_BITBLT,
	};
private:
	HBRUSH    m_hBkBrush;

	String    m_strImagePath;
	UIImage*  m_pImage;

	IMAGE_DISPLAY_TYPE  m_eDisplayType;
};

//
//	ͼƬ��������
//
class CImageViewDlg : public CDialogImpl<CImageViewDlg>
{
public:
	enum { IDD = IDD_DIALOG_IMAGE_VIEW };

	CImageViewDlg();
	~CImageViewDlg();

	BEGIN_MSG_MAP_EX(CImageViewDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SIZE( OnSize )
		COMMAND_HANDLER_EX( IDC_BUTTON_ADD,    BN_CLICKED, OnBtnAdd )
		COMMAND_HANDLER_EX( IDC_BUTTON_DELETE, BN_CLICKED, OnBtnRemove )
		COMMAND_HANDLER_EX( IDC_BUTTON_LEFT,   BN_CLICKED, OnBtnLeft )
		COMMAND_HANDLER_EX( IDC_BUTTON_RIGHT,  BN_CLICKED, OnBtnRight )
		NOTIFY_HANDLER_EX( IDC_FILELIST, LVN_ITEMCHANGED, OnItemChanged )
		NOTIFY_HANDLER_EX( IDC_FILELIST, NM_DBLCLK, OnItemDBClick )
	END_MSG_MAP()

public:
	void     AttachSkin( HSKIN hSkin );
	HSKIN    GetSkin() { return m_hSkin; }
	void     OnSize(UINT nType, CSize size);
	BOOL     OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void	 OnBtnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	 OnBtnRemove(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	 OnBtnLeft(UINT uNotifyCode, int nID, CWindow wndCtl);
	void	 OnBtnRight(UINT uNotifyCode, int nID, CWindow wndCtl);

	CImageDispalyView*  GetImageDisplayView() { return m_pImageDisplayView; }

	LRESULT  OnItemChanged(LPNMHDR pnmh);
	LRESULT  OnItemDBClick(LPNMHDR pnmh);

	bool     Reload();

private:
	HSKIN                m_hSkin;
	CListViewCtrl        m_listctrl;
	CImageDispalyView*   m_pImageDisplayView;
};

//
//	ͼƬ�Ĵ���༭����
//
class CImageCodeDlg : public CDialogImpl<CImageCodeDlg>, public CResourceCodeDlg<CImageCodeDlg>
{
public:
	enum { IDD = IDD_DIALOG_IMAGE_CODE };

	CImageCodeDlg();
	~CImageCodeDlg();

	BEGIN_MSG_MAP_EX(CImageCodeDlg)
		CHAIN_MSG_MAP(CResourceCodeDlg)
	END_MSG_MAP()

public:
};


class CImageBuildDlg : public CDialogImpl<CImageBuildDlg>
{
public:
	enum{IDD = IDD_DIALOG_IMAGE_BUILD };
	CImageBuildDlg();
	~CImageBuildDlg();
	
	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// ������childframe�в��ܺܺõĴ������builddlg���ͷţ���˽��ͷŲ��������������
		delete this;
	}

	BEGIN_MSG_MAP_EX( CImageBuildDlg )
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SIZE(OnSize)
		NOTIFY_HANDLER_EX(IDC_TAB, TCN_SELCHANGING, OnSelChanging )
	END_MSG_MAP()

public:
	void      AttachSkin( HSKIN hSkin );
	HSKIN     GetSkin() { return m_hSkin; }

protected:
	BOOL      OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void      OnSize(UINT nType, CSize size);
	LRESULT   OnSelChanging(LPNMHDR pnmh);

private:
	CTabCtrl        m_tabCtrl;
	CImageViewDlg*  m_pImageViewPanel;
	CImageCodeDlg*  m_pImageCodePanel;
	HSKIN           m_hSkin;
};
