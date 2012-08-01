#include "stdafx.h"

//
// 全局对象
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
}

UIApplication::~UIApplication(void)
{
	// 应用程序退出日志
	UI_LOG_INFO( _T("------------  UI Over ----------------\n"));

	if( NULL != m_pLog )
	{
		m_pLog.Release();
		m_pLog = NULL;
	}
	::CoUninitialize(); // do not call CoInitialize, CoInitializeEx, or CoUninitialize from the DllMain function. 
}


bool UIApplication::Initialize( const String& strUIProjXmlPath )
{
	if( false == this->InitLog(strUIProjXmlPath) )
	{
#ifdef _DEBUG
		::MessageBox(NULL,_T("Initialize log module failed."), _T("Warning"), MB_OK|MB_ICONWARNING );
#endif
	//	return false; <-- 有可能是没有注册uilog.dll导致的，暂不返回错误
	}

	// 初始化Gdiplus
	Image::InitGDIPlus();

	return m_ProjectMgr.Initialize( strUIProjXmlPath );
}

//
//	一个空的窗口过程，因为UI这个窗口类的窗口过程最终要被修改成为一个类的成员函数，
//  因此这里的窗口过程只是用来填充WNDCLASS参数。
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
**		Window对象的窗口使用的窗口类就是在这里注册的。
**
**	See Also
*/
void UIApplication::RegisterWndClass()
{
	WNDCLASSEX wcex;
	
	// 注册UI普通窗口类
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WindowBase::StartWindowProc;//WndProc;   改用windows默认的窗口过程
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

	// 注册用于实现动画的窗口类
	wcex.lpszClassName  = WND_ANIMATE_CLASS_NAME;
	wcex.lpfnWndProc    = ::DefWindowProc;
	RegisterClassEx(&wcex);


	// 注册用于实现菜单，弹出式列表框的窗口(带阴影/不带阴影)
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
//	初始化日志对象
//
//	Parameter
//		logXmlPath
//			[in]  日志配置文件的全路径
//
bool UIApplication::InitLog( const String& logXmlPath )
{
	HRESULT hr = S_OK;
	if ( NULL == m_pLog )
	{
		hr = m_pLog.CreateInstance(__uuidof(Log) );
		if( FAILED(hr) )
			return false;
	}
	
	_bstr_t bstr = logXmlPath.c_str();
	hr = m_pLog->Load( bstr );
	if( FAILED(hr))
		return false;

	return true;
}

// -----------     from uibuilder manager ----






#if 0
// bool UIApplication::CreateStyleXml ( HSKIN hSkin,   const String& strXmlPath )
// {
// 	// 1. 创建文件
// 	if( FALSE == PathFileExists(strXmlPath.c_str()) )
// 	{
// 		if( false == this->_CreateEmptyXmlFile(strXmlPath) )
// 			return false;
// 	}
// 
// 	// 2. 加载到解析器
// 	m_vXmlSkin[hSkinXml]->m_xmlStyle.LoadXml(strXmlPath);
// 
// 	// 4. 将字体信息添加到皮肤xml中
// 	m_vXmlSkin[hSkinXml]->m_xmlUI.SetStyleXml( strXmlPath );
// 
// 	// 5. 设置保存标志
// 	this->SetDirty(true);
// 
// 	return true;
// }
// bool UIApplication::CreateLayoutXml( HSKIN hSkin,   const String& strXmlPath )
// {
// 	// 1. 创建文件
// 	if( FALSE == PathFileExists(strXmlPath.c_str()) )
// 	{
// 		if( false == this->_CreateEmptyXmlFile(strXmlPath) )
// 			return false;
// 	}
// 
// 	// 2. 加载到解析器
// 	m_vXmlSkin[hSkinXml]->m_xmlLayout.LoadXml(strXmlPath);
// 
// 	// 4. 将字体信息添加到皮肤xml中
// 	m_vXmlSkin[hSkinXml]->m_xmlUI.SetLayoutXml( strXmlPath );
// 
// 	// 5. 设置保存标志
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
//	保存所有修改过的文件(日志模块的保存由外界负责调用，如uibuilder)
//
void UIApplication::Save( HSKIN hSkin, UI_RESOURCE_TYPE eResType )
{
	m_ProjectMgr.Save(hSkin,eResType);
}
