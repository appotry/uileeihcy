#pragma once
#include "resource.h"
#include "Dialogs.h"
class CStyleViewDlg;
class CStyleCodeDlg;


class CStyleViewDlg : public CDialogImpl<CStyleViewDlg>
{
public:
	enum { IDD = IDD_DIALOG_STYLE };

	CStyleViewDlg(void);
	~CStyleViewDlg(void);


	BEGIN_MSG_MAP_EX(CStyleViewDlg)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE( OnSize )
		COMMAND_HANDLER_EX( IDC_BUTTON_ADD_SELECTOR,BN_CLICKED,    OnBtnAddSelector )
		COMMAND_HANDLER_EX( IDC_BUTTON_MODIFY_SELECTOR,BN_CLICKED, OnBtnModifySelector )
		COMMAND_HANDLER_EX( IDC_BUTTON_DELETE_SELECTOR,BN_CLICKED, OnBtnDeleteSelector )
		COMMAND_HANDLER_EX( IDC_BUTTON_ADD_PROP,BN_CLICKED,    OnBtnAddProp )
		COMMAND_HANDLER_EX( IDC_BUTTON_MODIFY_PROP,BN_CLICKED, OnBtnModifyProp )
		COMMAND_HANDLER_EX( IDC_BUTTON_REMOVE_PROP,BN_CLICKED, OnBtnDeleteProp )
		NOTIFY_HANDLER_EX( IDC_STYLELIST, LVN_ITEMCHANGED, OnStyleItemChanged )
		NOTIFY_HANDLER_EX( IDC_STYLELIST, NM_DBLCLK, OnItemDBClick )
		NOTIFY_HANDLER_EX( IDC_LIST_STYLE_PROP, NM_DBLCLK, OnPropItemDBClick )
	END_MSG_MAP()

public:
	void  AttachSkin( HSKIN hSkin );
	HSKIN GetSkin() { return m_hSkin; }
	void  OnSize(UINT nType, CSize size);
	BOOL  OnInitDialog( HWND, LPARAM );
	void  OnDestroy();
	void  OnBtnAddSelector(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnModifySelector(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnDeleteSelector(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnAddProp(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnModifyProp(UINT uNotifyCode, int nID, CWindow wndCtl);
	void  OnBtnDeleteProp(UINT uNotifyCode, int nID, CWindow wndCtl);

	LRESULT OnStyleItemChanged(LPNMHDR pnmh);
	LRESULT OnItemDBClick(LPNMHDR pnmh);
	LRESULT OnPropItemDBClick(LPNMHDR pnmh);

	bool    Reload();
private:
	HSKIN   m_hSkin;

	CListViewCtrl m_listctrl;       // 所有样式列表
	CListViewCtrl m_listctrl_prop;  // 属性列表
};

//
//	样式style的代码编辑窗口
//
class CStyleCodeDlg : public CDialogImpl<CStyleCodeDlg>, public CResourceCodeDlg<CStyleCodeDlg>
{
public:
	enum { IDD = IDD_DIALOG_STYLE_CODE };

	CStyleCodeDlg();
	~CStyleCodeDlg();

	BEGIN_MSG_MAP_EX(CStyleCodeDlg)
		CHAIN_MSG_MAP(CResourceCodeDlg)
		END_MSG_MAP()

public:
};


class CStyleBuildDlg : public CDialogImpl<CStyleBuildDlg>
{
public:
	enum {IDD=IDD_DIALOG_STYLE_BUILD};
	CStyleBuildDlg();
	~CStyleBuildDlg();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// 由于在childframe中不能很好的处理各种builddlg的释放，因此将释放操作放在这里进行
		delete this;
	}

	BEGIN_MSG_MAP_EX( CStyleBuildDlg )
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
	CStyleViewDlg*  m_pStyleViewPanel;
	CStyleCodeDlg*  m_pStyleCodePanel;
	HSKIN           m_hSkin;
};