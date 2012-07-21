#pragma once
#include "resource.h"

#if 0
typedef CWinTraits< WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | MDIS_ALLCHILDSTYLES, WS_EX_CLIENTEDGE>		CMDIFrameWinTraits;
class CMDIFrameWindow : public CWindowImpl<CMDIFrameWindow,CWindow, CMDIFrameWinTraits >
{
public:
	CMDIFrameWindow(){};
	~CMDIFrameWindow(){};

	BEGIN_MSG_MAP_EX(CMDIFrameWindow)
	END_MSG_MAP()

public:

};

class CMDIChildWindow : public CWindowImpl<CMDIChildWindow,CWindow,CMDIChildWinTraits>
{
public:
	CMDIChildWindow(){};
	~CMDIChildWindow(){};

	BEGIN_MSG_MAP_EX(CMDIChildWindow)

	END_MSG_MAP()

public:

};

// class CMDIChildDialog : public CDialogImpl<CMDIChildDialog,CDialog,CMDIChildWinTraits>
// {
// public:
// 
// };

class CWl_multidocView : public CWindowImpl<CWl_multidocView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CWl_multidocView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		//TODO: Add your drawing code here

		return 0;
	}
};
#endif

class CWorkAreaPanel //: public CMDIFrameWindow
{
public:
	enum{ IDD=IDD_DIALOG_WORKAREA };

	CWorkAreaPanel() {}
	~CWorkAreaPanel() {}

	BEGIN_MSG_MAP_EX(CWorkAreaPanel)
		MSG_WM_CREATE(OnCreate)
		//CHAIN_MSG_MAP(CMDIFrameWindow)
	END_MSG_MAP()

protected:
	BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	bool OpenWindow( UI_RESOURCE_TYPE,  void* data );
	bool IsWindowOpened( UI_RESOURCE_TYPE,  void* data );

private:
	
};
