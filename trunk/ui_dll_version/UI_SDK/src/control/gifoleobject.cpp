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


#pragma region  // CF_DIB
#if 0
	{
		FORMATETC  format = {0};
		format.dwAspect = DVASPECT_CONTENT;
		format.cfFormat = CF_DIB;
		format.tymed = TYMED_HGLOBAL;

		// CF_DIB的结构为: BITMAPINFOHEADER + BITS

		GIF_Frame* pFirstFrame = m_pGifRenderItem->GetGifImage()->GetFrame(0);
		UIASSERT(NULL != pFirstFrame);

		DIBSECTION  dibsec;
		::GetObject(pFirstFrame->image, sizeof(DIBSECTION), &dibsec);

		LPBYTE  lpImageBits = (LPBYTE)pFirstFrame->image.GetBits();
		int     nPitch = pFirstFrame->image.GetPitch();
		int     nAbsPitch = abs(nPitch);
		int     nImgHeight = pFirstFrame->image.GetHeight();
		int     nImgWidth = pFirstFrame->image.GetWidth();

		HGLOBAL hGlobal = GlobalAlloc(0, sizeof(BITMAPINFOHEADER) + nImgHeight*nAbsPitch);
		LPBYTE  lpbits = (LPBYTE)::GlobalLock(hGlobal);
		
		LPBYTE  pSrc = lpImageBits;
		LPBYTE  pDst = lpbits;

		memcpy(pDst, &dibsec.dsBmih, sizeof(BITMAPINFOHEADER));
		pDst += sizeof(BITMAPINFOHEADER);

		if (nPitch < 0)
		{
			pDst += nAbsPitch*(nImgHeight-1);
		}

		for (int i = 0; i < nImgHeight; i++)
		{
#if 1       // 由于目前解析出来的gif透明位置的像素值为00 00 00 00
			// 导致其它绘图软件粘贴后显示为黑色。因此在这里做一次
			// 转换，将黑色换成白色
			for (int j = 0; j < nAbsPitch; j+=4)
			{
				int n = *(int*)(pSrc+j);
				if (n == 0)
					n = 0x00FFFFFF;

				*(int*)(pDst+j) = n;
			}
#else
			memcpy(pDst, pSrc, abs(nPitch));
#endif
			pDst += nPitch;
			pSrc += nPitch;
		}
		
		::GlobalUnlock(hGlobal);

		STGMEDIUM medium = {0};
		medium.tymed = TYMED_HGLOBAL;
		medium.hGlobal = hGlobal;
		
		pDataobject->SetData(&format, &medium, TRUE);
	}
#endif
#pragma endregion

#pragma region // CF_BITMAP
	{
		FORMATETC  format = {0};
		format.dwAspect = DVASPECT_CONTENT;
		format.cfFormat = CF_BITMAP;
		format.tymed = TYMED_GDI;

		GIF_Frame* pFirstFrame = m_pGifRenderItem->GetGifImage()->GetFrame(0);
		UIASSERT(NULL != pFirstFrame);

		STGMEDIUM medium = {0};
		medium.tymed = TYMED_GDI;
		medium.hBitmap = (HBITMAP)OleDuplicateData((HANDLE)(HBITMAP)pFirstFrame->image, CF_BITMAP, 0);;
		pDataobject->SetData(&format, &medium, TRUE);
	} 
#pragma endregion
	

#pragma region // CF_TEXT For Test
// 	{
// 		FORMATETC  format = {0};
// 		format.dwAspect = DVASPECT_CONTENT;
// 		format.cfFormat = CF_TEXT;
// 		format.tymed = TYMED_HGLOBAL;
// 
// 		char* pTextDesc = "ui_ole_object_gifimage";
// 		HGLOBAL hGlobal = GlobalAlloc(0, strlen(pTextDesc)+1);
// 		LPSTR lpstr = (LPSTR)::GlobalLock(hGlobal);
// 		strcpy(lpstr, pTextDesc);
// 		::GlobalUnlock(hGlobal);
// 
// 		STGMEDIUM medium = {0};
// 		medium.tymed = TYMED_HGLOBAL;
// 		medium.hGlobal = hGlobal;
// 		pDataobject->SetData(&format, &medium, TRUE);
// 	}
#pragma  endregion

	*lplpdataobj = static_cast<IDataObject*>(pDataobject);
	return S_OK;
}