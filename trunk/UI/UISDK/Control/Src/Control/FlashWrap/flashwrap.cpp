#include "stdafx.h"
#include "flashwrap.h"
#include <atlcomcli.h>
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"

// clsid = {CLSID_Shockwave Flash Object}
// {D27CDB6E-AE6D-11CF-96B8-444553540000}
static const GUID CLSID_Shockwave = 
{ 0xD27CDB6E, 0xAE6D, 0x11CF, { 0x96, 0xB8, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };


//////////////////////////////////////////////////////////////////////////

FlashEmbeddingSite::FlashEmbeddingSite(FlashWrap* p)
{
	m_lRef = 0;
	m_pFlashWrap = p;
    m_pIFlashWrap = p->GetIFlashWrap();

	m_rcInvalidate.SetRectEmpty();
	m_hInvalidateRgn = NULL;
	m_pListener = NULL;
}
FlashEmbeddingSite::~FlashEmbeddingSite()
{
	SAFE_DELETE_GDIOBJECT(m_hInvalidateRgn);
}
HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::QueryInterface(REFIID riid, void **ppvObject)
{
	if (NULL == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown, riid))
		*ppvObject = static_cast<IUnknown*>((IOleClientSite*)this);
	else if (::IsEqualIID(IID_IOleClientSite, riid))
		*ppvObject = static_cast<IOleClientSite*>(this);
	else if (::IsEqualIID(IID_IOleControlSite, riid))
		*ppvObject = static_cast<IOleControlSite*>(this);
	else if (::IsEqualIID(IID_IAdviseSink, riid))
		*ppvObject = static_cast<IAdviseSink*>(this);
	else if (::IsEqualIID(IID_IOleInPlaceSiteWindowless, riid))
		*ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
	else if (::IsEqualIID(IID_IOleInPlaceSiteEx, riid))
		*ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
	else if (::IsEqualIID(IID_IOleInPlaceSite, riid))
		*ppvObject = static_cast<IOleInPlaceSite*>(this);
	else if (::IsEqualIID(IID_IOleWindow, riid))
		*ppvObject = static_cast<IOleWindow*>(this);
	else
		return E_NOINTERFACE;
	
	AddRef();
	return S_OK;
}
ULONG STDMETHODCALLTYPE FlashEmbeddingSite::AddRef(void){return ++m_lRef;}
ULONG STDMETHODCALLTYPE FlashEmbeddingSite::Release(void){--m_lRef; if (0==m_lRef){delete this; return 0;} return m_lRef;}

// 不实现的话Flash不显示
HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::GetWindow(HWND *phwnd)
{
	if (NULL == phwnd || NULL == m_pFlashWrap)
		return E_FAIL;

	*phwnd = m_pIFlashWrap->GetHWND();
	return S_OK;
}


HRGN FlashEmbeddingSite::GetInvalidateRect(bool bClear)
{
// 	if (::IsRectEmpty(&m_rcInvalidate))
// 		return false;
// 
// 	::CopyRect(lprc, &m_rcInvalidate);
// 
// 	if (bClear)
// 		::SetRectEmpty(&m_rcInvalidate);
//
//	return true;
	return m_hInvalidateRgn;
}

HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::InvalidateRect(LPCRECT pRect, BOOL fErase)
{ 
    if (!m_pIFlashWrap->CanRedraw())
        return S_FALSE;
        
	if (m_pListener)
	{
		m_pListener->OnInvalidateRect(pRect, fErase);
		return S_OK;
	}

	if (NULL == m_pFlashWrap)
		return E_FAIL;

// 	::InvalidateRect(m_pFlashWrap->GetHWND(), pRect, fErase);
// 	return S_OK;

// 	if (NULL == pRect)
// 		UI_LOG_DEBUG(_T("null"));
// 	else
// 		UI_LOG_DEBUG(_T("%d, %d, %d, %d"), pRect->left, pRect->right, pRect->top, pRect->bottom);

//	this->m_pFlashWrap->UpdateObject();


	if (NULL == pRect)
	{
	}
	else
	{
		if (NULL == m_hInvalidateRgn)
			m_hInvalidateRgn = CreateRectRgn(0,0,0,0);

		HRGN hrgnTemp = CreateRectRgnIndirect(pRect);
		::CombineRgn(m_hInvalidateRgn, m_hInvalidateRgn, hrgnTemp, RGN_OR);
		SAFE_DELETE_GDIOBJECT(hrgnTemp);

		//m_rcInvalidate.UnionRect(m_rcInvalidate, pRect);
	}

	UIMSG msg;
	msg.message = UI_WM_REDRAWOBJECT;
	msg.pMsgTo = m_pIFlashWrap;
	UIPostMessage(m_pIFlashWrap->GetUIApplication(), &msg);
	return S_OK;
}

// 设置Flash的窗口位置
HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,  LPRECT lprcPosRect, LPRECT lprcClipRect,  LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	if (ppFrame)
		*ppFrame = NULL;
	if (ppDoc)
		*ppDoc = NULL;

	CRect rc;
	m_pIFlashWrap->GetParentRect(&rc);

	if (lprcPosRect)
	{
		CopyRect(lprcPosRect, &rc);
	}
	if (lprcClipRect)
	{
		CopyRect(lprcClipRect, &rc);
	}
	if (lpFrameInfo)
	{
		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = m_pIFlashWrap->GetHWND();
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
FlashWrap::FlashWrap()
{
    m_pIFlashWrap = NULL;

	m_pSite = NULL;
	m_pOleObject = NULL;
	m_pFlash = NULL;
	m_pViewObject = NULL;
	m_nFlashWidth = m_nFlashHeight = 0;
	m_eWMode = FLASH_WMODE_OPAQUE;
}
FlashWrap::~FlashWrap()
{
	SAFE_RELEASE(m_pSite);
	SAFE_RELEASE(m_pOleObject);
	SAFE_RELEASE(m_pFlash);
	SAFE_RELEASE(m_pViewObject);
}
HRESULT FlashWrap::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IFlashWrap, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

	// 由于此时还没有布局，不能做其它处理
	HRESULT hr = CoCreateInstance(CLSID_Shockwave, NULL, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&m_pOleObject);
	if (FAILED(hr))
	{
		UI_LOG_ERROR(_T("%s CoCreateInstance Flash Active Control Failed. Ret val=%x"), FUNC_NAME, hr);
		return hr;
	}

	return S_OK;
}

bool FlashWrap::CreateControl()
{
	HRESULT   hr   = S_OK;
	bool      bRet = false;

	do 
	{
		// 创建Flash控件
		if (NULL == m_pOleObject)
		{
			hr = CoCreateInstance(CLSID_Shockwave, NULL, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&m_pOleObject);
		}
		if (FAILED(hr) || NULL == m_pOleObject)
		{
			UI_LOG_ERROR(_T("%s CoCreateInstance Flash Active Control Failed. Ret val=%x"), FUNC_NAME, hr);
			break;
		}

		hr = m_pOleObject->QueryInterface(__uuidof(IShockwaveFlash), (void**)&m_pFlash);
		if (FAILED(hr) || NULL == m_pFlash)	
			break;

		hr = m_pOleObject->QueryInterface(__uuidof(IViewObjectEx), (void**)&m_pViewObject);
		if (FAILED(hr) || NULL == m_pViewObject)	
			break;

		// 创建站点对象
		m_pSite = new FlashEmbeddingSite(this);
		m_pSite->AddRef();

		IOleClientSite* pControlSite = static_cast<IOleClientSite*>(m_pSite);
		IAdviseSink*    pAdviseSink = static_cast<IAdviseSink*>(m_pSite);

		// 将站点对象与控件关联起来
		hr = m_pOleObject->SetClientSite(pControlSite);
		if (FAILED(hr))
			break;

		hr = m_pViewObject->SetAdvise(DVASPECT_CONTENT, 0, pAdviseSink);
		if (FAILED(hr))
			break;
		
		// 初始化控件属性
		// 注：只有在调用了InitNew之后，再修改WMode才有用
		CComQIPtr<IPersistStreamInit> spPSI = m_pOleObject;
		hr = spPSI->InitNew();
		UIASSERT(SUCCEEDED(hr));

 		this->SetWMode(m_eWMode);

		// 注：DoVerb OLEIVERB_INPLACEACTIVATE 不要放在put_Movie后面，否则flash会播放的很快。（为什么会这样?）
		// 此时会去请求IOleInPlaceSiteWindowless接口
		// 也会走到GetWindowContext接口中，貌似只在要该接口中指定Flash的位置即可，因此将DoVerb中的一些参数直接设置为NULL
		HRESULT hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pControlSite, 0, NULL, NULL);

		if (!m_strFlashUri.empty())
		{
			hr = m_pFlash->put_Movie((BSTR)m_strFlashUri.c_str());

			m_nFlashWidth  = FlashPropertyUtil::GetFlashWidth(m_pFlash);
			m_nFlashHeight = FlashPropertyUtil::GetFlashHeight(m_pFlash);
		}

		bRet = true;
	} while (0);

	if (false == bRet)
	{
		SAFE_RELEASE(m_pSite);
		SAFE_RELEASE(m_pOleObject);
		SAFE_RELEASE(m_pFlash);
		SAFE_RELEASE(m_pViewObject);
	}

	return true;
}

void FlashWrap::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = m_nFlashWidth;
    pSize->cy = m_nFlashHeight;
}

void FlashWrap::ResetAttribute()
{
	DO_PARENT_PROCESS(IFlashWrap, IControl);

	SAFE_RELEASE(m_pSite);
	SAFE_RELEASE(m_pOleObject);
	SAFE_RELEASE(m_pFlash);
	SAFE_RELEASE(m_pViewObject);

	m_eWMode = FLASH_WMODE_OPAQUE;
	m_strFlashUri.clear();
	m_nFlashWidth = m_nFlashHeight = 0;

	m_pIFlashWrap->SetTabstop(false);   // Label 不能有焦点
	m_pIFlashWrap->ModifyStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_ALL, 0, 0);  // 默认不接收鼠标消息
}

void FlashWrap::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
	DO_PARENT_PROCESS(IFlashWrap, IControl);

	const TCHAR* szText = pMapAttr->GetAttr(XML_FLASH_URI, true);
	if (szText)
	{
		if (Util::IsFullPath(szText))
		{
			m_strFlashUri = szText;
		}
		else
		{
			TCHAR szPath[MAX_PATH] = _T("");
			TCHAR szRealPath[MAX_PATH] = _T("");

			Util::GetAppPath_(szPath);
			Util::CalcFullPathByRelative(szPath, szText, szRealPath );
			m_strFlashUri.assign(szRealPath);
		}
	}

	szText = pMapAttr->GetAttr(XML_FLASH_WMODE, true);
	if (szText)
	{
		if (0 == _tcscmp(XML_FLASH_WMODE_WINDOW, szText))
			m_eWMode = FLASH_WMODE_WINDOW;
		else if (0 == _tcscmp(XML_FLASH_WMODE_OPAQUE, szText))
			m_eWMode = FLASH_WMODE_OPAQUE;
		else if (0 == _tcscmp(XML_FLASH_WMODE_TRANSPARENT, szText))
			m_eWMode = FLASH_WMODE_TRANSPARENT;
	}

	if (NULL == m_pSite)
		CreateControl();
}

void FlashWrap::OnPaint(IRenderTarget* pRenderTarget)
{
	if (NULL == m_pFlash)
		return;

	HDC hDC = pRenderTarget->GetBindHDC();  // Flash直接支持原始GDI句柄，并不需要通过Gdiplus来实现
	DrawFlash(hDC);
}
void FlashWrap::DrawFlash(HDC hDC)
{
	if (NULL == m_pViewObject)
		return;

	CRect rc;
	m_pIFlashWrap->GetClientRectAsWin32(&rc);

	HRESULT hr = m_pViewObject->Draw(
				DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC,
				reinterpret_cast<LPCRECTL>(&rc), NULL, NULL, 0);
}
void FlashWrap::OnRedrawObject()
{
	if (NULL == m_pSite->m_hInvalidateRgn)
		return;

    IRenderLayer*  pRenderLayer = m_pIFlashWrap->GetRenderLayer();
	HDC hDC = pRenderLayer->GetMemoryLayerDC();
    if (NULL == hDC)
        return;

	SaveDC(hDC);   // SetMetaRgn之前必须调用一次，配合restoredc还原metargn

	::SelectClipRgn(hDC, m_pSite->m_hInvalidateRgn);
	SAFE_DELETE_GDIOBJECT(m_pSite->m_hInvalidateRgn);

	// SetMetaRgn使用当前clip rgn做为meta rgn。
	// 因为RedrawObject内部刷新一个对象时会重新设置裁剪区域为object区域，
	// 导致rcInvalidate clip rgn被覆盖。这里借用metargn来完成双重裁剪区域的设置。
	// PS: GDIPLUS貌似不支持MetaRgn

	::SetMetaRgn(hDC);   

	m_pIFlashWrap->UpdateObject();   

	RestoreDC(hDC,-1);              
}
void FlashWrap::OnSize( UINT nType, int cx, int cy )
{
	if (NULL == m_pFlash)
		return;

    CRect rc;
    m_pIFlashWrap->GetParentRect(&rc);
	SetFlashPos(&rc);
}

void FlashWrap::SetFlashUri(BSTR bstr)
{
	if (m_pFlash && NULL != bstr)
	{
		String strUri = bstr;
		if (Util::IsFullPath(strUri.c_str()))
		{
			m_strFlashUri = strUri;
		}
		else
		{
			TCHAR szPath[MAX_PATH] = _T("");
			TCHAR szRealPath[MAX_PATH] = _T("");

			Util::GetAppPath_(szPath);
			Util::CalcFullPathByRelative(szPath, strUri.c_str(), szRealPath );
			m_strFlashUri.assign(szRealPath);
		}

		if (SUCCEEDED(m_pFlash->put_Movie((BSTR)m_strFlashUri.c_str())))
		{
			m_nFlashWidth  = FlashPropertyUtil::GetFlashWidth(m_pFlash);
			m_nFlashHeight = FlashPropertyUtil::GetFlashHeight(m_pFlash);
		}
		else
		{
			m_nFlashHeight = 0;
			m_nFlashWidth = 0;
		}
	}
}
bool FlashWrap::IsAvailable()
{
	if (m_nFlashWidth > 0 && m_nFlashHeight > 0)
		return true;

	return false;
}

HRESULT FlashWrap::SetFlashPos(LPRECT lprc)
{
	if (NULL == m_pFlash || NULL == lprc)
		return E_INVALIDARG;

	CComQIPtr<IOleInPlaceObject> p = m_pFlash;
	return p->SetObjectRects(lprc, lprc);
}
HRESULT FlashWrap::SetFlashSize(int cx, int cy)
{
	if (NULL == m_pOleObject)
		return E_INVALIDARG;

	SIZEL pxsize = { cx, cy };
	SIZEL hmsize = { 0 };
	AtlPixelToHiMetric(&pxsize, &hmsize);
	return m_pOleObject->SetExtent(DVASPECT_CONTENT, &hmsize);
}

// 默认为窗口模式:Window。Opaque是无窗口模式，但有背景色，Transparent是无窗口模式，但有背景色
// PS: Window模式在分层窗口上是不能正常刷新的。
void FlashWrap::SetWMode(FLASH_WMODE e)
{
	m_eWMode = e;
	if (NULL == m_pFlash)
		return;

	switch (m_eWMode)
	{
	case FLASH_WMODE_OPAQUE:
		m_pFlash->put_WMode(L"Opaque");
		break;

	case FLASH_WMODE_TRANSPARENT:
		m_pFlash->put_WMode(L"Transparent");
		break;

	case FLASH_WMODE_WINDOW:
		m_pFlash->put_WMode(L"Window");
		break;
	}	
}

void FlashWrap::SetFlashInvalidateListener(IFlashInvalidateListener* p)
{
	if (m_pSite)
	{
		m_pSite->SetFlashInvalidateListener(p);
	}
}
/*
CallFlashFunction XML 格式如下：

const cstInvoke='<invoke name="%s" returntype="xml"><arguments>%s</arguments></invoke>';
其中，具體的參數如下：
	a、字串以string表示
	b、數值以number表示
	c、Boolean類型比較特殊，直接傳遞自閉合的 <true/> 或 <false/>
	d、日期類型以date表示、
	e、未定義以undefined表示
	f、空值以自閉和的 <null/> 表示
舉例如下：傳遞字串和Boolean類型參數
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string>商家名稱</string><true/></arguments></invoke>')
如果傳遞字串參數，有可能會遇到XML保留關鍵字或字元，會導致XML解析失敗。
因此建議使用CDATA來保證字串出現XML的保留關鍵字或字元。
CDATA格式：<![CDATA[內容]]>
上訴例子可以改為：
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string><![CDATA[商家名稱]]></string><true/></arguments></invoke>')
*/
HRESULT FlashWrap::CallFlashFunction(const TCHAR* szFuncName, const TCHAR* szArgs, BSTR* pbstrRet)
{
	if (NULL == m_pFlash)
		return E_FAIL;

    long lVertion = 0;
    m_pFlash->FlashVersion(&lVertion);
    int nMajor = HIWORD(lVertion);
    if (nMajor < 9)  // 在这里至少要求9.0以上版本才让调用，具体值是多少没查到
        return E_FAIL;

	String  str = _T("<invoke name='");
	str.append(szFuncName);
	str.append(_T("' returntype='xml'><arguments>"));
	str.append(szArgs);
	str.append(_T("</arguments></invoke>"));

	if (NULL == pbstrRet)
	{
		CComBSTR bstr;
		return m_pFlash->CallFunction((BSTR)str.c_str(), &bstr);
	}
	return m_pFlash->CallFunction((BSTR)str.c_str(), pbstrRet);
}

// swf文件是不能暂停的
void  FlashWrap::Pause()
{
    if (m_pFlash)
        m_pFlash->StopPlay();
}
void  FlashWrap::Play()
{
    if (m_pFlash)
        m_pFlash->Play();
}
//////////////////////////////////////////////////////////////////////////


#if 0
	TGetProperty(nameOfTargetMovieClip, propertyIndex) 
			
	X Position X坐标                       0 Get,Set 
	Y Position Y坐标                       1 Get,Set 
	X Scale X方向缩放比率                  2 Get,Set 
	Y Scale Y方向缩放比率                  3 Get,Set 
	CurrentFrame movie_clip当前所在帧数    4 Get 
	TotalFrames movie_clip总帧数           5 Get 
	Alpha movie_clip的透明度               6 Get,Set 
	Visibility movie_clip是否可见          7 Get,Set 
	Width movie_clip的宽度                 8 Get 
	Height movie_clip的高度                9 Get 
	Rotation movie_clip的旋转度           10 Get,Set 
	Target movie_clip的路径               11 Get 
	Framesloaded movie_clip已载入的帧数   12 Get 
	Name movie_clip的实体名字             13 Get,Set 
	DropTarget movie_clip的拖拽           14 Get 
	Url 包含movie_clip的动画的url         15 Get 
	HighQuality 设置画面质量为高质量      16 Get,Set 
	FocusRect 显示按钮边框                17 Get,Set 
	SoundBufTime 预设声音缓冲时间         18 Get,Set 
#endif
int FlashPropertyUtil::GetFlashWidth(IShockwaveFlash* pFlash)
{
	if (NULL == pFlash)
		return 0;

	CComBSTR bstrWidth;
	HRESULT hr = pFlash->TGetProperty(L"_level0", 8, &bstrWidth);
    if (FAILED(hr))
        return 0;

	return _wtoi(bstrWidth);
}
int FlashPropertyUtil::GetFlashHeight(IShockwaveFlash* pFlash)
{
	if (NULL == pFlash)
		return 0;

	CComBSTR bstrHeight;
	HRESULT hr = pFlash->TGetProperty(L"_level0", 9, &bstrHeight);
    if (FAILED(hr))
        return 0;

	return _wtoi(bstrHeight);
}

