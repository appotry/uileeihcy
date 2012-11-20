#include "stdafx.h"
#include "control\gifoleobject.h"

GifOleObject::GifOleObject()
{
	m_pGif = NULL;
}
GifOleObject::~GifOleObject()
{
	SAFE_DELETE(m_pGif);
}

// HRESULT GifOleObject::CreateInstance(REFIID riid, void** ppv)
// {
// 	GifOleObject* p = new GifOleObject;
// 	HRESULT hr = p->QueryInterface(riid, ppv);
// 	if (FAILED(hr))
// 		delete p;
// 
// 	return hr;
// }

HRESULT __stdcall GifOleObject::LoadGif(const TCHAR* szPath)
{
	return S_FALSE;
}
HRESULT __stdcall GifOleObject::Refresh()
{
	if (NULL != m_pViewAdviseSink)
	{
		m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT,-1);  
	}
	return S_OK;
}