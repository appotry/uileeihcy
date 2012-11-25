#include "stdafx.h"
#include "control\richeditolemgr.h"
#include "control\gifoleobject.h"

#pragma region
RichEditOleObjectItem_Com::RichEditOleObjectItem_Com()
{
	m_pOleObject = NULL;
}
RichEditOleObjectItem_Com::~RichEditOleObjectItem_Com()
{  
	SAFE_RELEASE(m_pOleObject);
}

HRESULT  RichEditOleObjectItem_Com::GetOleObject(IOleObject** ppOleObject, bool bAddRef)
{
	if (NULL == ppOleObject)
		return E_INVALIDARG;

	if (NULL == m_pOleObject)
		return E_FAIL;

	*ppOleObject = m_pOleObject;

	if (bAddRef)
		m_pOleObject->AddRef();

	return S_OK;
}
HRESULT  RichEditOleObjectItem_Com::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
	return E_NOTIMPL;
}


HRESULT  RichEditOleObjectItem_Com::Attach(CLSID  clsid)
{
	SAFE_RELEASE(m_pOleObject);
	return ::CoCreateInstance(clsid, NULL, CLSCTX_INPROC, IID_IOleObject, (void**)&m_pOleObject); 
}
#pragma endregion

#pragma region
RichEditOleObjectItem_Inner::RichEditOleObjectItem_Inner()
{
	m_pViewAdviseSink = NULL;
	m_pClientSite = NULL;
	m_dwRef = 0;

	m_pOleAdviseHolder = NULL;
//	m_pDataAdviseHolder = NULL;
}
RichEditOleObjectItem_Inner::~RichEditOleObjectItem_Inner()
{
	SAFE_RELEASE(m_pViewAdviseSink);
	SAFE_RELEASE(m_pClientSite);
	SAFE_RELEASE(m_pOleAdviseHolder);
//	SAFE_RELEASE(m_pDataAdviseHolder);
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::QueryInterface(REFIID riid, void** ppvObject)
{
	if (::IsEqualIID(riid, IID_IUnknown) || ::IsEqualIID(riid, IID_IOleObject))
	{
		*ppvObject = this;
	}
	else if (::IsEqualIID(riid, IID_IViewObject))
	{
		*ppvObject = static_cast<IViewObject*>(this);
	}
	else if (::IsEqualIID(riid, IID_IViewObject2))
	{
		*ppvObject = static_cast<IViewObject2*>(this);
	}
	else
	{
		return E_NOINTERFACE;
	}

	this->AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE RichEditOleObjectItem_Inner::AddRef( void) 
{
	return ++m_dwRef;
}

ULONG STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Release( void) 
{
	if (0 == --m_dwRef)
	{
		delete this;
		return 0;
	}
	return m_dwRef;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::SetClientSite(IOleClientSite *pClientSite)
{
	SAFE_RELEASE(m_pClientSite);
	m_pClientSite = pClientSite;
	if (NULL != m_pClientSite)
		m_pClientSite->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetClientSite(IOleClientSite **ppClientSite) 
{
	*ppClientSite = m_pClientSite;
	if (NULL != m_pClientSite)
		m_pClientSite->AddRef();

	return S_OK;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetExtent(DWORD dwDrawAspect, SIZEL *psizel) 
{
//	psizel->cx = psizel->cy = 2000;
	SIZE  size = {0,0};
	this->OnGetSize(&size);

	psizel->cx = Util::DXtoHimetricX(size.cx);
	psizel->cy = Util::DYtoHimetricY(size.cx);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Draw( 
												DWORD dwDrawAspect,
												LONG lindex,
												void *pvAspect,
												DVTARGETDEVICE *ptd,
												HDC hdcTargetDev,
												HDC hdcDraw,
												LPCRECTL lprcBounds,
												LPCRECTL lprcWBounds,
												BOOL ( STDMETHODCALLTYPE *pfnContinue )(ULONG_PTR dwContinue),
												ULONG_PTR dwContinue) 
{

	RECT  rc = {lprcBounds->left, lprcBounds->top, lprcBounds->right, lprcBounds->bottom};
	return this->OnDraw(hdcDraw, &rc);
// 	HBRUSH hBrush = CreateSolidBrush(rand());
// 	::FillRect(hdcDraw, (RECT*)lprcBounds, hBrush);
// 	DeleteObject(hBrush);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel) 
{
	return this->GetExtent(dwDrawAspect, lpsizel);
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection)
{
	HRESULT hr = S_OK;
	if (m_pOleAdviseHolder == NULL)
		hr = CreateOleAdviseHolder(&m_pOleAdviseHolder);
	if (SUCCEEDED(hr))
		hr = m_pOleAdviseHolder->Advise(pAdvSink, pdwConnection);
	return hr;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Unadvise(DWORD dwConnection)
{
	HRESULT hRes = E_FAIL;
	if (m_pOleAdviseHolder != NULL)
		hRes = m_pOleAdviseHolder->Unadvise(dwConnection);
	return hRes;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink) 
{
	SAFE_RELEASE(m_pViewAdviseSink);
	m_pViewAdviseSink = pAdvSink;
	if (NULL != m_pViewAdviseSink)
		m_pViewAdviseSink->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	*ppAdvSink = m_pViewAdviseSink;
	if (NULL != m_pViewAdviseSink)
		m_pViewAdviseSink->AddRef();
	return S_OK;
}
#pragma endregion

#pragma region
RichEditOleObjectManager::RichEditOleObjectManager(WindowlessRichEdit* pRichEdit)
{
	m_pRichEdit = pRichEdit;
	m_dwIndex = 0;
//	m_pGifImageItemMgr = new GifImageItemMgr();
}
RichEditOleObjectManager::~RichEditOleObjectManager()
{
	// еп╤ор╩об
	// OleFlushClipboard();

	OLEOITEMMAP::iterator iter = m_mapOleObject.begin();
	OLEOITEMMAP::iterator iterEnd = m_mapOleObject.end();
	for (; iter != iterEnd; iter++)
	{
		SAFE_DELETE(iter->second);
	}
	m_mapOleObject.clear();
//	SAFE_DELETE(m_pGifImageItemMgr);
}

bool RichEditOleObjectManager::AddOleItem(IRichEditOleObjectItem* pItem)
{
	m_mapOleObject.insert(make_pair(m_dwIndex++, pItem));
	return true;
}

IRichEditOleObjectItem*  RichEditOleObjectManager::GetOleItem(int dwUser)
{
	OLEOITEMMAP::iterator iter = m_mapOleObject.find(dwUser);
	if (iter == m_mapOleObject.end())
	{
		return NULL;
	}
	
	return iter->second;
}

#pragma endregion