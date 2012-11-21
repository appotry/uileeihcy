#include "stdafx.h"
#include "control\richeditolemgr.h"
#include "control\gifoleobject.h"

#pragma region
RichEditOleObjectItem::RichEditOleObjectItem()
{
	m_pViewAdviseSink = NULL;
	m_pClientSite = NULL;
	m_dwRef = 0;

	m_pOleAdviseHolder = NULL;
//	m_pDataAdviseHolder = NULL;
}
RichEditOleObjectItem::~RichEditOleObjectItem()
{
	SAFE_RELEASE(m_pViewAdviseSink);
	SAFE_RELEASE(m_pClientSite);
	SAFE_RELEASE(m_pOleAdviseHolder);
//	SAFE_RELEASE(m_pDataAdviseHolder);
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::QueryInterface(REFIID riid, void** ppvObject)
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

ULONG STDMETHODCALLTYPE RichEditOleObjectItem::AddRef( void) 
{
	return ++m_dwRef;
}

ULONG STDMETHODCALLTYPE RichEditOleObjectItem::Release( void) 
{
	if (0 == --m_dwRef)
	{
		delete this;
		return 0;
	}
	return m_dwRef;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::SetClientSite(IOleClientSite *pClientSite)
{
	SAFE_RELEASE(m_pClientSite);
	m_pClientSite = pClientSite;
	if (NULL != m_pClientSite)
		m_pClientSite->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::GetClientSite(IOleClientSite **ppClientSite) 
{
	*ppClientSite = m_pClientSite;
	if (NULL != m_pClientSite)
		m_pClientSite->AddRef();

	return S_OK;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::GetExtent(DWORD dwDrawAspect, SIZEL *psizel) 
{
	psizel->cx = psizel->cy = 2000;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::Draw( 
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
	HBRUSH hBrush = CreateSolidBrush(rand());
	::FillRect(hdcDraw, (RECT*)lprcBounds, hBrush);
	DeleteObject(hBrush);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel) 
{
	return this->GetExtent(dwDrawAspect, lpsizel);
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection)
{
	HRESULT hr = S_OK;
	if (m_pOleAdviseHolder == NULL)
		hr = CreateOleAdviseHolder(&m_pOleAdviseHolder);
	if (SUCCEEDED(hr))
		hr = m_pOleAdviseHolder->Advise(pAdvSink, pdwConnection);
	return hr;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::Unadvise(DWORD dwConnection)
{
	HRESULT hRes = E_FAIL;
	if (m_pOleAdviseHolder != NULL)
		hRes = m_pOleAdviseHolder->Unadvise(dwConnection);
	return hRes;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink) 
{
	SAFE_RELEASE(m_pViewAdviseSink);
	m_pViewAdviseSink = pAdvSink;
	if (NULL != m_pViewAdviseSink)
		m_pViewAdviseSink->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
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
	m_pGifImageItemMgr = new GifImageItemMgr();
}
RichEditOleObjectManager::~RichEditOleObjectManager()
{
	OLEOITEMMAP::iterator iter = m_mapOleObject.begin();
	OLEOITEMMAP::iterator iterEnd = m_mapOleObject.end();
	for (; iter != iterEnd; iter++)
	{
		iter->second->Release();
	}
	m_mapOleObject.clear();
	SAFE_DELETE(m_pGifImageItemMgr);
}

bool RichEditOleObjectManager::AddOleItem(RichEditOleObjectItem* pItem)
{
	m_mapOleObject.insert(make_pair(m_dwIndex++, pItem));
	return true;
}

RichEditOleObjectItem*  RichEditOleObjectManager::GetOleItem(int dwUser)
{
	OLEOITEMMAP::iterator iter = m_mapOleObject.find(dwUser);
	if (iter == m_mapOleObject.end())
	{
		return NULL;
	}
	
	return iter->second;
}

#pragma endregion