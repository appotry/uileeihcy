#include "stdafx.h"
#include "UISDK\Kernel\Inc\Util\igifimage.h"
#include "UISDK\Kernel\Src\Util\Gif\GifImage.h"

IGifImage::IGifImage(GifImageBase* p)
{
    m_pImpl = p;
    m_bCreateGifImageImpl = false;
}
IGifImage::IGifImage(IUIApplication* pUIApp)
{
    CreateGifImpl(pUIApp);
}
IGifImage::~IGifImage()
{
    if (m_bCreateGifImageImpl)
    {
        SAFE_DELETE(m_pImpl);
    }
}

void  IGifImage::CreateGifImpl(IUIApplication* pUIApp)
{
    if (NULL == pUIApp)
        return;

    m_pImpl = new GifImage(pUIApp->GetForwardPostMessageWnd());
    m_bCreateGifImageImpl = true;
}
void  IGifImage::CreatePnglistGifImpl()
{

}
GifImageBase*  IGifImage::GetImpl()
{
    return m_pImpl;
}
void  IGifImage::SetImpl(GifImageBase* p)
{
    if (m_pImpl && m_bCreateGifImageImpl)
    {
        SAFE_DELETE(m_pImpl);
    }
    m_pImpl = p;
    m_bCreateGifImageImpl = false;

}

bool  IGifImage::Load(const TCHAR* szPath, IMapAttribute* pMapAttrib)
{
    return m_pImpl->Load(szPath, pMapAttrib);
}
bool  IGifImage::Destroy()
{
    return m_pImpl->Destroy();
}

IGifImageRender*  IGifImage::AddRender(Gif_Timer_Notify* pNotify, int* pIndex)
{
    return m_pImpl->AddRender(pNotify, pIndex);
}
bool  IGifImage::ModifyRender(Gif_Timer_Notify* pNotify, int nIndex)
{
    return m_pImpl->ModifyRender(pNotify, nIndex);
}
bool  IGifImage::DeleteRender(int nIndex)
{
    return m_pImpl->DeleteRender(nIndex);
}

bool  IGifImage::SetTransparentColor(COLORREF colTransparent)
{
    return m_pImpl->SetTransparentColor(colTransparent);
}