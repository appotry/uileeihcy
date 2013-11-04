#include "stdafx.h"
#include "WindowAlphaShowAnimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\layeredanimatewindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"

namespace UI
{

WindowAlphaShowAnimate::WindowAlphaShowAnimate()
{
    m_pIWindowAlphaShowAnimate = NULL;
    m_bShow = true;
}

WindowAlphaShowAnimate::~WindowAlphaShowAnimate()
{
    if (m_bShow)
    {

    }
    else
    {
        ShowWindow(m_pLayeredWindow->m_hHostWnd, SW_HIDE);
    }
}

void  WindowAlphaShowAnimate::Initialize()
{
    m_pWindow->GetRenderChain()->SetCanCommit(false);
    ::ShowWindow(m_pWindow->m_hWnd, SW_SHOW);
    m_pWindow->PaintWindow(NULL);
    m_pWindow->GetRenderChain()->SetCanCommit(true);

    __super::Initialize();
}

void  WindowAlphaShowAnimate::OnTick(int nCount, IStoryboard** ppTimerArray)
{
    m_pLayeredWindow->Commit2LayeredWindow();
}

bool  WindowAlphaShowAnimate::Show(int nId, int nDuration)
{
    return DoAction(nId, nDuration, true);
}
bool  WindowAlphaShowAnimate::Hide(int nId, int nDuration)
{
    return DoAction(nId, nDuration, false);
}

// 注：
//    alpha渐变并不仅仅是修改每个像素的alpha值，还应该同时修改rgb的值，否则效果不正确
//    例如  r = r*p; g = g*p; b = b*p; a = a*p;
//
bool  WindowAlphaShowAnimate::DoAction(int nId, int nDuration, bool bShow)
{
    if (NULL == m_pAnimateMgr || NULL == m_pWindow)
        return false;

    IIntLinearMove* pMoveAlgo = NULL;
    IIntTimeline*  pTimeline = static_cast<IIntTimeline*>(
        CreateTimeline(TLV_INT, 0, ITMA_Linear, (IMoveAlgorithm**)&pMoveAlgo));
    if (NULL == pTimeline)
        return false;

    m_bShow = bShow;
    if (bShow)
        pMoveAlgo->SetParam1(0, 255, nDuration);
    else
        pMoveAlgo->SetParam1(255, 0, nDuration);

    pTimeline->SetOutRef(&m_pLayeredWindow->m_nSourceConstantAlpha);

    this->SetId(nId);
    this->SetNotifyObj(m_pWindow->GetIMessage());
    this->Begin();

    m_SrcImage.BitBlt(m_pLayeredWindow->m_hLayeredMemDC,0,0, m_pLayeredWindow->m_nWidth, m_pLayeredWindow->m_nHeight, 0 , 0);

    return true;
}

}