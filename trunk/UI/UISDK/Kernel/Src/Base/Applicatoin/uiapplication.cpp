#include "stdafx.h"
#include "uiapplication.h"

#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\UIObject\Control\control.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"
#include "UISDK\Kernel\Src\Renderbase\colorrender\colorrender.h"
#include "UISDK\Kernel\Src\Renderbase\textrenderbase\textrender.h"
#include "UISDK\Kernel\Src\Renderbase\imagerender\imagerender.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Src\Helper\layout\stacklayout.h"
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"
#include "UISDK\Kernel\Src\Helper\layout\cardlayout.h"
#include "UISDK\Kernel\Src\Helper\layout\gridlayout.h"
#include "UISDK\Kernel\Src\Helper\layout\averagelayout.h"
#include "UISDK\Kernel\Src\UIObject\HwndHost\HwndHost.h"
#include "UISDK\Kernel\Src\UIObject\Panel\ScrollPanel\scrollpanel.h"
// #include "UISDK\Kernel\Src\Window\tooltipwindow.h"
#include "UISDK\Kernel\Src\Resource\imagemanager.h"
#include "UISDK\Kernel\Src\Resource\colormanager.h"
#include "UISDK\Kernel\Src\Resource\fontmanager.h"
#include "UISDK\Kernel\Src\Resource\stylemanager.h"
#include "UISDK\Kernel\Src\Resource\layoutmanager.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"
#include "UISDK\Kernel\Inc\Interface\iuieditor.h"

ILog*     UIApplication::s_pLog = NULL;
long      UIApplication::s_lUiLogCookie = 0;
long      UIApplication::s_lAppCount = 0;

UIApplication::UIApplication(IUIApplication* p) : 
    m_WndForwardPostMsg(this), 
    m_TopWindowMgr(this),
    m_ToolTipMgr(this)
{
    s_lAppCount++;

//	::CoInitialize(0);
	OleInitialize(0);  // 需要注册richedit的drag drop，因此用ole初始化
    m_pUIApplication = p;

	m_bDesignMode = false;
	m_pUIEditor = NULL;

    m_pSkinMgr = NULL;
    m_pSkinMgr = new SkinManager;
	m_pSkinMgr->SetUIApplication(m_pUIApplication);

    m_pAnimateMgr = NULL;
    m_pAnimateMgr = new AnimateManager;
    m_pAnimateMgr->SetUIApplication(this);

    // 获取操作系统版本信息
    ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
    m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*) &m_osvi);

    // 设置当前语言。主要是用于 strcoll 中文拼音排序(如：combobox排序)(TODO:这一个是不是需要作成一个配置项？)
    _wsetlocale( LC_ALL, _T("chs") );

    // 初始化Gdiplus
    // 注：gdiplus会创建一个背景线程：GdiPlus.dll!BackgroundThreadProc()  + 0x59 字节	
    Image::InitGDIPlus();

    /* INITIALIZE COMMON CONTROLS, tooltip support */
    INITCOMMONCONTROLSEX iccex; 
    iccex.dwICC = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

    // 创建一个用于转发消息的窗口，实现post ui message
    m_WndForwardPostMsg.Create(HWND_MESSAGE);

	// 针对layer window防止无响应时窗口变黑
	//DisableProcessWindowsGhosting();

    RegisterDefaultUIObject(); 
    RegisterWndClass();
}

UIApplication::~UIApplication(void)
{
    s_lAppCount--;

	// 应用程序退出日志
	UI_LOG_INFO( _T("\n\n------------  UI Quit ----------------\n"));

	this->m_ToolTipMgr.Release();  // 保证顶层窗口计数为0

#ifdef _DEBUG
	int nCount = this->m_TopWindowMgr.GetTopWindowCount();
	if (0 != nCount)   // <-- 该方法有可能还是不准，有可能窗口被销毁了，但窗口对象还没有析构
	{
		UI_LOG_WARN(_T("UI_Exit TopWindowCount=%d"),nCount);  
	}
#endif

    if (0 == s_lAppCount)
	    SAFE_RELEASE(s_pLog);

    ClearRegisterUIObject();

	if (m_WndForwardPostMsg.IsWindow())
	{
		m_WndForwardPostMsg.DestroyWindow();
	}
	m_pUIEditor = NULL;
    SAFE_DELETE(m_pSkinMgr);     // SkinRes中引用了MSXML Com指针，需要在OleUninitialize前释放
	SAFE_DELETE(m_pAnimateMgr)  // 如果不先调用的话，会报: 0xC0000096: Privileged instruction. （是不是与ole uninitialize 有关?)

    Image::ReleaseGDIPlus();

//	::CoUninitialize(); // do not call CoInitialize, CoInitializeEx, or CoUninitialize from the DllMain function. 
	OleUninitialize();
}

HRESULT  UIApplication::SetSkinDirection(const TCHAR* szDir)
{
    if (!m_pSkinMgr)
        return E_FAIL;

    m_pSkinMgr->SetSkinDirection(szDir);
    return S_OK;
}
HRESULT  UIApplication::LoadSkin(const TCHAR* szSkinName)
{
    UI_LOG_INFO( _T("\n\n------------  LoadSkin ----------------\n"));

    if (NULL == szSkinName)
    {
    }
    else
    {
		bool bRet = m_pSkinMgr->LoadSkin(szSkinName);
		if (false == bRet)
		{
			SAFE_RELEASE(s_pLog);
			return E_FAIL;
		}
	}
	return S_OK;
}


// 外部调用，共享日志指针
HRESULT UIApplication::SetLog(ILog* pLog, const TCHAR* szLogXmlPath)
{
	SAFE_RELEASE(s_pLog);
	if (pLog)
	{
		s_pLog = pLog;
		pLog->AddRef();

		ILogItem* pLogItem = NULL;
		pLog->GetLogItemByName(L"ui", &pLogItem);
		if (pLogItem)
		{
			pLogItem->GetCookie(&s_lUiLogCookie);
		}
		return S_OK;
	}
	if (szLogXmlPath)
	{
		return this->InitLog(szLogXmlPath);
	}
	return E_FAIL;
}
HRESULT UIApplication::GetLog(ILog** ppLog)
{
	if (NULL == s_pLog || NULL == ppLog)
		return E_FAIL;

	*ppLog = s_pLog;
	(*ppLog)->AddRef();
	return S_OK;
}

HRESULT UIApplication::SetTooltipsUI(IToolTipUI* pUI)
{
	m_ToolTipMgr.Init(pUI);
	return S_OK;
}

HRESULT UIApplication::UseInnerTooltipsUI(const TCHAR* szWndID)
{
	if (NULL == szWndID)
		return false;
#if 0 // -- 架构改造
	TooltipWindow* pWindow = new TooltipWindow;
	static_cast<CustomWindow*>(pWindow)->Create(this, String(szWndID));
	
	return this->SetTooltipsUI(pWindow);
#endif 
    return 0;
}

ISkinManager* UIApplication::GetSkinMgr()
{
	return m_pSkinMgr->GetISkinManager();
}

ITopWindowManager* UIApplication::GetTopWindowMgr()
{
	return m_TopWindowMgr.GetITopWindowManager();
}

IAnimateManager* UIApplication::GetAnimateMgr()
{
	return m_pAnimateMgr->GetIAnimateManager();
}

HRESULT UIApplication::GetWaitForHandlesMgr(IWaitForHandlesMgr** ppMgr)
{
	if (NULL == ppMgr)
		return E_INVALIDARG;

	*ppMgr = &m_WaitForHandlesMgr;
	return S_OK;
}

HRESULT UIApplication::GetMessageFilterMgr(IMessageFilterMgr** ppMgr)
{
	if (NULL == ppMgr)
		return E_INVALIDARG;

	*ppMgr = &m_MsgFilterMgr;
	return S_OK;
}

ISkinRes* UIApplication::GetActiveSkinRes()
{
	return m_pSkinMgr->GetActiveSkin();
}

IImageManager*  UIApplication::GetActiveSkinImageMgr()
{
	ISkinRes* pSkinRes = GetActiveSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return pSkinRes->GetImageManager();
}
IImageRes*  UIApplication::GetActiveSkinImageRes()
{
	IImageManager* pImageMgr = GetActiveSkinImageMgr();
	if (NULL == pImageMgr)
		return NULL;

	return pImageMgr->GetImageRes();
}
ICursorRes*  UIApplication::GetActiveSkinCursorRes()
{
	IImageManager* pImageMgr =GetActiveSkinImageMgr();
	if (NULL == pImageMgr)
		return NULL;

	return pImageMgr->GetCursorRes();
}
IGifRes*  UIApplication::GetActiveSkinGifRes()
{
	IImageManager* pImageMgr = GetActiveSkinImageMgr();
	if (NULL == pImageMgr)
		return NULL;

	return pImageMgr->GetGifRes();
}
IFontManager*  UIApplication::GetActiveSkinFontMgr()
{
	ISkinRes* pSkinRes = GetActiveSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return pSkinRes->GetFontManager();
}
IFontRes*  UIApplication::GetActiveSkinFontRes()
{
	IFontManager* pFontMgr = GetActiveSkinFontMgr();
	if (NULL == pFontMgr)
		return NULL;

	return pFontMgr->GetFontRes();
}
IColorManager*  UIApplication::GetActiveSkinColorMgr()
{
	ISkinRes* pSkinRes = GetActiveSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return pSkinRes->GetColorManager();
}
IColorRes*  UIApplication::GetActiveSkinColorRes()
{
	IColorManager* pColorMgr = GetActiveSkinColorMgr();
	if (NULL == pColorMgr)
		return NULL;

	return pColorMgr->GetColorRes();
}
IStyleManager*  UIApplication::GetActiveSkinStyleMgr()
{
	ISkinRes* pSkinRes = GetActiveSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return pSkinRes->GetStyleManager();
}
IStyleRes*  UIApplication::GetActiveSkinStyleRes()
{
	IStyleManager* pStyleMgr = GetActiveSkinStyleMgr();
	if (NULL == pStyleMgr)
		return NULL;

	return pStyleMgr->GetStyleRes();
}
ILayoutManager*  UIApplication::GetActiveSkinLayoutMgr()
{
	ISkinRes* pSkinRes = GetActiveSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return pSkinRes->GetLayoutManager();
}
ILayoutRes*  UIApplication::GetActiveSkinLayoutRes()
{
	ILayoutManager* pLayoutMgr = GetActiveSkinLayoutMgr();
	if (NULL == pLayoutMgr)
		return NULL;

	return pLayoutMgr->GetLayoutRes();
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
	wcex.style			= /*CS_HREDRAW | CS_VREDRAW |*/CS_DBLCLKS;  // <-- 分层窗口最大化时收不到redraw消息，因此直接在OnSize中刷新
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
HRESULT UIApplication::InitLog(const TCHAR* szLogConfigFilePath)
{
	HRESULT hr = S_OK;
	if (NULL == s_pLog)
	{
		hr = ::CoCreateInstance(__uuidof(Log), NULL, CLSCTX_INPROC, __uuidof(ILog), (void**)&s_pLog);
		if (FAILED(hr))
			return hr;
	}
	
	hr = s_pLog->Load((BSTR)szLogConfigFilePath);
	if (FAILED(hr))
	{
		SAFE_RELEASE(s_pLog);
		return hr;
	}

	ILogItem* pLogItem = NULL;
	s_pLog->GetLogItemByName(L"ui", &pLogItem);
	if (pLogItem)
	{
		pLogItem->GetCookie(&s_lUiLogCookie);
	}

	return S_OK;
}

// -----------     from uibuilder manager ----

HRESULT UIApplication::SetDesignMode(VARIANT_BOOL b, IUIEditor* pUIBuilder) 
{
	m_bDesignMode = b==VARIANT_TRUE?true:false;
	m_pUIEditor = pUIBuilder; 

	return S_OK;
}
IUIEditor* UIApplication::GetUIEditorPtr()
{	
	return m_pUIEditor;
}
bool UIApplication::IsUnderXpOS()
{
	bool bUnderXpOs = true;;
	if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId)
	{
		if (m_osvi.dwMajorVersion >= 6)
		{
			bUnderXpOs = false;
		}
	}
	else
	{
		bUnderXpOs = false;
	}
	return bUnderXpOs;
}

bool UIApplication::IsVistaOrWin7etc()
{
	bool bHighThanVista = true;;
	if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId)
	{
		if (m_osvi.dwMajorVersion < 6)
		{
			bHighThanVista = false;
		}
	}
	else
	{
		bHighThanVista = false;
	}
	return bHighThanVista;
}

bool UIApplication::IsUIObjectAvailable(IMessage* p)
{	
	if (NULL == p)
		return false;

	list<IMessage*>::iterator iter = std::find(m_aliveUIObject.begin(), m_aliveUIObject.end(), p);
	if (iter == m_aliveUIObject.end())
		return false;

	return true;
}
HRESULT UIApplication::AddUIObject(IMessage* p)
{
	if (NULL == p)
		return E_INVALIDARG;

	list<IMessage*>::iterator iter = std::find(m_aliveUIObject.begin(), m_aliveUIObject.end(), p);
	if (iter == m_aliveUIObject.end())
	{
		m_aliveUIObject.push_back(p);
		return S_OK;
	}

	return E_FAIL;
}
HRESULT UIApplication::RemoveUIObject(IMessage* p)
{
	if (NULL == p)
		return E_FAIL;

	list<IMessage*>::iterator iter = std::find(m_aliveUIObject.begin(), m_aliveUIObject.end(), p);
	if (iter != m_aliveUIObject.end())
	{
		m_aliveUIObject.erase(iter);
		return S_OK;
	}


	return E_FAIL;
}

HRESULT  UIApplication::GetSkinTagParseFunc(const TCHAR* szTag, funcUIParseSkinElement* pFunc)
{
    if (NULL == szTag || NULL == pFunc)
        return E_INVALIDARG;

    UISKINTAGPARSE_DATA::iterator iter = m_mapSkinTagParseData.find(szTag);
    if (iter == m_mapSkinTagParseData.end())
        return E_FAIL;

    *pFunc = iter->second;
    return S_OK;
}

HRESULT  UIApplication::RegisterLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement func)
{
    if (NULL == szTag || NULL == func)
        return E_FAIL;

    m_mapLayoutTagParseData[szTag] = func;
    return S_OK;
}

HRESULT  UIApplication::GetLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement* pFunc)
{
    if (NULL == szTag || NULL == pFunc)
        return E_INVALIDARG;

    UILAYOUTTAGPARSE_DATA::iterator iter = m_mapLayoutTagParseData.find(szTag);
    if (iter == m_mapLayoutTagParseData.end())
        return E_FAIL;

    *pFunc = iter->second;
    return S_OK;
}

//
//	为了实现UI对象的创建（从字符串来创建其对应的类），在app类中保存了所有UI对象的创建信息。
//
//	注: 如果仅仅采用在UICreateObject中添加映射列表，无法处理第三方实现一个UI对象的情况，因些
//      必须将该映射列表保存为动态数组。当第三方实现了一个UI类时，向app来注册其创建信息。
//

HRESULT UIApplication::RegisterUIObjectCreateData(const TCHAR* bstrXmlName, 
                                                  const TCHAR* szCategory, UINT nObjType, 
                                                  REFCLSID guid, funcUICreateInstancePtr pfun)
{
	if (NULL == pfun || NULL == bstrXmlName)
	{
		UI_LOG_WARN(_T("%s invalid arg. name=%s, pfun=%x"), FUNC_NAME, bstrXmlName, pfun);
		return E_INVALIDARG;
	}
	
	int nSize = (int)m_vecUICreateData.size();
	for (int i = 0; i < nSize; i++)
	{
		UIOBJ_CREATE_INFO* pData = m_vecUICreateData[i];
		if (pData && pData->m_strXmlName == bstrXmlName)
		{
			UI_LOG_WARN(_T("%s register duplicate. name=%s, pfun=%x"), FUNC_NAME, bstrXmlName, pfun);
			return E_FAIL;
		}
	}

	UIOBJ_CREATE_INFO* pData = new UIOBJ_CREATE_INFO;
	pData->m_strXmlName = bstrXmlName;
    pData->m_nObjType = nObjType;
    memcpy(&pData->m_guid, &guid, sizeof(GUID));
	pData->m_func = pfun;
    if (szCategory)
        pData->m_strCategory = szCategory;
	
	m_vecUICreateData.push_back(pData);

    LPOLESTR lpolestr = NULL;
    StringFromIID(guid, &lpolestr);
    UI_LOG_DEBUG(_T("%s  %s %s @ 0x%08X"), FUNC_NAME, bstrXmlName, lpolestr, pfun);
    CoTaskMemFree(lpolestr);
      
	return S_OK;
}

bool UIApplication::GetUICreateInstanceFuncPtr(const TCHAR* bstrXmlName, funcUICreateInstancePtr* pOut)
{
	if (NULL == bstrXmlName || NULL == pOut)
		return false;
	
    UIOBJ_CREATE_DATA::iterator iter = m_vecUICreateData.begin();
	for (; iter != m_vecUICreateData.end(); iter++)
	{
		UIOBJ_CREATE_INFO* pData = *iter;
		if (pData && pData->m_strXmlName == bstrXmlName)
		{
			*pOut = pData->m_func;
			return true;
		}
	}

	UI_LOG_DEBUG(_T("%s get func ptr failed. name=%s"), FUNC_NAME, bstrXmlName);  // 从warn改成debug级别。可能是renderchain，它由别的方式来创建
	return false;
}

bool UIApplication::GetUICreateInstanceFuncPtr(REFCLSID guid, funcUICreateInstancePtr* pOut)
{
    if (NULL == pOut)
        return false;

    int nSize = (int)m_vecUICreateData.size();
    for (int i = 0; i < nSize; i++)
    {
        UIOBJ_CREATE_INFO* pData = m_vecUICreateData[i];
        if (NULL == pData)
            continue;

        if (::IsEqualIID(guid, pData->m_guid))
        {
            *pOut = pData->m_func;
            return true;
        }
    }

    LPOLESTR pText = NULL;
    StringFromIID(guid, &pText);
    if (pText)
    {
        UI_LOG_WARN(_T("%s get func ptr failed. pGuid=%s"), FUNC_NAME, pText);
        ::CoTaskMemFree(pText);
    }
    return false;
}

void  UIApplication::ClearRegisterUIObject()
{
#define vec_clear(type, var)                  \
    {                                         \
        type::iterator iter = var.begin();    \
        for ( ; iter != var.end(); iter++ )   \
            SAFE_DELETE(*iter);               \
        var.clear();                          \
    }

    vec_clear(UIOBJ_CREATE_DATA,            m_vecUICreateData);
    vec_clear(UIRENDERBASE_CREATE_DATA,     m_vecUIRenderBaseCreateData);
    vec_clear(UITEXTRENDERBASE_CREATE_DATA, m_vecUITextRenderBaseCreateData);
    vec_clear(UILAYOUT_CREATE_DATA,         m_vecUILayoutCreateData);
}

void UIApplication::RegisterDefaultUIObject()
{
//     REGISTER_UI_OBJECT (this, Object);
//     REGISTER_UI_OBJECT (this, Control);
    REGISTER_UI_OBJECT (this, Panel);
//     REGISTER_UI_OBJECT (this, WindowBase);
    REGISTER_UI_OBJECT (this, Window);
    REGISTER_UI_OBJECT (this, CustomWindow);
    REGISTER_UI_OBJECT (this, HwndHost);
    REGISTER_UI_OBJECT (this, ScrollPanel);

    REGISTER_UI_RENDERBASE(this, NullRender)
    REGISTER_UI_RENDERBASE(this, ColorRender);
    REGISTER_UI_RENDERBASE(this, SysColorRender);
    REGISTER_UI_RENDERBASE(this, GradientRender);
    REGISTER_UI_RENDERBASE(this, ColorListRender);
    REGISTER_UI_RENDERBASE(this, ImageRender);
    REGISTER_UI_RENDERBASE(this, ImageListItemRender);
    REGISTER_UI_RENDERBASE(this, ImageListRender);

    REGISTER_UI_TEXTRENDERBASE(this, NormalTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, ColorListTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, FontColorListTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, ContrastColorTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, ContrastColorListTextRender)
        
    REGISTER_UI_LAYOUT(this, _T("canvas"), LAYOUT_TYPE_CANVAS, CanvasLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("stack"),  LAYOUT_TYPE_STACK,  StackLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("card"),   LAYOUT_TYPE_CARD,   CardLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("grid"),   LAYOUT_TYPE_GRID,   GridLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("average"),   LAYOUT_TYPE_AVERAGE,   AverageLayout::CreateInstance)

    m_mapSkinTagParseData[XML_IMG] = ImageManager::UIParseSkinElement;
    m_mapSkinTagParseData[XML_COLOR] = ColorManager::UIParseSkinElement;
    m_mapSkinTagParseData[XML_FONT] = FontManager::UIParseSkinElement;
    m_mapSkinTagParseData[XML_STYLE] = StyleManager::UIParseSkinElement;
    m_mapSkinTagParseData[XML_LAYOUT] = LayoutManager::UIParseSkinElement;
    m_mapSkinTagParseData[XML_INCLUDE] = SkinParseEngine::UIParseIncludeElement;

    m_mapLayoutTagParseData[XML_RENDERCHAIN] = RenderChain::LoadRenderChain;

 //   m_mapLayoutTagParseData[_T("Menu")] = LayoutManager::UIParseLayoutElement;
}

// 用于编辑器中调整控件库的依赖
void  UIApplication::RestoreRegisterUIObject()
{
    ClearRegisterUIObject();
    RegisterDefaultUIObject();
}

HRESULT UIApplication::CreateInstanceByName(const TCHAR* bstrXmlName, IObject** pOut)
{
	if (NULL == bstrXmlName || NULL == pOut)
		return E_INVALIDARG;

	*pOut = NULL;

	funcUICreateInstancePtr  funcptr;
	if (false == this->GetUICreateInstanceFuncPtr(bstrXmlName, &funcptr))
		return E_FAIL;

	return funcptr(m_pUIApplication, (void**)pOut);
}

HRESULT UIApplication::CreateInstanceByClsid(REFCLSID clsid, void** pOut)
{
    if (NULL == pOut)
        return E_INVALIDARG;

    *pOut = NULL;

    funcUICreateInstancePtr  funcptr;
    if (false == this->GetUICreateInstanceFuncPtr(clsid, &funcptr))
    {
        LPOLESTR lpstr = NULL;
        StringFromIID(clsid, &lpstr);
        UI_LOG_ERROR(_T("%s GetUICreateInstanceFuncPtr Failed. clsid=%s"), lpstr);
        CoTaskMemFree(lpstr);

        return E_FAIL;
    }

    return funcptr(m_pUIApplication, (void**)pOut);
}

HRESULT  UIApplication::RegisterUIRenderBaseCreateData(const TCHAR* bstrName, int nType, int nControlType, int nControlSubType, funcUICreateRenderBasePtr pfunc)
{
    if (NULL == bstrName || NULL == pfunc)
        return E_INVALIDARG;

    String strName(bstrName);

    UIRENDERBASE_CREATE_INFO* pInfo = new UIRENDERBASE_CREATE_INFO;
    pInfo->m_func = pfunc;
    pInfo->m_nRenderType = nType;
    pInfo->m_strName = strName;
    pInfo->m_nControlType = nControlType;
    pInfo->m_nControlSubType = nControlSubType;
    m_vecUIRenderBaseCreateData.push_back(pInfo);

    UI_LOG_DEBUG(_T("%s  %s, type=%d, ctrl=%d, subctrl=%d  @ 0x%08X"), FUNC_NAME, bstrName, nType, nControlType, nControlSubType, pfunc);
    return S_OK;
}
HRESULT  UIApplication::CreateRenderBaseByName(const TCHAR* bstrName, IObject* pObject, IRenderBase** ppOut)
{
    if (NULL == bstrName || NULL == pObject || NULL == ppOut)
        return E_INVALIDARG;

    UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
    for ( ; iter != m_vecUIRenderBaseCreateData.end(); iter++ )
    {
        UIRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_strName != bstrName)
            continue;

        if (-1 != pData->m_nControlType)
        {
            if (pObject->GetObjectExtentType() != pData->m_nControlType)
                continue;
        }

        if (-1 != pData->m_nControlSubType)
        {
            int  nStylyEx = pObject->GetStyleEx();
            if (GETCONTROLSUBTYPE(nStylyEx) != pData->m_nControlSubType)
                continue;
        }

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetRenderType((RENDER_TYPE)pData->m_nRenderType);
        }
        
        return hr;
    }

    UI_LOG_WARN(_T("%s Create Failed. Name=%s"), FUNC_NAME, bstrName);
    return E_FAIL;
}
HRESULT  UIApplication::CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut)
{
    if (NULL == ppOut)
        return E_INVALIDARG;

    UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
    for ( ; iter != m_vecUIRenderBaseCreateData.end(); iter++ )
    {
        UIRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_nRenderType != nType)
            continue;

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetRenderType((RENDER_TYPE)nType);
        }
        
        return hr;
    }

    UI_LOG_WARN(_T("%s Create Failed. Type=%d"), FUNC_NAME, nType);
    return E_FAIL;
}

HRESULT  UIApplication::RegisterUITextRenderBaseCreateData(const TCHAR* bstrName, int nType, int nControlType, int nControlSubType, funcUICreateTextRenderBasePtr pfunc)
{
    if (NULL == bstrName || NULL == pfunc)
        return E_INVALIDARG;

    String strName(bstrName);

    UITEXTRENDERBASE_CREATE_INFO* pInfo = new UITEXTRENDERBASE_CREATE_INFO;
    pInfo->m_func = pfunc;
    pInfo->m_nRenderType = nType;
    pInfo->m_strName = strName;
    pInfo->m_nControlType = nControlType;
    pInfo->m_nControlSubType = nControlSubType;
    m_vecUITextRenderBaseCreateData.push_back(pInfo);

    UI_LOG_DEBUG(_T("%s  %s   @ 0x%08X"), FUNC_NAME, bstrName,  pfunc);
    return S_OK;
}
HRESULT  UIApplication::CreateTextRenderBaseByName(const TCHAR* bstrName, IObject* pObject, ITextRenderBase** ppOut)
{
    if (NULL == bstrName || NULL == pObject || NULL == ppOut)
        return E_INVALIDARG;

    UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
    for ( ; iter != m_vecUITextRenderBaseCreateData.end(); iter++ )
    {
        UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_strName != bstrName)
            continue;

        if (-1 != pData->m_nControlType)
        {
            if (pObject->GetObjectExtentType() != pData->m_nControlType)
                continue;
        }

        if (-1 != pData->m_nControlSubType)
        {
            int  nStylyEx = pObject->GetStyleEx();
            if (GETCONTROLSUBTYPE(nStylyEx) != pData->m_nControlSubType)
                continue;
        }

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetTextRenderType((TEXTRENDER_TYPE)pData->m_nRenderType);
        }

        return hr;
    }

    UI_LOG_WARN(_T("%s Create Failed. Name=%s"), FUNC_NAME, bstrName);
    return E_FAIL;
}
HRESULT  UIApplication::CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut)
{
    if (/*NULL == pObject ||*/ NULL == ppOut)
        return E_INVALIDARG;

    UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
    for ( ; iter != m_vecUITextRenderBaseCreateData.end(); iter++ )
    {
        UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_nRenderType != nType)
            continue;

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetTextRenderType((TEXTRENDER_TYPE)nType);
        }

        return hr;
    }

    UI_LOG_WARN(_T("%s Create Failed. Type=%d"), FUNC_NAME, nType);
    return E_FAIL;
}

HRESULT  UIApplication::RegisterLayoutCreateData(const TCHAR* szName, int nType, funcUICreateLayoutPtr pfun)
{
    if (NULL == pfun || NULL == szName)
    {
        UI_LOG_WARN(_T("%s invalid arg. name=%s, pfun=%x"), FUNC_NAME, szName, pfun);
        return E_INVALIDARG;
    }

    int nSize = (int)m_vecUILayoutCreateData.size();
    for (int i = 0; i < nSize; i++)
    {
        UILAYOUT_CREATE_INFO* pData = m_vecUILayoutCreateData[i];
        if (pData && (pData->m_strName == szName || pData->m_nLayoutType == nType))
        {
            UI_LOG_WARN(_T("%s register duplicate. name=%s, type=%d, pfun=%x"), FUNC_NAME, szName, nType, pfun);
            return E_FAIL;
        }
    }

    UILAYOUT_CREATE_INFO* pData = new UILAYOUT_CREATE_INFO;
    pData->m_strName = szName;
    pData->m_nLayoutType = nType;
    pData->m_func = pfun;

    m_vecUILayoutCreateData.push_back(pData);
    return S_OK;
}
HRESULT  UIApplication::CreateLayout(int nType, IObject* pObject, ILayout** ppOut)
{
    if (NULL == ppOut)
        return false;

    funcUICreateLayoutPtr  pFunc = NULL;
    UILAYOUT_CREATE_DATA::iterator iter = m_vecUILayoutCreateData.begin();
    for ( ; iter != m_vecUILayoutCreateData.end(); iter++ )
    {
        UILAYOUT_CREATE_INFO* pData = *iter;
        if (pData && pData->m_nLayoutType == nType)
        {
            pFunc = pData->m_func;
            break;
        }
    }

    if (NULL == pFunc)
        return false;

    pFunc(pObject, ppOut);
    return true;
}
HRESULT  UIApplication::CreateLayoutByName(const TCHAR* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut)
{
    if (NULL == ppOut)
        return false;

    funcUICreateLayoutPtr  pFunc = NULL;
    if (szName)  // 为NULL有可能是没有配置layout属性，默认创建一个canvas布局
    {
        UILAYOUT_CREATE_DATA::iterator iter = m_vecUILayoutCreateData.begin();
        for ( ; iter != m_vecUILayoutCreateData.end(); iter++ )
        {
            UILAYOUT_CREATE_INFO* pData = *iter;
            if (pData && pData->m_strName == szName)
            {
                pFunc = pData->m_func;
                break;
            }
        }
    }

    if (NULL == pFunc && bCreateDefault && m_vecUILayoutCreateData.size() > 0)
        pFunc = m_vecUILayoutCreateData[0]->m_func;

    if (NULL == pFunc)
        return false;

    pFunc(pObject, ppOut);
    return true;
}

BOOL UIApplication::IsDialogMessage(MSG* pMsg)
{
	if (NULL == pMsg)
		return FALSE;

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < (WM_MOUSEFIRST+1) || pMsg->message > WM_MOUSELAST))
		return FALSE;

//	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		// 获取这个消息的窗口所在的顶层窗口，因为导航是针对是整个顶层窗口进行的
        // 2013.6.20 屏蔽掉这些代码。存在子窗口是UI窗口，但顶层窗口是普通窗口的情况
 		HWND hWndTop = pMsg->hwnd;
// 		while (1)
// 		{
// 			if (NULL == hWndTop)
// 				return FALSE;
// 
// 			LONG lStyle = ::GetWindowLong(hWndTop, GWL_STYLE);
// 			if (lStyle & WS_CHILD)
// 			{
// 				hWndTop = ::GetParent(hWndTop);
// 			}
// 			else
// 			{
// 				break;
// 			}
// 		}

		// 判断这个窗口是否属性UI管理的一个顶层窗口
		IWindowBase* pIWindow = m_TopWindowMgr.GetWindowBase(hWndTop);
        if (NULL == pIWindow)
            return FALSE;

        WindowBase* pWindow = pIWindow->GetWindowBaseImpl();
        if (!pWindow)
            return FALSE;

        return pWindow->GetMouseMgr()->IsDialogMessage(pMsg);
	}

	return FALSE;
}

// http://dsdm.bokee.com/6033955.html  如何正确使用PeekMessage
// 这里要注意的是多重模态的问题，如何保证正常退出
HRESULT  UIApplication::MsgHandleLoop(bool* pbQuitLoopRef)
{
	DWORD    dwRet = 0;
    DWORD&   nCount = m_WaitForHandlesMgr.m_nHandleCount;
    HANDLE*& pHandles = m_WaitForHandlesMgr.m_pHandles;
	MSG      msg;

    // 会传递pbQuitLoopRef参数的，有可能是Modal类型的菜单，这种情况下需要更多的条件判断
    // 因此单独用一个分支来优化
    if (pbQuitLoopRef)
    {
        bool   bExit = false;
        bool&  bQuitRef = (pbQuitLoopRef==NULL ? bExit : *pbQuitLoopRef);
	    while (false == bQuitRef)
	    {
		    dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, FALSE, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
		    if (nCount > 0 && dwRet < nCount)
		    {
			    m_WaitForHandlesMgr.Do(pHandles[dwRet]);
		    }
		    else
		    {
			    while (::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE))
			    {
				    if (WM_QUIT == msg.message)  // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
					    return S_OK;

                    if (m_MsgFilterMgr.PreTranslateMessage(&msg))  // 有可能在PreTranslateMessage中修改了pbQuitLoopRef
                    {
                        PeekMessage(&msg, NULL, 0,0, PM_REMOVE);   // 该消息已被处理过了，移除
                        continue;
                    }

                    if (bQuitRef)     // 可能的场景是弹出模态菜单后，然后在主窗口的关闭按钮上点了一下，然后菜单的PopupMenuWindow::PreTranslateMessage就要求退出了
                        return S_OK;  // 如果这时还继续处理，就会导致主窗口被销毁，当前用于弹出菜单的堆栈对象也被销毁了

                    PeekMessage(&msg, NULL, 0,0, PM_REMOVE);
                    if (FALSE == this->IsDialogMessage(&msg))
                    {
                        ::TranslateMessage(&msg);                                      
                        ::DispatchMessage(&msg);
                    }

                    if (bQuitRef)
                        return S_OK;
			    }
		    }
	    }
    }
    else
    {
        while (1)
        {
            dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, FALSE, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
            if (nCount > 0 && dwRet < nCount)
            {
                m_WaitForHandlesMgr.Do(pHandles[dwRet]);
            }
            else
            {
                while (::PeekMessage(&msg, NULL, 0,0, PM_REMOVE))
                {
                    if (WM_QUIT == msg.message)  
                    {
                        ::PostMessage(NULL, WM_QUIT, 0, 0);  // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
                        return S_OK;
                    }

                    if (FALSE == m_MsgFilterMgr.PreTranslateMessage(&msg) && 
                        FALSE == this->IsDialogMessage(&msg)) 
                    {
                        ::TranslateMessage(&msg);                                      
                        ::DispatchMessage(&msg);
                    }
                }
            }
        }
    }
	return S_OK;
}

HRESULT  UIApplication::RedrawTopWindows()
{
	m_TopWindowMgr.UpdateAllWindow();
	return S_OK;
}

HRESULT  UIApplication::ShowToolTip(TOOLTIPITEM* pItem)
{
	bool bRet = m_ToolTipMgr.Show(pItem);
	return bRet?S_OK:E_FAIL;
}
HRESULT  UIApplication::HideToolTip()
{
	m_ToolTipMgr.Hide();
	return S_OK;
}

HWND  UIApplication::GetForwardPostMessageWnd()
{
	return m_WndForwardPostMsg.m_hWnd;
}

void UIApplication::LoadUIObjectListToToolBox()
{
    if (!m_pUIEditor)
        return;

    UIOBJ_CREATE_DATA::iterator iter = m_vecUICreateData.begin();
    for (; iter != m_vecUICreateData.end(); iter++)
    {
        m_pUIEditor->OnToolBox_AddObject((*iter)->m_strXmlName.c_str(), (*iter)->m_strCategory.c_str(), (*iter)->m_nObjType);
    }
}
