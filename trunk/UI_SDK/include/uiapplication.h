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
//	ȫ�ֶ���
//
class UIApplication
{
public:
	UIApplication(void);
	~UIApplication(void);

private:
	void RegisterWndClass();
	bool InitLog( const String& );

public:
	// ---------- 
	bool Initialize( const String& strUIProjXmlPath );

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
	ILogPtr          m_pLog;
	TopWindowManager m_TopWindowMgr; 
	ProjectManager   m_ProjectMgr;
	RenderManager    m_RenderMgr;

	CREATE_WND_DATA  m_create_wnd_data;    // ���UI_AddCreateWndData��UI_ExtractCreateWndDataʹ��
	CDCCache         m_dcCache;            // ���UI_GetCacheDC��UI_ReleaseCacheDCʹ��

private:
	static HINSTANCE m_hInstance;          // ��ǰDLL�����������չ·��
	bool             m_bDesignMode;        // �Ƿ������ģʽ

	

};

}