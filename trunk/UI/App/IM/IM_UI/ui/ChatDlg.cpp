#include "StdAfx.h"
#include "ChatDlg.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"

#define FRAME_SHADOW_SIZE  6
#define ANIMATE_ID_RIGHT_PANEL  1

CChatDlg::CChatDlg(void)
{
	m_pRightPanel = NULL;
	m_pLeftPanel = NULL;
	m_pBtnAnimate = NULL;
//	m_pListView = NULL;

	m_eRightPanelState = RIGHT_PANEL_STATE_EXPAND;
	m_nRightPanelConfigWidth = 0;
	m_nLeftPanelWidth = 0;

    m_pRichEditInput = m_pRichEditOutput = NULL;
}

CChatDlg::~CChatDlg(void)
{

}

void CChatDlg::OnInitWindow()
{
    ICustomWindow::xProcessMessage(GetCurMsg(), 0, 0);
    this->SetWindowResizeType(WRSB_ALL);

	m_pLeftPanel = (UI::IPanel*)this->FindChildObject(_T("panel_left"));
	m_pRightPanel = (UI::IPanel*)this->FindChildObject(_T("right_panel"));
	m_pBtnAnimate = (UI::IButton*)this->FindChildObject(_T("btn_expand_right_panel"));
	if (m_pRightPanel)
	{
		m_nRightPanelConfigWidth = m_pRightPanel->GetConfigWidth();
	}
    if (m_pLeftPanel)
    {
        m_pRichEditOutput = (UI::IRichEdit*)m_pLeftPanel->FindChildObject(_T("outputedit"));
        m_pRichEditInput = (UI::IRichEdit*)m_pLeftPanel->FindChildObject(_T("inputedit"));
    }

    if (m_pRichEditOutput)
    {
        //m_pRichEditOutput->SetMultiLine(false);
        //m_pRichEditOutput->SetAutoResize(true, 200);
    }
    if (m_pRichEditInput)
    {
    }

// 	if (m_pRightPanel)
// 	{
// 		m_pListView = (QQContactMgrListView*)m_pRightPanel->FindChildObject(_T("listview"));
// 	}
// 	if (m_pListView)
// 	{
// 		m_pListView->InsertColumn(0, _T("First"), 90);
// 		m_pListView->InsertColumn(1, _T("Second"), 90);
// 
// 		m_pListView->InsertItem(0, _T("航迹云"), LISTCTRL_ITEM_OPERATOR_FLAG_NOALL);
// 		m_pListView->InsertItem(1, _T("leeihcy"), LISTCTRL_ITEM_OPERATOR_FLAG_NOALL);
// 
// 		m_pListView->UpdateItemRect(m_pListView->GetFirstItem(), false);
// 	}

	SetWindowText(GetHWND(), _T("IM2012_Demo"));

    if (NULL == m_pLeftPanel->GetMaterialRender())
    {
        m_pLeftPanel->SetMaterialRender(g_pShareSkinRender);
    }
}

#if 0
void CChatDlg::OnBnClickChangeBkImg()
{
	CFileDialog  dlg(TRUE, _T("*.png"), 0,4|2, _T("*.png\0*.png\0*.jpg\0*.jpg\0*.bmp\0*.bmp\0*.*\0*.*\0\0"));
	if (IDOK !=dlg.DoModal())
		return;

	String strFileName = dlg.m_szFileName;

    UI::IImageManager* pImageMgr = NULL;
	g_pUIApp->GetActiveSkinImageMgr(&pImageMgr);
	if (pImageMgr)
		pImageMgr->ModifyImageItem(_T("chatframe_surface"), (BSTR)strFileName.c_str());
	
	// 求出图片平均色。注：QQ在这里对替换的图片还做了渐变处理，使得图片边缘与背景色连接融洽
    UI::IImageRes* pImageRes = NULL;
	UI::IColorRes* pColorRes = NULL;

	g_pUIApp->GetActiveSkinImageRes(&pImageRes);
	g_pUIApp->GetActiveSkinColorRes(&pColorRes);

    UI::IRenderBitmap* pRenderBitmap = NULL;
	
	if (pImageRes)
        pImageRes->GetBitmap(_T("chatframe_surface"), UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, &pRenderBitmap);

	COLORREF avgColor = 0;
	if (pRenderBitmap)
	{
		avgColor = pRenderBitmap->GetAverageColor();
		SAFE_RELEASE(pRenderBitmap);
	}

    UI::Color* pColor = NULL;
	if (pColorRes)
		pColorRes->GetColor(_T("avg_surface_color"), &pColor);

	if (pColor)
	{
		pColor->SetColor(avgColor);
		SAFE_RELEASE(pColor);
	}

	g_pUIApp->RedrawTopWindows();
}
#endif

void  CChatDlg::OnTextureAlphaChanged(int nNewAlpha)
{
//	if (nScrollType == SB_ENDSCROLL)
//	{
//         UI::IImageManager* pImageMgr = NULL;
// 		g_pUIApp->GetActiveSkinImageMgr(&pImageMgr);
// 		if (pImageMgr)
// 			pImageMgr->ModifyImageItemAlpha(_T("chatframe.png"), nPos);
// 
// 		g_pUIApp->RedrawTopWindows();
//	}

    UI::IRenderBase* p = m_pLeftPanel->GetForeRender();
    if (NULL == p)
        return;

    UI::IImageRender*  pBkgndRender = (UI::IImageRender*)p->QueryInterface(UI::uiiidof(IImageRender));
    if (NULL == pBkgndRender)
        return;

    pBkgndRender->SetAlpha(nNewAlpha);
}

void CChatDlg::OnBnClickAnimate()
{
	if (m_eRightPanelState == RIGHT_PANEL_STATE_ANIMATING_EXPAND ||
		m_eRightPanelState == RIGHT_PANEL_STATE_ANIMATING_COLLAPSE )
		return;

    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IIntTimeline* pIntTimeline = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
	pIntTimeline->SetDuration(300);

	if (m_eRightPanelState == RIGHT_PANEL_STATE_COLLAPSE)
	{
		pIntTimeline->SetFromTo(0, m_nRightPanelConfigWidth);
		m_eRightPanelState = RIGHT_PANEL_STATE_ANIMATING_EXPAND;
	}
	else
	{
		pIntTimeline->SetFromTo(m_nRightPanelConfigWidth, 0);
		m_eRightPanelState = RIGHT_PANEL_STATE_ANIMATING_COLLAPSE;
	}
	if (m_pLeftPanel)
	{
		m_nLeftPanelWidth = m_pLeftPanel->GetWidth();
	}

    UI::IStoryboard*  pStoryboard = pAnimateMgr->CreateStoryboard();
    pStoryboard->SetID(ANIMATE_ID_RIGHT_PANEL);
	pStoryboard->AddTimeline(pIntTimeline);
    pStoryboard->SetNotifyObj(this);
    pStoryboard->Begin();
}

void CChatDlg::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
	for (int i = 0; i < nCount; i++)
	{
		switch (ppArray[i]->GetID())
		{
		case ANIMATE_ID_RIGHT_PANEL:
			{
				int nWidth = 0;
				ppArray[i]->FindTimeline(0)->GetCurrentValue(&nWidth);
				m_pRightPanel->SetConfigRight(m_nRightPanelConfigWidth-nWidth);
				
				if (::IsZoomed(GetHWND()) && NULL != m_pLeftPanel)
				{ 
					int nRightWidth = nWidth-FRAME_SHADOW_SIZE;  // 6px 阴影
					if (nRightWidth < 0)
						nRightWidth = 0;
					m_pLeftPanel->SetConfigRight(nRightWidth);
				}

				this->UpdateLayout(true);

				if (ppArray[i]->IsFinish())
				{
					if (m_eRightPanelState == RIGHT_PANEL_STATE_ANIMATING_EXPAND)
					{
						m_eRightPanelState = RIGHT_PANEL_STATE_EXPAND;
					}
					else
					{
						m_eRightPanelState = RIGHT_PANEL_STATE_COLLAPSE;
					}
                    
                    // 动画过程中，动画按钮的位置改变了，但hover样式却没有修改。
                    // 再发一个鼠标消息更新hover对象
                    POINT pt;
                    GetCursorPos(&pt);
                    ::ScreenToClient(GetHWND(), &pt);
                    ::PostMessage(GetHWND(), WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
				}
			}
			break;
		}
	}
}
void CChatDlg::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);
	if (nID == SC_RESTORE && IsZoomed(GetHWND()))
	{
		if (m_pLeftPanel)
		{
			m_pLeftPanel->SetConfigRight(m_nRightPanelConfigWidth-FRAME_SHADOW_SIZE);
		}
	}
	else if (nID == SC_MAXIMIZE)
	{
		if (m_eRightPanelState == RIGHT_PANEL_STATE_COLLAPSE)
		{
			m_pLeftPanel->SetConfigRight(0);
		}
	}
    else if (nID == SC_CLOSE)
    {
        SetMsgHandled(TRUE);
        HideWindow();
        return;
    }
}
