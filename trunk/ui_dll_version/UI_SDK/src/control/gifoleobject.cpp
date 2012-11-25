#include "stdafx.h"
#include "control\gifoleobject.h"
#include "control\oledataobject.h"

const GUID IID_IGifOleObject =
{ 0x2eae75f5, 0xd78f, 0x43ca, { 0x81, 0x1d, 0x8f, 0x8b, 0x1, 0xcc, 0xe0, 0x5b } };

GifOleObject::GifOleObject(CPojo_Gif* pMgr, Message* pNotifyObj)
{
	m_pGifMgr = pMgr;
	m_pGifRenderItem = NULL;
	m_pNotifyMsg = pNotifyObj;
}
GifOleObject::~GifOleObject()
{
	SAFE_RELEASE(m_pGifRenderItem);
}

HRESULT __stdcall GifOleObject::LoadGif(const TCHAR* szPath)
{
	if (NULL == m_pGifMgr || NULL == szPath)
		return E_INVALIDARG;

	ATTRMAP mapAttr;
	String  strID(szPath);
	mapAttr[XML_ID] = szPath;

	m_pGifMgr->LoadItem(mapAttr, szPath);
	GifImageBase* pGif = m_pGifMgr->GetGif(strID);
	if (NULL == pGif)
		return E_FAIL;

	Gif_Timer_Notify notify(m_pNotifyMsg, 1, (LPARAM)this);
	m_pGifRenderItem = pGif->AddRender(&notify);
	m_pGifRenderItem->Start();

	if (NULL == m_pGifRenderItem)
		return E_FAIL;

	return S_OK;
}
HRESULT __stdcall GifOleObject::Refresh()
{
	if (NULL != m_pViewAdviseSink)
	{
		m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT,-1);  
	}
	return S_OK;
}

HRESULT GifOleObject::OnDraw(HDC hDC, RECT* prc)
{
	if (NULL == m_pGifRenderItem)
		return E_FAIL;

	m_pGifRenderItem->OnPaint(hDC, prc->left, prc->top);
	return S_OK;
}
HRESULT GifOleObject::OnGetSize(SIZE* pSize)
{
	if (NULL == pSize)
		return E_INVALIDARG;

	if (NULL == m_pGifRenderItem)
		return E_FAIL;

	pSize->cx = m_pGifRenderItem->GetWidth();
	pSize->cy = m_pGifRenderItem->GetHeight();
	
	return S_OK;
}

HRESULT GifOleObject::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT* lplpdataobj)
{
	if (NULL == lplpdataobj)
		return E_INVALIDARG;

	OleDataObject* pDataobject = new OleDataObject;
	pDataobject->AddRef();

	FORMATETC  format = {0};
	format.cfFormat = CF_TEXT/*CF_BITMAP*/;
	format.dwAspect = DVASPECT_CONTENT;
	format.tymed = TYMED_HGLOBAL/*TYMED_GDI*/;

	GIF_Frame* pFirstFrame = m_pGifRenderItem->GetGifImage()->GetFrame(0);
	UIASSERT(NULL != pFirstFrame);
	HGLOBAL hGlobal = GlobalAlloc(0, 10);
	LPSTR lpstr = (LPSTR)::GlobalLock(hGlobal);
	strcpy(lpstr, "leeihcy");
	::GlobalUnlock(hGlobal);

	STGMEDIUM medium = {0};
	medium.tymed = TYMED_HGLOBAL/*TYMED_GDI*/;
	//medium.hBitmap = (HBITMAP)OleDuplicateData((HANDLE)(HBITMAP)pFirstFrame->image, CF_BITMAP, 0);;
	
	medium.hGlobal = hGlobal;
	pDataobject->SetData(&format, &medium, TRUE);
	
	*lplpdataobj = static_cast<IDataObject*>(pDataobject);
	return S_OK;
}