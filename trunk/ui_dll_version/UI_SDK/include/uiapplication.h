#pragma once

namespace UI
{
extern UIApplication*  g_pUIApplication;


// 
//	�����ڴ������ڵ�ʱ�����ص�һ��������Ϣ�����������໯
//
class CREATE_WND_DATA  // ��ͬ�� ATL �е�_ATL_WIN_MODULE70
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
//	UI���󴴽���Ϣ
//
struct UI_CREATE_DATA 
{
	String                 m_strXmlName;
	s_UICreateInstancePtr  m_fun;
};


// ����һ��������Ϣ���ڣ�����ʵ��post ui message
class CForwardPostMessageWindow : public CWindowImpl<CForwardPostMessageWindow>
{
public:
// 	BEGIN_MSG_MAP_EX(CForwardPostMessageWindow)
// 	END_MSG_MAP()
	
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);
};

//
//	ȫ�ֶ���
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

	//  ȫ�ֶ���
public:
	//LogManager     m_MgrLog;
	
public:
	CComPtr<ILog>    m_pLog;
	TopWindowManager m_TopWindowMgr; 
	ProjectManager   m_ProjectMgr;
	ToolTipManager   m_ToolTipMgr;

	CREATE_WND_DATA  m_create_wnd_data;    // ���UI_AddCreateWndData��UI_ExtractCreateWndDataʹ��
	CDCCache         m_dcCache;            // ���UI_GetCacheDC��UI_ReleaseCacheDCʹ��
	CForwardPostMessageWindow   m_WndForwardPostMsg;  // ���UI_GetForwardPostMessageWndʹ��
	
private:
	static HINSTANCE m_hInstance;          // ��ǰDLL�����������չ·��
	bool             m_bDesignMode;        // �Ƿ������ģʽ

	vector<UI_CREATE_DATA*>   m_vecUICreateData;

	OSVERSIONINFOEX  m_osvi;               // ����ϵͳ�汾

};

}