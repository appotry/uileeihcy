#include "stdafx.h"

//
// ȫ�ֶ���
namespace UI
{
	UIApplication*     g_pUIApplication = NULL;
}


CREATE_WND_DATA::CREATE_WND_DATA()
{
	m_pCreateWndList = NULL;
	::InitializeCriticalSection(&m_cs);
}
CREATE_WND_DATA::~CREATE_WND_DATA()
{
	::DeleteCriticalSection(&m_cs);
}

void  CREATE_WND_DATA::AddCreateWndData(_AtlCreateWndData* pData, void* pThis)
{
	UIASSERT(pData != NULL && pThis != NULL);

	pData->m_pThis = pThis;
	pData->m_dwThreadID = ::GetCurrentThreadId();
	
	EnterCriticalSection(&m_cs);

	pData->m_pNext = this->m_pCreateWndList;
	this->m_pCreateWndList = pData;

	LeaveCriticalSection(&m_cs);
}
void*  CREATE_WND_DATA::ExtractCreateWndData()
{
	void* pv = NULL;
	EnterCriticalSection(&m_cs);

	_AtlCreateWndData* pEntry = this->m_pCreateWndList;
	if(pEntry != NULL)
	{
		DWORD dwThreadID = ::GetCurrentThreadId();
		_AtlCreateWndData* pPrev = NULL;
		while(pEntry != NULL)
		{
			if(pEntry->m_dwThreadID == dwThreadID)
			{
				if(pPrev == NULL)
					this->m_pCreateWndList = pEntry->m_pNext;
				else
					pPrev->m_pNext = pEntry->m_pNext;
				pv = pEntry->m_pThis;
				break;
			}
			pPrev = pEntry;
			pEntry = pEntry->m_pNext;
		}
	}

	LeaveCriticalSection(&m_cs);
	return pv;
}

//////////////////////////////////////////////////////////////////////////

BOOL CForwardPostMessageWindow::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
{
	if (UI_WM_POSTMESSAGE==uMsg)
	{
		UIMSG* pMsg = (UIMSG*)wParam;
		UISendMessage(pMsg, lParam);
		delete pMsg;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

HINSTANCE UIApplication::m_hInstance = NULL;
UIApplication::UIApplication(void)
{
	::CoInitialize(0);
	m_pLog = NULL;
	m_bDesignMode = false;

	/* INITIALIZE COMMON CONTROLS, tooltip support */
	INITCOMMONCONTROLSEX iccex; 
	iccex.dwICC = ICC_WIN95_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	RegisterWndClass();
	RegisterDefaultUIObject();
}

UIApplication::~UIApplication(void)
{
	// Ӧ�ó����˳���־
	UI_LOG_INFO( _T("------------  UI Over ----------------\n"));

	if( NULL != m_pLog )
	{
		m_pLog.Release();
		m_pLog = NULL;
	}

	int nSize = (int)m_vecUICreateData.size();
	for (int i = 0; i < nSize; i++)
	{
		UI_CREATE_DATA* pData = m_vecUICreateData[i];
		SAFE_DELETE(pData);
	}
	m_vecUICreateData.clear();

	if (m_WndForwardPostMsg.IsWindow())
	{
		m_WndForwardPostMsg.DestroyWindow();
	}
	::CoUninitialize(); // do not call CoInitialize, CoInitializeEx, or CoUninitialize from the DllMain function. 
}


bool UIApplication::Initialize( const String& strUIProjXmlPath )
{
	if( false == this->InitLog(strUIProjXmlPath) )
	{
#ifdef _DEBUG
		//::MessageBox(NULL,_T("Initialize log module failed."), _T("Warning"), MB_OK|MB_ICONWARNING );
#endif
	//	return false; <-- �п�����û��ע��uilog.dll���µģ��ݲ����ش���
	}

	// ��ʼ��Gdiplus
	Image::InitGDIPlus();

	// ����һ������ת����Ϣ�Ĵ��ڣ�ʵ��post ui message
	m_WndForwardPostMsg.Create(HWND_MESSAGE);

	return m_ProjectMgr.Initialize( strUIProjXmlPath );
}

//
//	һ���յĴ��ڹ��̣���ΪUI���������Ĵ��ڹ�������Ҫ���޸ĳ�Ϊһ����ĳ�Ա������
//  �������Ĵ��ڹ���ֻ���������WNDCLASS������
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc( hWnd, message, wParam, lParam );
}

/*
**	[private] void  RegisterWndClass()
**
**	Parameters
**
**	Return
**		NA
**
**	Remark
**		Window����Ĵ���ʹ�õĴ��������������ע��ġ�
**
**	See Also
*/
void UIApplication::RegisterWndClass()
{
	WNDCLASSEX wcex;
	
	// ע��UI��ͨ������
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
	wcex.lpfnWndProc	= WindowBase::StartWindowProc;//WndProc;   ����windowsĬ�ϵĴ��ڹ���
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;//::AfxGetInstanceHandle();;
	wcex.hIcon			= 0;//LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WND_CLASS_NAME;
	wcex.hIconSm		= 0;//LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
	RegisterClassEx(&wcex);

	// ע������ʵ�ֶ����Ĵ�����
	wcex.lpszClassName  = WND_ANIMATE_CLASS_NAME;
	wcex.lpfnWndProc    = ::DefWindowProc;
	RegisterClassEx(&wcex);


	// ע������ʵ�ֲ˵�������ʽ�б��Ĵ���(����Ӱ/������Ӱ)
	wcex.lpszClassName  = WND_POPUP_CONTROL_SHADOW_NAME;
	wcex.style          = CS_DROPSHADOW;
	wcex.lpfnWndProc    = WindowBase::StartWindowProc;;
	RegisterClassEx(&wcex);

	wcex.lpszClassName  = WND_POPUP_CONTROL_NAME;
	wcex.style          = 0;
	wcex.lpfnWndProc    = WindowBase::StartWindowProc;;
	RegisterClassEx(&wcex);
}

//
//	��ʼ����־����
//
//	Parameter
//		logXmlPath
//			[in]  ��־�����ļ���ȫ·��
//
bool UIApplication::InitLog( const String& logXmlPath )
{
	HRESULT hr = S_OK;
	if ( NULL == m_pLog )
	{
		hr = m_pLog.CoCreateInstance(__uuidof(Log) );
		if( FAILED(hr) )
			return false;
	}
	
	//_bstr_t bstr = logXmlPath.c_str();
	CComBSTR bstr(logXmlPath.c_str());
	long lRet = 0;
	hr = m_pLog->Load( bstr, &lRet );
	if( FAILED(hr) || 0 == lRet )
		return false;

	return true;
}

// -----------     from uibuilder manager ----






#if 0
// bool UIApplication::CreateStyleXml ( HSKIN hSkin,   const String& strXmlPath )
// {
// 	// 1. �����ļ�
// 	if( FALSE == PathFileExists(strXmlPath.c_str()) )
// 	{
// 		if( false == this->_CreateEmptyXmlFile(strXmlPath) )
// 			return false;
// 	}
// 
// 	// 2. ���ص�������
// 	m_vXmlSkin[hSkinXml]->m_xmlStyle.LoadXml(strXmlPath);
// 
// 	// 4. ��������Ϣ��ӵ�Ƥ��xml��
// 	m_vXmlSkin[hSkinXml]->m_xmlUI.SetStyleXml( strXmlPath );
// 
// 	// 5. ���ñ����־
// 	this->SetDirty(true);
// 
// 	return true;
// }
// bool UIApplication::CreateLayoutXml( HSKIN hSkin,   const String& strXmlPath )
// {
// 	// 1. �����ļ�
// 	if( FALSE == PathFileExists(strXmlPath.c_str()) )
// 	{
// 		if( false == this->_CreateEmptyXmlFile(strXmlPath) )
// 			return false;
// 	}
// 
// 	// 2. ���ص�������
// 	m_vXmlSkin[hSkinXml]->m_xmlLayout.LoadXml(strXmlPath);
// 
// 	// 4. ��������Ϣ��ӵ�Ƥ��xml��
// 	m_vXmlSkin[hSkinXml]->m_xmlUI.SetLayoutXml( strXmlPath );
// 
// 	// 5. ���ñ����־
// 	this->SetDirty(true);
// 
// 	return true;
// }
#endif


// HSKIN UIApplication::GetNextProjectSkin( HSKIN hSkin /*= NULL*/ )
// {
// 	return m_ProjectMgr.GetNextProjectSkin(hSkin);
// }
 
 
//
//	���������޸Ĺ����ļ�(��־ģ��ı�������縺����ã���uibuilder)
//
void UIApplication::Save( HSKIN hSkin, UI_RESOURCE_TYPE eResType )
{
	m_ProjectMgr.Save(hSkin,eResType);
}

//
//	Ϊ��ʵ��UI����Ĵ��������ַ������������Ӧ���ࣩ����app���б���������UI����Ĵ�����Ϣ��
//
//	ע: �������������UICreateObject�����ӳ���б��޷����������ʵ��һ��UI������������Щ
//      ���뽫��ӳ���б���Ϊ��̬���顣��������ʵ����һ��UI��ʱ����app��ע���䴴����Ϣ��
//

bool UIApplication::RegisterUICreateData(const String& strXmlName, s_UICreateInstancePtr pfun)
{
	if (NULL == pfun || strXmlName.empty())
	{
		UI_LOG_WARN(_T("%s invalid arg. name=%s, pfun=%x"), _T(__FUNCTION__), strXmlName.c_str(), pfun);
		return false;
	}
	
	int nSize = (int)m_vecUICreateData.size();
	for (int i = 0; i < nSize; i++)
	{
		UI_CREATE_DATA* pData = m_vecUICreateData[i];
		if (NULL != pData && pData->m_strXmlName == strXmlName)
		{
			UI_LOG_WARN(_T("%s register duplicate. name=%s, pfun=%x"), _T(__FUNCTION__), strXmlName.c_str(), pfun);
			return false;
		}
	}

	UI_CREATE_DATA* pData = new UI_CREATE_DATA;
	pData->m_strXmlName = strXmlName;
	pData->m_fun = pfun;
	
	m_vecUICreateData.push_back(pData);
	return true;
}

bool UIApplication::GetUICreateInstanceFuncPtr(const String& strXmlName, s_UICreateInstancePtr* pOut)
{
	if (NULL == pOut)
		return false;
	
	int nSize = (int)m_vecUICreateData.size();
	for (int i = 0; i < nSize; i++)
	{
		UI_CREATE_DATA* pData = m_vecUICreateData[i];
		if (NULL != pData && pData->m_strXmlName == strXmlName)
		{
			*pOut = pData->m_fun;
			return true;
		}
	}

	UI_LOG_WARN(_T("%s get func ptr failed. name=%s"), _T(__FUNCTION__), strXmlName.c_str());
	return false;
}

void UIApplication::RegisterDefaultUIObject()
{

#define REGISTER_UI_OBJECT(className) \
	this->RegisterUICreateData(className::XmlName(),(s_UICreateInstancePtr)className::_CreatorClass::UICreateInstance);

	REGISTER_UI_OBJECT( Panel )
	REGISTER_UI_OBJECT( Button );
	REGISTER_UI_OBJECT( GroupBox );
	REGISTER_UI_OBJECT( ScrollPanel );
	REGISTER_UI_OBJECT( HwndHost );
	REGISTER_UI_OBJECT( CheckButton );
	REGISTER_UI_OBJECT( RadioButton );
	REGISTER_UI_OBJECT( HyperLink );
	REGISTER_UI_OBJECT( Label );
	REGISTER_UI_OBJECT( Picture );
	REGISTER_UI_OBJECT( Edit );
	REGISTER_UI_OBJECT( SliderCtrl );
	REGISTER_UI_OBJECT( ListBox );
	REGISTER_UI_OBJECT( HScrollBar );
	REGISTER_UI_OBJECT( VScrollBar );
	REGISTER_UI_OBJECT( Combobox );
	REGISTER_UI_OBJECT( RichEdit );
	REGISTER_UI_OBJECT( LEDCtrl );
}