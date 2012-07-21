#pragma once
#include "resource.h"
#include "Dialogs.h"
class CLayoutCodeDlg;


//
//	��ʽlayout�Ĵ���༭����
//
class CLayoutCodeDlg : public CDialogImpl<CLayoutCodeDlg>, public CResourceCodeDlg<CLayoutCodeDlg>
{
public:
	enum { IDD = IDD_DIALOG_LAYOUT_CODE };

	CLayoutCodeDlg();
	~CLayoutCodeDlg();

	BEGIN_MSG_MAP_EX(CLayoutCodeDlg)
		CHAIN_MSG_MAP(CResourceCodeDlg)
	END_MSG_MAP()

public:
};


class CLayoutBuildDlg : public CDialogImpl<CLayoutBuildDlg>
{
public:
	enum {IDD=IDD_DIALOG_LAYOUT_BUILD};
	CLayoutBuildDlg();
	~CLayoutBuildDlg();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// ������childframe�в��ܺܺõĴ������builddlg���ͷţ���˽��ͷŲ��������������
		delete this;
	}

	BEGIN_MSG_MAP_EX( CLayoutBuildDlg )
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
	CLayoutCodeDlg*  m_pStyleCodePanel;
	HSKIN           m_hSkin;
};