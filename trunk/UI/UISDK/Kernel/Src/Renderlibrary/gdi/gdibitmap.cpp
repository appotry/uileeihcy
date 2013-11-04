#include "stdafx.h"
#include "gdibitmap.h"

void GDIRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	GDIRenderBitmap* p = new GDIRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}

GDIImageListRenderBitmap::GDIImageListRenderBitmap()
{
	m_nCount = 0;
	m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
}
void GDIImageListRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	GDIImageListRenderBitmap* p = new GDIImageListRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}
void GDIImageListRenderBitmap::SetAttribute(IMapAttribute* pMapAttrib)
{
	__super::SetAttribute(pMapAttrib);

    pMapAttrib->GetAttr_int(XML_IMAGE_IMAGELIST_COUNT, false, &m_nCount);

    const TCHAR* szText = pMapAttrib->GetAttr(XML_IMAGE_IMAGELIST_LAYOUT, false);
	if (szText)
	{
		if (0 == _tcscmp(szText, XML_IMAGE_IMAGELIST_LAYOUT_H))
		{
			m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
		}
		else if (0 == _tcscmp(szText, XML_IMAGE_IMAGELIST_LAYOUT_V))
		{
			m_eLayout = IMAGELIST_LAYOUT_TYPE_V;
		}
	}
}
int GDIImageListRenderBitmap::GetItemWidth()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_V:
		return GetWidth();

	case IMAGELIST_LAYOUT_TYPE_H:
		return GetWidth()/m_nCount;
	}

	return 0;
}
int GDIImageListRenderBitmap::GetItemHeight()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_H:
		return GetHeight();

	case IMAGELIST_LAYOUT_TYPE_V:
		return GetHeight()/m_nCount;
	}

	return 0;
}
IMAGELIST_LAYOUT_TYPE GDIImageListRenderBitmap::GetLayoutType()
{
	return m_eLayout;
}
bool GDIImageListRenderBitmap::GetIndexPos(int nIndex, POINT* pPoint)
{
	if (NULL == pPoint)
		return false;

	pPoint->x = pPoint->y = 0;
	if (nIndex > m_nCount)
		return false;

	if (IMAGELIST_LAYOUT_TYPE_H == m_eLayout)
	{
		pPoint->x = nIndex*GetItemWidth();
		pPoint->y = 0;
	}
	else if (IMAGELIST_LAYOUT_TYPE_V == m_eLayout)
	{
		pPoint->x = 0;
		pPoint->y = nIndex*GetItemHeight();
	}
	else 
		return false;

	return true;
}
GDIIconRenderBitmap::GDIIconRenderBitmap()
{
	m_nIconWidth = m_nIconHeight = 16;
}
void GDIIconRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GDIIconRenderBitmap* p = new GDIIconRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}

void GDIIconRenderBitmap::SetAttribute(IMapAttribute* pMapAttrib)
{
	__super::SetAttribute(pMapAttrib);

    pMapAttrib->GetAttr_int(XML_IMAGE_ICON_WIDTH,  false, &m_nIconWidth);
    pMapAttrib->GetAttr_int(XML_IMAGE_ICON_HEIGHT, false, &m_nIconHeight);
}

//
// 注：1. 为了解决在16位色系统分辨率下面，系统不会加载32位的图标的问题，
//     在这里根据icon中mask bitmap来设置图片的分辨率。
//     2. 如果图标中不包含32位色图标项时，也可根据mask bitmap来生成带
//     alpha通道的位图
//     3. 32位色图标color bitmap自带alpha值，可直接通过DrawIconEx来得到32位位图
//
bool GDIIconRenderBitmap::LoadFromFile(const TCHAR* szPath, bool bCreateAlphaChannel)
{
	if (!m_image.IsNull())
		m_image.Destroy();

	HICON hIcon = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, m_nIconWidth, m_nIconHeight, LR_LOADFROMFILE);
	if (NULL == hIcon)
		return false;

	ICONINFO iconinfo = {0};
	::GetIconInfo(hIcon, &iconinfo);

	// 判断该图标是否是32位色的图标
	bool IsIcon32 = false;
	BITMAP bm = {0};
	bm.bmBitsPixel = 1; // 默认1位的位图
	if (iconinfo.hbmColor)
	{
		::GetObject(iconinfo.hbmColor, sizeof(BITMAP), &bm);
		if (bm.bmBitsPixel == 32)
		{
			IsIcon32 = true;
		}
	}

	m_image.Create(m_nIconHeight, m_nIconHeight, 32, Image::createAlphaChannel);
	if (IsIcon32)
	{
		HDC hDC = m_image.BeginDrawToMyself();
		BOOL bRet = ::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, NULL, DI_NORMAL);
		m_image.EndDrawToMyself();
	}
	else
	{
		HDC hDC = m_image.BeginDrawToMyself();
		BOOL bRet = ::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, NULL, DI_NORMAL);
		m_image.EndDrawToMyself();

		BITMAP bmMask = {0};
		GetObject(iconinfo.hbmMask, sizeof(BITMAP), &bmMask);

		int nSize = bmMask.bmWidthBytes*bmMask.bmHeight;
		BYTE* pMaskBitmapBits = new BYTE[nSize];
		::GetBitmapBits(iconinfo.hbmMask, nSize, (void**)pMaskBitmapBits);

		BYTE* pMaskBits = pMaskBitmapBits;
		BYTE* pDestBits = (BYTE*)m_image.GetBits();

		int nOffset = 0;
		for (int j = 0; j < bm.bmHeight; j++)   // 这里默认了mask bitmap是1位的黑白图片。
		{
			for (int i = 0; i < bm.bmWidth; i++)
			{
				BYTE maskvalue = *pMaskBits;
				maskvalue = ((maskvalue << nOffset)&0x80);
				pDestBits[i*4+3] = maskvalue? 0:255;

				nOffset++;
				if (nOffset >= 8)
				{
					nOffset = 0;
					pMaskBits++;
				}

			}

			pDestBits += m_image.GetPitch();
		}
		SAFE_ARRAY_DELETE(pMaskBitmapBits);
	}

	::DestroyIcon(hIcon);

	if (m_image.IsNull())
		return false;
	else
		return true;
}

bool  GDIIconRenderBitmap::LoadFromData(byte* pData, int nSize, bool bCreateAlphaChannel)
{
    if (!m_image.IsNull())
        m_image.Destroy();

    HICON hIcon = (HICON)::CreateIconFromResource(pData, nSize, TRUE, 0x00030000);
    if (NULL == hIcon)
        return false;

    ICONINFO iconinfo = {0};
    ::GetIconInfo(hIcon, &iconinfo);

    // 判断该图标是否是32位色的图标
    bool IsIcon32 = false;
    BITMAP bm = {0};
    bm.bmBitsPixel = 1; // 默认1位的位图
    if (iconinfo.hbmColor)
    {
        ::GetObject(iconinfo.hbmColor, sizeof(BITMAP), &bm);
        if (bm.bmBitsPixel == 32)
        {
            IsIcon32 = true;
        }
    }

    m_image.Create(m_nIconHeight, m_nIconHeight, 32, Image::createAlphaChannel);
    if (IsIcon32)
    {
        HDC hDC = m_image.BeginDrawToMyself();
        BOOL bRet = ::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, NULL, DI_NORMAL);
        m_image.EndDrawToMyself();
    }
    else
    {
        HDC hDC = m_image.BeginDrawToMyself();
        BOOL bRet = ::DrawIconEx(hDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, NULL, DI_NORMAL);
        m_image.EndDrawToMyself();

        BITMAP bmMask = {0};
        GetObject(iconinfo.hbmMask, sizeof(BITMAP), &bmMask);

        int nSize = bmMask.bmWidthBytes*bmMask.bmHeight;
        BYTE* pMaskBitmapBits = new BYTE[nSize];
        ::GetBitmapBits(iconinfo.hbmMask, nSize, (void**)pMaskBitmapBits);

        BYTE* pMaskBits = pMaskBitmapBits;
        BYTE* pDestBits = (BYTE*)m_image.GetBits();

        int nOffset = 0;
        for (int j = 0; j < bm.bmHeight; j++)   // 这里默认了mask bitmap是1位的黑白图片。
        {
            for (int i = 0; i < bm.bmWidth; i++)
            {
                BYTE maskvalue = *pMaskBits;
                maskvalue = ((maskvalue << nOffset)&0x80);
                pDestBits[i*4+3] = maskvalue? 0:255;

                nOffset++;
                if (nOffset >= 8)
                {
                    nOffset = 0;
                    pMaskBits++;
                }

            }

            pDestBits += m_image.GetPitch();
        }
        SAFE_ARRAY_DELETE(pMaskBitmapBits);
    }

    ::DestroyIcon(hIcon);

    if (m_image.IsNull())
        return false;
    else
        return true;
}
