#include "stdafx.h"
#include "multilabel.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

MultiLabel::MultiLabel()
{
    m_pIMultiLabel = NULL;
    m_nCurShowIndex = -1;
    m_nTextAlpha = 255;
    m_pColor = NULL;
    m_pRenderFont = NULL;
    m_nSwitchTimerId = 0;
}

MultiLabel::~MultiLabel()
{
    SAFE_RELEASE(m_pColor);
    SAFE_RELEASE(m_pRenderFont);
}
void  MultiLabel::SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload)
{
    DO_PARENT_PROCESS(IMultiLabel, UI::IControl);

    UI::IColorRes* pColorRes = m_pIMultiLabel->GetUIApplication()->GetActiveSkinColorRes();
    UI::IFontRes*  pFontRes = m_pIMultiLabel->GetUIApplication()->GetActiveSkinFontRes();

    // 颜色
    const TCHAR* szText = pMapAttrib->GetAttr(XML_TEXTRENDER_COLOR, true);
    if (szText)
        pColorRes->GetColor((BSTR)szText, &m_pColor);

    szText = pMapAttrib->GetAttr(XML_TEXTRENDER_FONT, true);
    if (szText)
        pFontRes->GetFont((BSTR)szText, this->OnGetGraphicsRenderType(), &m_pRenderFont);
}

void  MultiLabel::ResetAttribute()
{
    DO_PARENT_PROCESS(IMultiLabel, UI::IControl);
    SAFE_RELEASE(m_pColor);
    SAFE_RELEASE(m_pRenderFont);

    m_pIMultiLabel->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, 0);
}

void  MultiLabel::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    const TCHAR*  szText = GetCurShowText();
    if (NULL == szText)
        return;
    if (NULL == m_pRenderFont || NULL == m_pColor)
        return;

    CRect  rc;
    m_pIMultiLabel->GetClientRectAsWin32(&rc);

    UI::DRAWTEXTPARAM  param;
    
    UI::Color color;
    if (m_pColor)
        param.color = *m_pColor;
    param.color.a = m_nTextAlpha;
    param.nFormatFlag = DT_LEFT|DT_SINGLELINE|DT_VCENTER;
    param.prc = &rc;
    param.szText = szText;
    pRenderTarget->DrawString(m_pRenderFont, &param);
}


void MultiLabel::OnLButtonUp(UINT nFlags, POINT point)
{
    UI::IObject* pParent = m_pIMultiLabel->GetParentObject();
    if (NULL == pParent)
        return;

    POINT ptObj = {0,0};
    pParent->WindowPoint2ObjectClientPoint(&point, &ptObj);

    // UI_WM_HITTEST是判断与自己的m_rcParent的交集，因此要将窗口坐标转换成parent的client坐标
    UINT nHitTest = UISendMessage(m_pIMultiLabel, UI_WM_HITTEST, ptObj.x, ptObj.y);
    if (HTNOWHERE != nHitTest)
    {
        this->OnClicked(&point);   // 备注：由于DoNotify可能导致当前press hover对象发生改变，使得本控件丢失刷新
    }
}


void MultiLabel::OnClicked(POINT* pt)
{
    UI::IAnimateManager*  pAnimateMgr = m_pIMultiLabel->GetUIApplication()->GetAnimateMgr();
    pAnimateMgr->ClearStoryboardOfNotify(m_pIMultiLabel);
    m_nTextAlpha = 255;

    // 通知消息
    UI::UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_BN_CLICKED;
    msg.wParam  = NULL; //(WPARAM)pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = this->m_pIMultiLabel;

    msg.pMsgTo = NULL;
    m_pIMultiLabel->DoNotify(&msg);

    if (!msg.bHandled)
    {
        ShowNextLabelNow();
    }
}


const TCHAR*  MultiLabel::GetCurShowText()
{
    if (-1 == m_nCurShowIndex)
        return NULL;

    int nSize = (int)m_vecContent.size();
    if (0 == nSize)
        return NULL;

    return m_vecContent[m_nCurShowIndex].c_str();
}
const TCHAR*  MultiLabel::GetNextShowText()
{
    if (-1 == m_nCurShowIndex)
        return NULL;

    int nSize = (int)m_vecContent.size();
    if (0 == nSize)
        return NULL;

    m_nCurShowIndex ++;
    if (m_nCurShowIndex >= nSize)
        m_nCurShowIndex = 0;

    return m_vecContent[m_nCurShowIndex].c_str();
}
const TCHAR*  MultiLabel::GetPrevShowText()
{
    if (-1 == m_nCurShowIndex)
        return NULL;

    int nSize = (int)m_vecContent.size();
    if (0 == nSize)
        return NULL;

    m_nCurShowIndex --;
    if (m_nCurShowIndex < 0)
        m_nCurShowIndex = nSize - 1;

    return m_vecContent[m_nCurShowIndex].c_str();
}

void  MultiLabel::SetContent(const TCHAR* szContent, const TCHAR szSeparator)
{
    if (NULL == szContent)
    {
        m_vecContent.clear();
        m_nCurShowIndex = -1;
        return;
    }

    UI::Util::ISplitStringEnum*  pEnum = NULL;
    int nCount = UI::Util::SplitString(szContent, szSeparator, &pEnum);
    if (0 == nCount || NULL == pEnum)
        return;

    m_vecContent.clear();
    m_nCurShowIndex = 0;
    for (int i = 0; i < nCount; i++)
    {
        m_vecContent.push_back(pEnum->GetText(i));
    }
    SAFE_RELEASE(pEnum);
}

void  MultiLabel::ShowNextLabelNow()
{
    if (GetNextShowText())
    {
        m_pIMultiLabel->UpdateObject();
    }
}
void  MultiLabel::ShowPrevLabelNow()
{
    if (GetPrevShowText())
    {
        m_pIMultiLabel->UpdateObject();
    }
}

void  MultiLabel::StartAnimateSwitch(int nTime)
{
    if (0 == m_nSwitchTimerId)
    {
        m_nSwitchTimerId = m_pIMultiLabel->GetUIApplication()->SetTimer(nTime, m_pIMultiLabel);
    }
}
void  MultiLabel::StopAnimateSwitch()
{
    if (0 != m_nSwitchTimerId)
    {
        m_pIMultiLabel->GetUIApplication()->KillTimer(m_nSwitchTimerId);
    }
}

void  MultiLabel::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    if (nIDEvent == m_nSwitchTimerId)
    {
//      this->ShowNextLabelNow();

        UI::IAnimateManager*  pAnimateMgr = m_pIMultiLabel->GetUIApplication()->GetAnimateMgr();

        UI::IStoryboard*  pStoryboard = pAnimateMgr->CreateStoryboard();
        pStoryboard->SetNotifyObj(m_pIMultiLabel);

        UI::IIntTimeline* p = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
        p->SetFromTo(255, 0);
        p->SetDuration(500);
        p->SetOutRef(&m_nTextAlpha);
        pStoryboard->AddTimeline(p);

        pStoryboard->Begin();
    }
}

void  MultiLabel::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
    if (ppArray[0]->FindTimeline(0)->IsFinish())
    {
        m_nTextAlpha = 255;
        ShowNextLabelNow();
    }
    else
    {
        m_pIMultiLabel->UpdateObject();
    }
}