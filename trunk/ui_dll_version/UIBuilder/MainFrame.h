#pragma once
#include "resource.h"
#include "ProjectTreePanel.h"


#define CHILD_WINDOW_CLASS_FONT    _T("WNDCLASS_FontBuilder")
#define CHILD_WINDOW_CLASS_COLOR   _T("WNDCLASS_ColorBuilder")
#define CHILD_WINDOW_CLASS_STYLE   _T("WNDCLASS_StyleBuilder")
#define CHILD_WINDOW_CLASS_LAYOUT  _T("WNDCLASS_LayoutBuilder")

class CLogViewDlg;
class CFirstPageDlg;
class CImageBuildDlg;
class CWorkAreaPanel;
class CChildFrame;

//
//	主框架窗口
//
class CMainFrame : public CWindowImpl<CMainFrame>, public CMessageFilter
{
public:
	CMainFrame(void);
	~CMainFrame(void);

	BEGIN_MSG_MAP_EX(CMainFrame)
		MSG_WM_CREATE( OnCreate )
		MSG_WM_CLOSE( OnClose )
		MSG_WM_DESTROY( OnDestroy )
		MSG_WM_SIZE( OnSize )
		MSG_WM_ERASEBKGND( OnEraseBkgnd )
		MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
		MSG_WM_DROPFILES(OnDropFiles)

		MESSAGE_HANDLER_EX(WM_SET_DIRTY, OnSetDirty)
		MESSAGE_HANDLER_EX(WM_GET_DIRTY, OnGetDirty)

		COMMAND_ID_HANDLER_EX(ID_NEW_PROJECT, OnMenuNewProject )
		COMMAND_ID_HANDLER_EX(ID_OPEN_PROJECT, OnMenuOpenProject )
		COMMAND_ID_HANDLER_EX(ID_NEW_SKIN, OnMenuNewSkin )
		COMMAND_ID_HANDLER_EX(IDM_SAVE, OnMenuSave )

		COMMAND_ID_HANDLER_EX(ID_WINDOW_CASCADE,    OnMenuMDIWindow )
		COMMAND_ID_HANDLER_EX(ID_WINDOW_TILE_HORZ,  OnMenuMDIWindow )
		COMMAND_ID_HANDLER_EX(ID_WINDOW_ARRANGE,    OnMenuMDIWindow )
		COMMAND_ID_HANDLER_EX(ID_MENU_MDI_CLOSE,    OnMenuMDIWindow )
		COMMAND_ID_HANDLER_EX(ID_MENU_MDI_MINIMIZE, OnMenuMDIWindow )
		COMMAND_ID_HANDLER_EX(ID_MENU_MDI_RESTORE,  OnMenuMDIWindow )

		COMMAND_ID_HANDLER_EX(IDM_EXIT, OnMenuExit )
	END_MSG_MAP()

public:
	LRESULT   OnCreate( LPCREATESTRUCT lpcs );
	void      OnClose();
	void      OnDestroy();
	void      OnSize(UINT nType, CSize size);
	LRESULT   OnEraseBkgnd( HDC hDC );
	void      CalcLayout( int cx, int cy );
	void      OnMenuNewProject( UINT,int,HWND );
	void      OnMenuOpenProject( UINT,int,HWND );
	void      OnMenuNewSkin( UINT,int,HWND );
	void      OnMenuMDIWindow( UINT,int,HWND );
	void      OnMenuExit(UINT,int,HWND);
	void      OnMenuSave(UINT,int,HWND);
	void      OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu);
	LRESULT   OnSetDirty(UINT,WPARAM,LPARAM);
	LRESULT   OnGetDirty(UINT,WPARAM,LPARAM);
	void      OnDropFiles(HDROP hDropInfo);

	void      SwitchToSkinResPanel( UI_RESOURCE_TYPE nType, HSKIN hSkin );
	void      SwitchToLogPanel( int nType, const String& strModuleName );

	void      RemoveChildFrame( CChildFrame* pChildFrame );
	void      AddChildFrame( CChildFrame* pChildFrame );

	virtual   BOOL PreTranslateMessage(MSG* pMsg);
protected:
	void      _InitMDIClient();
	void      _OpenProject(String strFile);

private:
	CProjectTreePanel*   m_pPanelProjectTree;
	CFirstPageDlg*       m_pDlgFirstPage;
	HWND                 m_hWndMDIClient;

	CLogViewDlg*         m_pLogViewDlg;
	bool                 m_bProjectOpened;      // 标志当前工程是否已经创建/打开
	
	list<CChildFrame*>   m_listChildFrame;
};


class CChildFrame : public CMDIChildWindowImpl<CChildFrame>
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_IMAGEBUILD)
	CChildFrame(CMainFrame* pMainFrame);

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	BEGIN_MSG_MAP_EX(CChildFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		MESSAGE_HANDLER_EX(WM_SET_DIRTY,OnSetDirty)
		MESSAGE_HANDLER_EX(WM_GET_DIRTY,OnGetDirty)
		CHAIN_MSG_MAP(CMDIChildWindowImpl<CChildFrame>)
	END_MSG_MAP()

public:

	int     OnCreate(LPCREATESTRUCT lpCreateStruct);
	void    OnDestroy();
	LRESULT OnSetDirty(UINT, WPARAM w, LPARAM);
	LRESULT OnGetDirty(UINT, WPARAM w, LPARAM);
	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);

	UI_RESOURCE_TYPE GetResourceType() { return m_eResType; }
	HSKIN GetHSkin() { return m_hSkin; }
	void  SetView(HWND hView, HSKIN hSkin, UI_RESOURCE_TYPE eResType );

	void  SetDirty(bool b);
	bool  IsDirty(){ return m_bDirty; }

private:
	void  UpdateTitle();

private:
	HSKIN             m_hSkin;
	UI_RESOURCE_TYPE  m_eResType;

	bool              m_bDirty;   // 是否需要保存

	CMainFrame*       m_pMainFrame;

};
