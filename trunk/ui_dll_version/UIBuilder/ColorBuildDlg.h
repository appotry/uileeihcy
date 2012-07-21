#pragma once
#include "resource.h"
#include "Dialogs.h"

//
//	color操作窗口
//
class CColorViewDlg : public CDialogImpl<CColorViewDlg>
{
public:
	enum { IDD = IDD_DIALOG_COLOR_VIEW };

	CColorViewDlg();
	~CColorViewDlg();

	BEGIN_MSG_MAP_EX(CColorViewDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE( OnSize )
		COMMAND_HANDLER_EX( IDC_BUTTON_ADD,    BN_CLICKED, OnBtnAdd )
		COMMAND_HANDLER_EX( IDC_BUTTON_DELETE, BN_CLICKED, OnBtnRemove )
		NOTIFY_HANDLER_EX( IDC_COLORLIST, NM_CUSTOMDRAW, OnCustomDraw )
		NOTIFY_HANDLER_EX( IDC_COLORLIST, NM_DBLCLK, OnItemDBClick )
	END_MSG_MAP()

public:
	void  AttachSkin( HSKIN hSkin );
	HSKIN GetSkin() { return m_hSkin; }
	void  OnSize(UINT nType, CSize size);
	BOOL  OnInitDialog( HWND, LPARAM );
	void  OnDestroy();
	void  OnBtnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnRemove(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnCustomDraw(LPNMHDR pnmh);
	LRESULT OnItemDBClick(LPNMHDR pnmh);

	void  Clear();
	void  Reload();

private:
	HSKIN m_hSkin;

	CListViewCtrl m_listctrl;
};


class CColorCodeDlg : public CDialogImpl<CColorCodeDlg>, public CResourceCodeDlg<CColorCodeDlg>
{
public:
	enum { IDD = IDD_DIALOG_COLOR_CODE };

	CColorCodeDlg();
	~CColorCodeDlg();

	BEGIN_MSG_MAP_EX(CColorCodeDlg)
		CHAIN_MSG_MAP(CResourceCodeDlg)
	END_MSG_MAP()

public:
};


class CColorBuildDlg : public CDialogImpl<CColorBuildDlg>
{
public:
	enum{IDD = IDD_DIALOG_COLOR_BUILD };
	CColorBuildDlg();
	~CColorBuildDlg();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// 由于在childframe中不能很好的处理各种builddlg的释放，因此将释放操作放在这里进行
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
	CColorViewDlg*  m_pColorViewPanel;
	CColorCodeDlg*  m_pColorCodePanel;
	HSKIN           m_hSkin;
};
