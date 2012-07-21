#pragma once
#include "resource.h"
#include "Dialogs.h"

//
//	ͼƬ��������
//
class CFontViewDlg : public CDialogImpl<CFontViewDlg>
{
public:
	enum { IDD = IDD_DIALOG_FONT_VIEW };

	CFontViewDlg();
	~CFontViewDlg();

	BEGIN_MSG_MAP_EX(CFontViewDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SIZE( OnSize )
		MSG_WM_DESTROY( OnDestroy )
		COMMAND_HANDLER_EX( IDC_BUTTON_ADD,    BN_CLICKED, OnBtnAdd )
		COMMAND_HANDLER_EX( IDC_BUTTON_DELETE, BN_CLICKED, OnBtnRemove )
		NOTIFY_HANDLER_EX( IDC_FONTLIST, LVN_ITEMCHANGED, OnItemChanged )
		NOTIFY_HANDLER_EX( IDC_FONTLIST, NM_DBLCLK, OnItemDBClick )
		MSG_WM_PAINT( OnPaint )
	END_MSG_MAP()

public:
	void  AttachSkin( HSKIN hSkin );
	HSKIN GetSkin() { return m_hSkin; }
	void  OnSize(UINT nType, CSize size);
	void  OnDestroy();
	void  OnPaint(HDC hDC);
	BOOL  OnInitDialog( HWND, LPARAM );
	LRESULT  OnItemChanged(LPNMHDR pnmh);
	LRESULT  OnItemDBClick(LPNMHDR pnmh);
	void     OnBtnAdd(UINT uNotifyCode, int nID, CWindow wndCtl);
	void     OnBtnRemove(UINT uNotifyCode, int nID, CWindow wndCtl);
	void     InvalidateDisplayArea();

	bool     Reload();
	bool     Clear();

private:
	HSKIN m_hSkin;

	CListViewCtrl m_listctrl;
};



//
//	ͼƬ�Ĵ���༭����
//
class CFontCodeDlg : public CDialogImpl<CFontCodeDlg>, public CResourceCodeDlg<CFontCodeDlg>
{
public:
	enum { IDD = IDD_DIALOG_FONT_CODE };

	CFontCodeDlg();
	~CFontCodeDlg();

	BEGIN_MSG_MAP_EX(CFontCodeDlg)
		CHAIN_MSG_MAP(CResourceCodeDlg)
	END_MSG_MAP()

public:
};


class CFontBuildDlg : public CDialogImpl<CFontBuildDlg>
{
public:
	enum{IDD = IDD_DIALOG_FONT_BUILD };
	CFontBuildDlg();
	~CFontBuildDlg();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// ������childframe�в��ܺܺõĴ������builddlg���ͷţ���˽��ͷŲ��������������
		delete this;
	}

	BEGIN_MSG_MAP_EX( CFontBuildDlg )
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
	CFontViewDlg*   m_pFontViewPanel;
	CFontCodeDlg*   m_pFontCodePanel;
	HSKIN           m_hSkin;
};
