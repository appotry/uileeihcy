#include "stdafx.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "image.h"

namespace UI
{

IImage::IImage()
{
    m_pImageImpl = new Image;
}

IImage::~IImage()
{
    SAFE_DELETE(m_pImageImpl);
}

Image*  IImage::GetImpl() { return m_pImageImpl; }

bool  IImage::Load(const TCHAR*  szPath, bool bForceAlpha) 
{ 
	if (bForceAlpha)
		m_pImageImpl->ForceUseAlpha();

    if (FAILED(m_pImageImpl->Load(szPath)))
        return false;
    if (m_pImageImpl->IsNull())
        return false;

    return true;
}

bool  IImage::LoadFromResource( HINSTANCE hInstance, UINT nIDResource, TCHAR* szResourceType )
{
    m_pImageImpl->LoadFromResource(hInstance, nIDResource, szResourceType);
    if (m_pImageImpl->IsNull())
        return false;
    return true;
}
void  IImage::Attach(HBITMAP hBitmap, bool bHasAlphaChannel)
{ 
    m_pImageImpl->Attach(hBitmap);
    if (bHasAlphaChannel)
    {
        m_pImageImpl->ForceUseAlpha();
    }
}
HBITMAP  IImage::Detach()                { return m_pImageImpl->Detach(); }

BOOL  IImage::Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags) { return m_pImageImpl->Create(nWidth, nHeight, nBPP, dwFlags); }
void  IImage::Destroy()  { m_pImageImpl->Destroy(); }
bool  IImage::IsNull()   { return m_pImageImpl->IsNull(); }

int   IImage::GetWidth() { return m_pImageImpl->GetWidth(); }
int   IImage::GetHeight(){ return m_pImageImpl->GetHeight();}
int   IImage::GetPitch() { return m_pImageImpl->GetPitch(); }
byte* IImage::GetBits()  { return (byte*)m_pImageImpl->GetBits(); }
int   IImage::GetBPP()   { return m_pImageImpl->GetBPP(); }

HDC   IImage::BeginDrawToMyself()        { return m_pImageImpl->BeginDrawToMyself(); }
void  IImage::EndDrawToMyself()          { return m_pImageImpl->EndDrawToMyself(); }
BOOL  IImage::BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP)
{ 
    return m_pImageImpl->BitBlt(hDestDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, dwROP); 
}
void  IImage::Draw(HDC hDestDC, int x, int y)
{
    m_pImageImpl->Draw(hDestDC, x, y);
}
void  IImage::Draw(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight)
{
    m_pImageImpl->Draw(hDestDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight);
}

void  IImage::AlphaTopEdge(int nSpeed)
{
    m_pImageImpl->AlphaEdge(Image::ImageAlphaEdge_Top, nSpeed); 
}
void  IImage::AlphaBottomEdge(int nSpeed) 
{ 
    m_pImageImpl->AlphaEdge(Image::ImageAlphaEdge_Bottom, nSpeed); 
}
void  IImage::AlphaLeftEdge(int nSpeed)
{
    m_pImageImpl->AlphaEdge(Image::ImageAlphaEdge_Left, nSpeed); 
}
void  IImage::AlphaRightEdge(int nSpeed)
{
    m_pImageImpl->AlphaEdge(Image::ImageAlphaEdge_Right, nSpeed); 
}
void  IImage::SaveAsPng(const TCHAR* szPath) 
{ 
    if (NULL == szPath)
        return;

    m_pImageImpl->Save(szPath, Gdiplus::ImageFormatPNG); 
}
COLORREF  IImage::GetPixel(int x, int y) { return m_pImageImpl->GetPixel(x, y); }
DWORD  IImage::GetAverageColor() { return m_pImageImpl->GetAverageColor(); }
}