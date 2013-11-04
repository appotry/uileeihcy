#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Kernel\Src\Renderbase\imagerender\imagerender.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IImageRender, ImageRender, RenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IImageListItemRender, ImageListItemRender, ImageRender)
UI_IMPLEMENT_Ixxx_INTERFACE(IImageListRender, ImageListRender, RenderBase)

void  IImageRender::SetAlpha(int nAlpha) { m_pImageRenderImpl->SetAlpha(nAlpha); }
int   IImageRender::GetAlpha() { return m_pImageRenderImpl->GetAlpha(); }
void  IImageRender::SetColor(Color c) { m_pImageRenderImpl->SetColor(c); }
Color IImageRender::GetColor() { return m_pImageRenderImpl->GetColor(); }
void  IImageRender::SetImageDrawType(int n) { m_pImageRenderImpl->SetImageDrawType(n); }
int   IImageRender::GetImageDrawType() { return m_pImageRenderImpl->GetImageDrawType(); }
BKCOLOR_FILL_TYPE  IImageRender::GetBkColorFillType() { return m_pImageRenderImpl->GetBkColorFillType(); }
void  IImageRender::SetBkColorFillType(BKCOLOR_FILL_TYPE eType) { m_pImageRenderImpl->SetBkColorFillType(eType); }
IRenderBitmap*  IImageRender::GetRenderBitmap() { return m_pImageRenderImpl->GetRenderBitmap(); }
void  IImageRender::SetRenderBitmap(IRenderBitmap* pBitmap) { m_pImageRenderImpl->SetRenderBitmap(pBitmap); }

void  IImageListRender::SetIImageListRenderBitmap(IImageListRenderBitmap* pBitmap) { m_pImageListRenderImpl->SetIImageListRenderBitmap(pBitmap); }
IRenderBitmap*  IImageListRender::GetRenderBitmap()  { return m_pImageListRenderImpl->GetRenderBitmap(); }
void  IImageListRender::SetImageDrawType(int n) { m_pImageListRenderImpl->SetImageDrawType(n); }
int   IImageListRender::GetImageDrawType() { return m_pImageListRenderImpl->GetImageDrawType(); }
void  IImageListRender::SetImageStretch9Region(Image9Region* p) { return m_pImageListRenderImpl->SetImageStretch9Region(p); }
int   IImageListRender::GetItemWidth() { return m_pImageListRenderImpl->GetItemWidth(); }
int   IImageListRender::GetItemHeight() { return m_pImageListRenderImpl->GetItemHeight(); }
int   IImageListRender::GetItemCount() { return m_pImageListRenderImpl->GetItemCount(); }


}