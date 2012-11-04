#include "stdafx.h"
#include "control\gifoleobject.h"

CGifOleObject::CGifOleObject()
{
	m_pViewAdviseSink = NULL;
	m_pClientSite = NULL;
	m_dwRef = 0;
	m_pGif = NULL;
}
CGifOleObject::~CGifOleObject()
{
	SAFE_RELEASE(m_pViewAdviseSink);
	SAFE_RELEASE(m_pClientSite);
	SAFE_DELETE(m_pGif);
}

HRESULT CGifOleObject::CreateInstance(REFIID riid, void** ppv)
{
	CGifOleObject* p = new CGifOleObject;
	HRESULT hr = p->QueryInterface(riid, ppv);
	if (FAILED(hr))
		delete p;

	return hr;
}

HRESULT STDMETHODCALLTYPE CGifOleObject::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
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
	else if (::IsEqualIID(riid, IID_IGifOleObject))
	{
		*ppvObject = static_cast<IGifOleObject*>(this);
	}
	else
	{
		return E_NOINTERFACE;
	}

	this->AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE CGifOleObject::AddRef( void) 
{
	return ++m_dwRef;
}

ULONG STDMETHODCALLTYPE CGifOleObject::Release( void) 
{
	if (0 == --m_dwRef)
	{
		delete this;
		return 0;
	}
	return m_dwRef;
}

HRESULT STDMETHODCALLTYPE CGifOleObject::SetClientSite(IOleClientSite *pClientSite)
{
	SAFE_RELEASE(m_pClientSite);
	m_pClientSite = pClientSite;
	if (NULL != m_pClientSite)
		m_pClientSite->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGifOleObject::GetClientSite(IOleClientSite **ppClientSite) 
{
	*ppClientSite = m_pClientSite;
	if (NULL != m_pClientSite)
		m_pClientSite->AddRef();

	return S_OK;
}
HRESULT STDMETHODCALLTYPE CGifOleObject::GetExtent(DWORD dwDrawAspect, SIZEL *psizel) 
{
	psizel->cx = psizel->cy = 2000;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGifOleObject::Draw( 
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

HRESULT STDMETHODCALLTYPE CGifOleObject::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel) 
{
	return this->GetExtent(dwDrawAspect, lpsizel);
}

HRESULT STDMETHODCALLTYPE CGifOleObject::SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink) 
{
	SAFE_RELEASE(m_pViewAdviseSink);
	m_pViewAdviseSink = pAdvSink;
	if (NULL != m_pViewAdviseSink)
		m_pViewAdviseSink->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGifOleObject::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	*ppAdvSink = m_pViewAdviseSink;
	if (NULL != m_pViewAdviseSink)
		m_pViewAdviseSink->AddRef();
	return S_OK;
}

HRESULT __stdcall CGifOleObject::LoadGif(const TCHAR* szPath)
{
	return S_FALSE;
}
HRESULT __stdcall CGifOleObject::Refresh()
{
	if (NULL != m_pViewAdviseSink)
	{
		m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT,-1);  
	}
	return S_OK;
}