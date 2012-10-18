#pragma once

namespace UI
{
extern UIApplication*  g_pUIApplication;


// 
//	用于在创建窗口的时候，拦截第一个窗口消息，并进行子类化
//
class CREATE_WND_DATA  // 等同于 ATL 中的_ATL_WIN_MODULE70
{
public:
	CREATE_WND_DATA();
	~CREATE_WND_DATA();

public:
	void  AddCreateWndData(_AtlCreateWndData* pData, void* pThis);
	void* ExtractCreateWndData();

private:
	CRITICAL_SECTION   m_cs;
	_AtlCreateWndData* m_pCreateWndList;
};

//
//	UI对象创建信息
//
struct UI_CREATE_DATA 
{
	String                 m_strXmlName;
	s_UICreateInstancePtr  m_fun;
};


// 创建一个隐藏消息窗口，用于实现post ui message
class CForwardPostMessageWindow : public CWindowImpl<CForwardPostMessageWindow>
{
public:
// 	BEGIN_MSG_MAP_EX(CForwardPostMessageWindow)
// 	END_MSG_MAP()
	
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);
};

//
//	全局对象
//
class UIApplication
{
public:
	UIApplication(void);
	~UIApplication(void);

private:
	void  RegisterWndClass();
	bool  InitLog( const String& );
	void  RegisterDefaultUIObject();

public:
	// ---------- 
	bool Initialize( const String& strUIProjXmlPath );
	bool RegisterUICreateData(const String& strXmlName, s_UICreateInstancePtr pfun);
	bool GetUICreateInstanceFuncPtr(const String& strXmlName, s_UICreateInstancePtr* pOut);

	// ----------
	bool IsUnderXpOS();


	// ----------   from uibuilder manager -------------- 
public:
	void       Save( HSKIN hSkin, UI_RESOURCE_TYPE eResType );

	bool       IsDesignMode() { return m_bDesignMode; }
	void       SetDesignMode(bool b) { m_bDesignMode = b; }

	static     void SetModuleInstance( HINSTANCE hInst ) { m_hInstance = hInst; }
	HINSTANCE  GetModuleInstance() { return UIApplication::m_hInstance; }

private:
	// ----------   from uibuilder manager -------------- 

	//  全局对象
public:
	//LogManager     m_MgrLog;
	
public:
	CComPtr<ILog>    m_pLog;
	TopWindowManager m_TopWindowMgr; 
	ProjectManager   m_ProjectMgr;
	ToolTipManager   m_ToolTipMgr;

	CREATE_WND_DATA  m_create_wnd_data;    // 配合UI_AddCreateWndData、UI_ExtractCreateWndData使用
	CDCCache         m_dcCache;            // 配合UI_GetCacheDC、UI_ReleaseCacheDC使用
	CForwardPostMessageWindow   m_WndForwardPostMsg;  // 配合UI_GetForwardPostMessageWnd使用
	
private:
	static HINSTANCE m_hInstance;          // 当前DLL句柄，用于扩展路径
	bool             m_bDesignMode;        // 是否是设计模式

	vector<UI_CREATE_DATA*>   m_vecUICreateData;

	OSVERSIONINFOEX  m_osvi;               // 操作系统版本

};

}