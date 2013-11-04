#include "stdafx.h"
#include "directrenderlayer.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

DirectRenderLayer::DirectRenderLayer()
{
    m_pIDirectRenderLayer = NULL;
    m_pRenderTarget = NULL;
}
DirectRenderLayer::~DirectRenderLayer()
{
    SAFE_DELETE(m_pRenderTarget);
}

// void  DirectRenderLayer::OnSize(UINT nType, int cx, int cy)
// {
//     SetMsgHandled(FALSE);
//     SAFE_DELETE(m_pRenderTarget);  // 因为Resize后，m_pRenderTarget所绑定的HDC也被销毁了，所以干脆在OnDraw中重新绑定新的HDC
// }                                  // 2013.4.23 修改：改变大小后,RenderChain的MEMDC不销毁，仅销毁Bitmap

void DirectRenderLayer::Commit(HDC hDstDC)
{
    if (hDstDC)
    {
        if (NULL == m_pRenderTarget)
        {
            m_pRenderTarget = UICreateRenderTarget(m_pWindow->GetHWND());
            m_pRenderTarget->BindHDC(hDstDC);
            m_pRenderTarget->BeginDraw(NULL, 0, false);

            // 注：EndDraw会导致hDstDC的剪裁、偏移属性也被清除，因此这里有问题！！在UpdateObject时设置的剪裁区域就被破坏了，导致整个窗口区域都被Commit
            //     索性，不再每次都去调用BeginDraw/EndDraw了. TODO: 有没有更加合理点的办法 
            // m_pRenderTarget->EndDraw();  
        }

        CRect rc;
        this->GetParentRect(&rc);

        if (m_pBkgndRender)
            m_pBkgndRender->DrawState(m_pRenderTarget, &rc, 0);
        if (m_pForegndRender)
            m_pForegndRender->DrawState(m_pRenderTarget, &rc, 0);

    }
}
