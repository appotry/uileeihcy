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
//     SAFE_DELETE(m_pRenderTarget);  // ��ΪResize��m_pRenderTarget���󶨵�HDCҲ�������ˣ����Ըɴ���OnDraw�����°��µ�HDC
// }                                  // 2013.4.23 �޸ģ��ı��С��,RenderChain��MEMDC�����٣�������Bitmap

void DirectRenderLayer::Commit(HDC hDstDC)
{
    if (hDstDC)
    {
        if (NULL == m_pRenderTarget)
        {
            m_pRenderTarget = UICreateRenderTarget(m_pWindow->GetHWND());
            m_pRenderTarget->BindHDC(hDstDC);
            m_pRenderTarget->BeginDraw(NULL, 0, false);

            // ע��EndDraw�ᵼ��hDstDC�ļ��á�ƫ������Ҳ�������������������⣡����UpdateObjectʱ���õļ�������ͱ��ƻ��ˣ����������������򶼱�Commit
            //     ���ԣ�����ÿ�ζ�ȥ����BeginDraw/EndDraw��. TODO: ��û�и��Ӻ����İ취 
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
