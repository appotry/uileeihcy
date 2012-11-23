#include "stdafx.h"
#include "control\gifoleobject.h"

const GUID IID_IGifOleObject =
{ 0x2eae75f5, 0xd78f, 0x43ca, { 0x81, 0x1d, 0x8f, 0x8b, 0x1, 0xcc, 0xe0, 0x5b } };

// #pragma region
// GifImageItem::GifImageItem()
// {
// 	m_szFileIDorName = NULL;
// 	m_eFileLoadType = GifImageLoadType_None;  
// 	m_pGif = NULL;
// 	m_dwRef = 0;
// }
// GifImageItem::~GifImageItem()
// {
// 	SAFE_ARRAY_DELETE(m_szFileIDorName);
// 	SAFE_DELETE(m_pGif);
// }
// 
// int GifImageItem::AddRef()
// {
// 	return ++m_dwRef;
// }
// int GifImageItem::Release()
// {
// 	m_dwRef--;
// 	if (0 == m_dwRef)
// 	{
// 		delete this;
// 		return 0;
// 	}
// 	return m_dwRef;
// }
// 
// bool GifImageItem::LoadGifByPath(const TCHAR* szFilePath)
// {
// 	SAFE_DELETE(m_pGif);
// 	m_pGif = new GifImage;
// 
// 	if (false == m_pGif->Load(szFilePath))
// 	{
// 		SAFE_DELETE(m_pGif);
// 		return false;
// 	}
// 
// 	Gif_Timer_Notify notify(NULL, 0,0);
// 	int nIndex = 0;
// 
// 	m_pGif->AddRender(&notify, &nIndex);
// 	m_pGif->Start();
// 	//	m_pGifItem->m_pGif->Pause();
// 
// 	m_eFileLoadType = GifImageLoadType_File;
// 	SAFE_ARRAY_DELETE(m_szFileIDorName);
// 	m_szFileIDorName = new TCHAR[_tcslen(szFilePath)+1];
// 	_tcscpy(m_szFileIDorName, szFilePath);
// 
// 	return true;
// }
// 
// bool  GifImageItemMgr::LoadGifByPath(const TCHAR* szFilePath, GifImageItem** ppGifImageItem)
// {
// 	if (NULL == ppGifImageItem)
// 		return false;
// 
// 	// 1. 查找该文件是否已经加载过
// 	GifImageItem* p = this->FindItem(GifImageLoadType_File, szFilePath);
// 	if (NULL != p)
// 	{
// 		p->AddRef();
// 		*ppGifImageItem = p;
// 		return true;
// 	}
// 	
// 	// 2. 加载文件
// 	GifImageItem* pItem = new GifImageItem;
// 	if (false == pItem->LoadGifByPath(szFilePath))
// 	{
// 		SAFE_DELETE(pItem);
// 		return false;
// 	}
// 	pItem->AddRef();
// 	*ppGifImageItem = pItem;
// 
// 	m_listGifImageFile.push_back(pItem);
// 	return true;
// }
// bool  GifImageItemMgr::LoadGifByID(const TCHAR* szImageID, GifImageItem** ppGifImageItem)
// {
// 	return false;
// }
// 
// GifImageItem*  GifImageItemMgr::FindItem(GifImageLoadType eType, const TCHAR* szFileIDorName)
// {
// 	list<GifImageItem*>::iterator  iter = m_listGifImageFile.begin();
// 	list<GifImageItem*>::iterator  iterEnd = m_listGifImageFile.end();
// 	for (; iter != iterEnd; iter++)
// 	{
// 		GifImageItem* pItem = *iter;
// 		if (NULL == pItem)
// 			continue;
// 		
// 		if (pItem->m_eFileLoadType == eType)
// 		{
// 			if (NULL == szFileIDorName || NULL == pItem->m_szFileIDorName)
// 				continue;
// 
// 			if (eType == GifImageLoadType_File)
// 			{
// 				if (0 == _tcsicmp(szFileIDorName, pItem->m_szFileIDorName))  // 文件路径不区分大小写 
// 				{
// 					return pItem;
// 				}
// 			}
// 			else
// 			{
// 				if (0 == _tcscmp(szFileIDorName, pItem->m_szFileIDorName))
// 				{
// 					return pItem;
// 				}
// 			}
// 		}
// 	}
// 
// 	return NULL;
// }
//#pragma endregion


GifOleObject::GifOleObject(CPojo_Gif* pMgr)
{
	m_pGifMgr = pMgr;
	m_pGifRenderItem = NULL;
}
GifOleObject::~GifOleObject()
{
	SAFE_RELEASE(m_pGifRenderItem);
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
	if (NULL == m_pGifMgr || NULL == szPath)
		return E_INVALIDARG;

	ATTRMAP mapAttr;
	String  strID(szPath);
	mapAttr[XML_ID] = szPath;

	m_pGifMgr->LoadItem(mapAttr, szPath);
	GifImageBase* pGif = m_pGifMgr->GetGif(strID);
	if (NULL == pGif)
		return E_FAIL;

	Gif_Timer_Notify notify(this, 1);
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

void GifOleObject::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
	if (NULL != m_pViewAdviseSink)
	{
		m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT, 0);
	}
}
