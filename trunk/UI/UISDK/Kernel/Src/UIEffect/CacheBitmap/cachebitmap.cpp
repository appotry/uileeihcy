#include "stdafx.h"
#include "cachebitmap.h"
#include "UISDK\Kernel\Src\Atl\image.h"
namespace UI
{

CacheBitmap::CacheBitmap()
{
    m_hCacheBitmap = NULL;
    m_nWidth = m_nHeight = 0;
}
CacheBitmap::~CacheBitmap()
{
    Destroy();
}

void  CacheBitmap::Destroy()
{
    ::DeleteObject(m_hCacheBitmap);
    m_hCacheBitmap = NULL;
    m_nWidth = m_nHeight = 0;
}

HBITMAP  CacheBitmap::Create(int nWidth, int nHeight)
{
    if (m_nHeight < nHeight || m_nWidth < nWidth)
    {
        Destroy();
    }

    if (NULL == m_hCacheBitmap)
    {
        Image  image;
        image.Create(nWidth, -nHeight, 32, Image::createAlphaChannel);  // 负数表示创建一个top-down DIB,这样坐标与位图能够直接对应，便于计算
        m_hCacheBitmap = image.Detach();
        m_nWidth = nWidth;
        m_nHeight = nHeight;

        return m_hCacheBitmap;
    }
    else
    {
        HDC hDC = Image::GetCacheDC();
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, m_hCacheBitmap);
        RECT rc = {0, 0, nWidth, nHeight};
        ::FillRect(hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
        ::SelectObject(hDC, hOldBitmap);
        Image::ReleaseCacheDC(hDC);
    }

    return m_hCacheBitmap;
}

void  CacheBitmap::Clear(DWORD dwColor, RECT* prc)
{
    if (NULL == m_hCacheBitmap || NULL == prc)
        return;

    RECT rcDst;
    RECT rcTemp = { 0, 0, m_nWidth, m_nHeight };
    if (!IntersectRect(&rcDst, &rcTemp, prc))
        return;

    BITMAP  bm;
    ::GetObject(m_hCacheBitmap, sizeof(bm), &bm);
    byte* pBits = (byte*)bm.bmBits;

    pBits = (pBits + (rcDst.top * bm.bmWidthBytes + prc->left*4));
    int nSize = (rcDst.right-rcDst.left)*4;
    for (int y = rcDst.top; y < rcDst.bottom; y++)
    {
        int* p = (int*)pBits;
        for (int x = rcDst.left; x < rcDst.right; x++, p++)
        {
            *p = dwColor;
        }
        pBits += bm.bmWidthBytes;
    }
}
// 
// void  CacheBitmap::Transparent(DWORD dwTransparent, RECT* prc)
// {
//     if (NULL == m_hCacheBitmap || NULL == prc)
//         return;
// 
//     RECT rcDst;
//     RECT rcTemp = { 0, 0, m_nWidth, m_nHeight };
//     if (!IntersectRect(&rcDst, &rcTemp, prc))
//         return;
// 
//     BITMAP  bm;
//     ::GetObject(m_hCacheBitmap, sizeof(bm), &bm);
//     byte* pBits = (byte*)bm.bmBits;
// 
//     pBits = (pBits + (rcDst.top * bm.bmWidthBytes + prc->left*4));
//     int nSize = (rcDst.right-rcDst.left)*4;
//     for (int y = rcDst.top; y < rcDst.bottom; y++)
//     {
//         int* p = (int*)pBits;
//         for (int x = rcDst.left; x < rcDst.right; x++, p++)
//         {
//             if (*p == dwTransparent)
//                 *p = 0;
//         }
//         pBits += bm.bmWidthBytes;
//     }
// }

}