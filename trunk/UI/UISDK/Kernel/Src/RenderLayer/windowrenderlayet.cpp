#include "stdafx.h"
#include "windowrenderlayet.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"

namespace UI{

void  WindowRenderLayer::Draw()
{
    if (!CanRedraw())
        return;

    if (NULL == m_pRenderTarget)
        return;

    if (m_pRenderTarget->BeginDraw(NULL, 0))
    {
        if (m_pWindow->IsTransparent())
            m_pRenderTarget->Clear();

        CRect  rc;
        m_pWindow->GetParentRect(&rc);
        RenderContext roc(&rc, true);

        m_pWindow->DrawObject(m_pRenderTarget, roc);
        m_pRenderTarget->EndDraw();
    }
}

}