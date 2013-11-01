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

// ��ʵ�ֵĻ�Flash����ʾ
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

// ����Flash�Ĵ���λ��
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

	// ���ڴ�ʱ��û�в��֣���������������
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
		// ����Flash�ؼ�
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

		// ����վ�����
		m_pSite = new FlashEmbeddingSite(this);
		m_pSite->AddRef();

		IOleClientSite* pControlSite = static_cast<IOleClientSite*>(m_pSite);
		IAdviseSink*    pAdviseSink = static_cast<IAdviseSink*>(m_pSite);

		// ��վ�������ؼ���������
		hr = m_pOleObject->SetClientSite(pControlSite);
		if (FAILED(hr))
			break;

		hr = m_pViewObject->SetAdvise(DVASPECT_CONTENT, 0, pAdviseSink);
		if (FAILED(hr))
			break;
		
		// ��ʼ���ؼ�����
		// ע��ֻ���ڵ�����InitNew֮�����޸�WMode������
		CComQIPtr<IPersistStreamInit> spPSI = m_pOleObject;
		hr = spPSI->InitNew();
		UIASSERT(SUCCEEDED(hr));

 		this->SetWMode(m_eWMode);

		// ע��DoVerb OLEIVERB_INPLACEACTIVATE ��Ҫ����put_Movie���棬����flash�Ქ�ŵĺܿ졣��Ϊʲô������?��
		// ��ʱ��ȥ����IOleInPlaceSiteWindowless�ӿ�
		// Ҳ���ߵ�GetWindowContext�ӿ��У�ò��ֻ��Ҫ�ýӿ���ָ��Flash��λ�ü��ɣ���˽�DoVerb�е�һЩ����ֱ������ΪNULL
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

	m_pIFlashWrap->SetTabstop(false);   // Label �����н���
	m_pIFlashWrap->ModifyStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_ALL, 0, 0);  // Ĭ�ϲ����������Ϣ
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

	HDC hDC = pRenderTarget->GetBindHDC();  // Flashֱ��֧��ԭʼGDI�����������Ҫͨ��Gdiplus��ʵ��
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

	SaveDC(hDC);   // SetMetaRgn֮ǰ�������һ�Σ����restoredc��ԭmetargn

	::SelectClipRgn(hDC, m_pSite->m_hInvalidateRgn);
	SAFE_DELETE_GDIOBJECT(m_pSite->m_hInvalidateRgn);

	// SetMetaRgnʹ�õ�ǰclip rgn��Ϊmeta rgn��
	// ��ΪRedrawObject�ڲ�ˢ��һ������ʱ���������òü�����Ϊobject����
	// ����rcInvalidate clip rgn�����ǡ��������metargn�����˫�زü���������á�
	// PS: GDIPLUSò�Ʋ�֧��MetaRgn

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

// Ĭ��Ϊ����ģʽ:Window��Opaque���޴���ģʽ�����б���ɫ��Transparent���޴���ģʽ�����б���ɫ
// PS: Windowģʽ�ڷֲ㴰�����ǲ�������ˢ�µġ�
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
CallFlashFunction XML ��ʽ���£�

const cstInvoke='<invoke name="%s" returntype="xml"><arguments>%s</arguments></invoke>';
���У����w�ą������£�
	a���ִ���string��ʾ
	b����ֵ��number��ʾ
	c��Boolean��ͱ��^���⣬ֱ�ӂ��f���]�ϵ� <true/> �� <false/>
	d�����������date��ʾ��
	e��δ���x��undefined��ʾ
	f����ֵ�����]�͵� <null/> ��ʾ
�e�����£����f�ִ���Boolean��ͅ���
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string>�̼����Q</string><true/></arguments></invoke>')
������f�ִ��������п��ܕ�����XML�����P�I�ֻ���Ԫ��������XML����ʧ����
��˽��hʹ��CDATA���C�ִ����FXML�ı����P�I�ֻ���Ԫ��
CDATA��ʽ��<![CDATA[����]]>
���V���ӿ��ԸĞ飺
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string><![CDATA[�̼����Q]]></string><true/></arguments></invoke>')
*/
HRESULT FlashWrap::CallFlashFunction(const TCHAR* szFuncName, const TCHAR* szArgs, BSTR* pbstrRet)
{
	if (NULL == m_pFlash)
		return E_FAIL;

    long lVertion = 0;
    m_pFlash->FlashVersion(&lVertion);
    int nMajor = HIWORD(lVertion);
    if (nMajor < 9)  // ����������Ҫ��9.0���ϰ汾���õ��ã�����ֵ�Ƕ���û�鵽
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

// swf�ļ��ǲ�����ͣ��
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
			
	X Position X����                       0 Get,Set 
	Y Position Y����                       1 Get,Set 
	X Scale X�������ű���                  2 Get,Set 
	Y Scale Y�������ű���                  3 Get,Set 
	CurrentFrame movie_clip��ǰ����֡��    4 Get 
	TotalFrames movie_clip��֡��           5 Get 
	Alpha movie_clip��͸����               6 Get,Set 
	Visibility movie_clip�Ƿ�ɼ�          7 Get,Set 
	Width movie_clip�Ŀ��                 8 Get 
	Height movie_clip�ĸ߶�                9 Get 
	Rotation movie_clip����ת��           10 Get,Set 
	Target movie_clip��·��               11 Get 
	Framesloaded movie_clip�������֡��   12 Get 
	Name movie_clip��ʵ������             13 Get,Set 
	DropTarget movie_clip����ק           14 Get 
	Url ����movie_clip�Ķ�����url         15 Get 
	HighQuality ���û�������Ϊ������      16 Get,Set 
	FocusRect ��ʾ��ť�߿�                17 Get,Set 
	SoundBufTime Ԥ����������ʱ��         18 Get,Set 
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

