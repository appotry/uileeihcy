#pragma once
#include "resource.h"
class CMainFrame;

// 插入一项日志模块
#define WM_INSERT_LOG_ITEM   (WM_USER+101)
// 修改一项日志模块
#define WM_MODIFY_LOG_ITEM   (WM_USER+102)


//
//	工程树面板 
//
class CProjectTreePanel : public CDialogImpl<CProjectTreePanel>
{
public:
	enum{IDD=IDD_PROJECT_TREE};

	CProjectTreePanel( CMainFrame* pFrame );
	~CProjectTreePanel(void);

	BEGIN_MSG_MAP_EX(CProjectTreePanel)
		MSG_WM_INITDIALOG( OnInitDialog )
		MSG_WM_SIZE( OnSize )
		MSG_WM_DESTROY( OnDestroy )
		MESSAGE_HANDLER_EX(WM_SET_DIRTY, OnSetDirty)
		MESSAGE_HANDLER_EX(WM_GET_DIRTY, OnGetDirty)
		MESSAGE_HANDLER_EX(WM_GET_APP_DIRTY, OnGetAppDirty)
		MESSAGE_HANDLER_EX(WM_CLEAR_DIRTY, OnClearDirty)
		
		NOTIFY_HANDLER_EX( IDC_TREE_RES, NM_DBLCLK, OnResTreeDBClick )
		NOTIFY_HANDLER_EX( IDC_TREE_LOG, NM_DBLCLK, OnLogTreeDBClick )
		NOTIFY_HANDLER_EX( IDC_TREE_LOG, NM_RCLICK, OnLogTreeRClick )
		NOTIFY_HANDLER_EX( IDC_TAB, TCN_SELCHANGE, OnTabSelChange )
		MESSAGE_HANDLER_EX(WM_INSERT_LOG_ITEM, OnInsertLogItem )
		MESSAGE_HANDLER_EX(WM_MODIFY_LOG_ITEM, OnModifyLogItem )
	END_MSG_MAP()

public:
	BOOL      OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void      OnSize(UINT nType, CSize size);
	void      OnDestroy( );
	void      CalcLayout( int cx, int cy );

	void      InsertSkinItem(HSKIN hSkin);

	LRESULT   OnInsertSkinItem( UINT, WPARAM, LPARAM );
	LRESULT   OnInsertLogItem( UINT, WPARAM, LPARAM );
	LRESULT   OnModifyLogItem( UINT, WPARAM, LPARAM );

	LRESULT   OnResTreeDBClick(LPNMHDR pnmh);
	LRESULT   OnLogTreeDBClick(LPNMHDR pnmh);
	LRESULT   OnLogTreeRClick(LPNMHDR pnmh);
	LRESULT   OnTabSelChange(LPNMHDR pnmh);

	LRESULT   OnGetDirty(UINT,WPARAM,LPARAM);
	LRESULT   OnSetDirty(UINT,WPARAM,LPARAM);
	LRESULT   OnGetAppDirty(UINT,WPARAM,LPARAM);
	LRESULT   OnClearDirty(UINT,WPARAM,LPARAM);

	HTREEITEM FindSkinItem(HSKIN hSkin);
	HTREEITEM FindResItem(HSKIN,UI_RESOURCE_TYPE);

	String    GetResName( UI_RESOURCE_TYPE );

public:

private:
	CMainFrame*     m_pMainFrame ;         // 主窗口

	CTreeViewCtrl   m_treeCtrlRes;
	CTreeViewCtrl   m_treeCtrlLog;
	CTabCtrl        m_tabCtrl;
};
