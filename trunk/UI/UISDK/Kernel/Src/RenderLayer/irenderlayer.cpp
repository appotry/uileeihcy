#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "bufferrenderlayer.h"
#include "directrenderlayer.h"
#include "renderchain.h"
#include "windowrenderlayet.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IRenderLayer, RenderLayer, Panel);
UI_IMPLEMENT_Ixxx_INTERFACE(IBufferRenderLayer, BufferRenderLayer, RenderLayer);
UI_IMPLEMENT_Ixxx_INTERFACE(IControlRenderLayer, ControlRenderLayer, BufferRenderLayer);
UI_IMPLEMENT_Ixxx_INTERFACE(IWindowRenderLayer, WindowRenderLayer, BufferRenderLayer);
UI_IMPLEMENT_Ixxx_INTERFACE(IDirectRenderLayer, DirectRenderLayer, RenderLayer);

HDC  IRenderLayer::GetMemoryLayerDC()            { return m_pRenderLayerImpl->GetMemoryLayerDC(); }
IRenderTarget*  IRenderLayer::GetRenderTarget()  { return m_pRenderLayerImpl->GetRenderTarget(); }
void  IRenderLayer::SetLayerEnable(bool b)       { m_pRenderLayerImpl->SetLayerEnable(b); }
bool  IRenderLayer::IsLayerEnable()              { return m_pRenderLayerImpl->IsLayerEnable(); }

bool  IBufferRenderLayer::SaveLayer(const TCHAR*  szPath) { return m_pBufferRenderLayerImpl->SaveLayer(szPath); }
IRenderChain::IRenderChain()
{
    m_pImpl = new RenderChain(this);
}
IRenderChain::~IRenderChain()
{
    SAFE_DELETE(m_pImpl);
}

IRenderLayer*  IRenderChain::FindLayer(const TCHAR*  szLyerID) { return m_pImpl->FindLayer(szLyerID); }

RenderChain*  IRenderChain::GetImpl() { return m_pImpl; }
IRenderTarget*  IRenderChain::BeginRedrawObjectPart(IObject* pRedrawObj, RECT* prcArray, int nCount, bool* pbIsDrawing, RenderContext* pOutRenderContext)
{
    if (pRedrawObj)
        return m_pImpl->BeginRedrawObjectPart(pRedrawObj->GetImpl(), prcArray, nCount, pbIsDrawing, pOutRenderContext); 

    return NULL;
}
void  IRenderChain::EndRedrawObjectPart(IRenderTarget* pRenderTarget, RECT* prcArray, int nCount) { m_pImpl->EndRedrawObjectPart(pRenderTarget, prcArray, nCount); }
}

void  IRenderChain::SetCanCommit(bool b) { m_pImpl->SetCanCommit(b); }
bool  IRenderChain::CanCommit() { return m_pImpl->CanCommit(); }