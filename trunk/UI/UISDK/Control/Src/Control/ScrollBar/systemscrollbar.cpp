#include "stdafx.h"
#include "systemscrollbar.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"

#define TIMER_TIME_TOSCROLL      500
#define TIMER_TIME_SCROLLING     50

namespace UI
{

SystemScrollBar::SystemScrollBar()
{
    m_pISystemScrollBar = NULL;
    m_pMgrScrollBar =  NULL;

	m_pBtnLineUpLeft = m_pBtnLineDownRight = NULL;
	m_nTimer1IDToScroll = m_nTimer2IDToScroll = m_nTimer1IDScrolling = m_nTimer2IDScrolling = 0;

    m_pBtnThumb = NULL;
	m_nOldPage = m_nOldRange = m_nOldPos = 0;
	m_nClickDiff = 0;
	m_bTracking = false;
}

SystemScrollBar::~SystemScrollBar()
{
	// 这三个对象会在parent的destroyChildObject中被释放，因此这里只需要置空就行
	// 但是在ResetAttribute的时候，又必须我们自己释放这三个对象
	m_pBtnLineDownRight = NULL;
	m_pBtnLineUpLeft = NULL;
	m_pBtnThumb = NULL;

    if (m_pMgrScrollBar && HSCROLLBAR == GetDirType())
        m_pMgrScrollBar->SetHScrollBar(NULL);
    else
        m_pMgrScrollBar->SetVScrollBar(NULL);
}

HRESULT  SystemScrollBar::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(ISystemScrollBar, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    IButton::CreateInstance(p, &m_pBtnThumb);
    if (m_pBtnThumb)
    {
        m_pBtnThumb->SetID(XML_SCROLLBAR_THUMB_BUTTON_ATTR_PRIFIX);
        m_pISystemScrollBar->AddChild(m_pBtnThumb);
        m_pBtnThumb->SetOutRef((void**)&m_pBtnThumb);
        m_pBtnThumb->AddHook(m_pISystemScrollBar, 0, ALT_MSG_ID_THUMB_BTN);

        m_pBtnThumb->SetButtonStyle(BUTTON_STYLE_HSCROLLTHUMB); // 先随便写一个值，在子类中再设置一次
        m_pBtnThumb->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
        m_pBtnThumb->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
        m_pBtnThumb->SetTabstop(false);
    }

    // 默认隐藏，这样才能触发第一次滚动条需要显示的场景
    m_pISystemScrollBar->SetVisible(false, false, false);
    return S_OK;
}

void  SystemScrollBar::SetIScrollBarMgr(IScrollBarManager* p)
{
    m_pMgrScrollBar = p; 
    if (p)
    {
        if (HSCROLLBAR == GetDirType())
            p->SetHScrollBar(static_cast<IMessage*>(m_pISystemScrollBar));
        else
            p->SetVScrollBar(static_cast<IMessage*>(m_pISystemScrollBar));
    }
}

void  SystemScrollBar::ResetAttribute()
{
    DO_PARENT_PROCESS(ISystemScrollBar, IControl);
	SAFE_DELETE_Ixxx(m_pBtnLineDownRight);
	SAFE_DELETE_Ixxx(m_pBtnLineUpLeft);
	SAFE_DELETE_Ixxx(m_pBtnThumb);
}

void  SystemScrollBar::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    DO_PARENT_PROCESS(ISystemScrollBar, IControl);

    bool bNoLineButton = false;
    pMapAttrib->GetAttr_bool(XML_SCROLLBAR_NO_LINEBTN, true, &bNoLineButton);
	if (bNoLineButton)  // 不显示行按钮，简洁
	{
		SAFE_DELETE_Ixxx(m_pBtnLineUpLeft);
		SAFE_DELETE_Ixxx(m_pBtnLineDownRight);
	}
	else
	{
		if (NULL == m_pBtnLineUpLeft)
		{
			IButton::CreateInstance(m_pISystemScrollBar->GetUIApplication(), &m_pBtnLineUpLeft);
			if (m_pBtnLineUpLeft)
			{
				m_pBtnLineUpLeft->SetID(XML_SCROLLBAR_LINE_BUTTON1_ATTR_PRIFIX);
				m_pISystemScrollBar->AddChild(m_pBtnLineUpLeft);
				m_pBtnLineUpLeft->AddHook(m_pISystemScrollBar, 0, ALT_MSG_ID_BUTTON1);
                m_pBtnLineUpLeft->SetOutRef((void**)&m_pBtnLineUpLeft);

				m_pBtnLineUpLeft->SetButtonStyle(BUTTON_STYLE_SCROLLLINEUP);  // 先取个默认值
				m_pBtnLineUpLeft->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
				m_pBtnLineUpLeft->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
				m_pBtnLineUpLeft->SetTabstop(false);
			}
		}
		if (NULL == m_pBtnLineDownRight)
		{
			IButton::CreateInstance(m_pISystemScrollBar->GetUIApplication(), &m_pBtnLineDownRight);
			if (m_pBtnLineDownRight)
			{
				m_pBtnLineDownRight->SetID(XML_SCROLLBAR_LINE_BUTTON2_ATTR_PRIFIX);
				m_pISystemScrollBar->AddChild(m_pBtnLineDownRight);
				m_pBtnLineDownRight->AddHook(m_pISystemScrollBar, 0, ALT_MSG_ID_BUTTON2);
                m_pBtnLineDownRight->SetOutRef((void**)&m_pBtnLineDownRight);

				m_pBtnLineDownRight->SetButtonStyle(BUTTON_STYLE_SCROLLLINEDOWN);  // 先取个默认值
				m_pBtnLineDownRight->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
				m_pBtnLineDownRight->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
				m_pBtnLineDownRight->SetTabstop(false);
			}
		}
	}
    if (m_pBtnLineUpLeft)
	    m_pBtnLineUpLeft->SetAttributeByPrefix(XML_SCROLLBAR_LINE_BUTTON1_ATTR_PRIFIX, pMapAttrib, false, true);
    if (m_pBtnLineDownRight)
	    m_pBtnLineDownRight->SetAttributeByPrefix(XML_SCROLLBAR_LINE_BUTTON2_ATTR_PRIFIX, pMapAttrib, false, true);
    if (m_pBtnThumb)
	    m_pBtnThumb->SetAttributeByPrefix(XML_SCROLLBAR_THUMB_BUTTON_ATTR_PRIFIX, pMapAttrib, false, true);
}


IObject*  SystemScrollBar::GetBindObject() 
{ 
    if (NULL == m_pMgrScrollBar)
        return NULL;

    return m_pMgrScrollBar->GetBindObject(); 
}
void  SystemScrollBar::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    IRenderBase* p = m_pISystemScrollBar->GetBkRender();
    if (p)
    {
        *pSize = p->GetDesiredSize();
        return;
    }

	if (m_pBtnThumb)
    {
		*pSize = m_pBtnThumb->GetDesiredSize();
        return;
    }

    pSize->cx = GetSystemMetrics(SM_CXVSCROLL);
    pSize->cy = GetSystemMetrics(SM_CYVSCROLL);
}

LRESULT SystemScrollBar::OnScrollBarInfoChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UISCROLLINFO* pInfo = (UISCROLLINFO*)wParam;
    bool bVisible1 = m_pISystemScrollBar->IsMySelfVisible();
    CalcScrollBarVisible();
    bool bVisible2 = m_pISystemScrollBar->IsMySelfVisible();

    if (bVisible1 != bVisible2)
        return 1;
    return 0;
}

LRESULT  SystemScrollBar::OnSetIScrollBarMgr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetIScrollBarMgr((IScrollBarManager*)wParam);
    return 0;
}

void  SystemScrollBar::OnPaint(IRenderTarget*)
{
	SetMsgHandled(FALSE);

	if (m_nOldRange != m_pMgrScrollBar->GetScrollRange(GetDirType()) ||
		m_nOldPage != m_pMgrScrollBar->GetScrollPage(GetDirType())   ||
		m_nOldPos != m_pMgrScrollBar->GetScrollPos(GetDirType()))
	{
		bool bNeedUpdateThumbButtonSize = false;
		if (m_nOldRange != m_pMgrScrollBar->GetScrollRange(GetDirType()) ||
			m_nOldPage != m_pMgrScrollBar->GetScrollPage(GetDirType()) )
		{
			bNeedUpdateThumbButtonSize = true;
		}
		if (this->CalcThumbButtonPos(bNeedUpdateThumbButtonSize))
		{
			m_nOldRange = m_pMgrScrollBar->GetScrollRange(GetDirType());
			m_nOldPage = m_pMgrScrollBar->GetScrollPage(GetDirType());
			m_nOldPos = m_pMgrScrollBar->GetScrollPos(GetDirType());
		}
	}
}

void  SystemScrollBar::OnTimer(UINT_PTR idEvent, LPARAM lParam)
{	
    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if(idEvent == m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
		m_nTimer1IDScrolling = pUIApplication->SetTimer(TIMER_TIME_SCROLLING, m_pISystemScrollBar);
	}
	else if(idEvent == m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll);
		m_nTimer2IDScrolling = pUIApplication->SetTimer(TIMER_TIME_SCROLLING, m_pISystemScrollBar);
	}
	else if(idEvent == m_nTimer1IDScrolling)
	{
		if (m_pBtnLineUpLeft)
		{
			// 检测鼠标是否在其上面
			POINT pt, ptObj;
			::GetCursorPos(&pt);
			::MapWindowPoints(NULL, m_pISystemScrollBar->GetHWND(), &pt, 1);
			m_pISystemScrollBar->WindowPoint2ObjectPoint(&pt, &ptObj);
		
			if(HTNOWHERE != UISendMessage(m_pBtnLineUpLeft, UI_WM_HITTEST, ptObj.x, ptObj.y))
			{
				m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEUP);
			}
		}
	}
	else if(idEvent == m_nTimer2IDScrolling)
	{
		if (m_pBtnLineDownRight)
		{
			// 检测鼠标是否在其上面
			POINT pt, ptObj;
			::GetCursorPos(&pt);
			::MapWindowPoints(NULL, m_pISystemScrollBar->GetHWND(), &pt, 1);
			m_pISystemScrollBar->WindowPoint2ObjectPoint(&pt, &ptObj);

			if(HTNOWHERE != UISendMessage(m_pBtnLineDownRight, UI_WM_HITTEST, ptObj.x, ptObj.y))
			{
				m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEDOWN);  // ScrollbarMgr::ProcessMessage中会负责刷新
			}
		}
	}
}


void  SystemScrollBar::OnStateChanged(UINT nMask)
{

}
void  SystemScrollBar::OnBtn1LButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
		m_nTimer1IDToScroll = 0;
	}
	if (0 != m_nTimer1IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer1IDScrolling);
		m_nTimer1IDScrolling = 0;
	}

	m_nTimer1IDToScroll = pUIApplication->SetTimer(TIMER_TIME_TOSCROLL, m_pISystemScrollBar);

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEUP);
}

void  SystemScrollBar::OnBtn2LButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll);
	}
	if (0 != m_nTimer2IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer2IDScrolling);
	}

	m_nTimer2IDToScroll = pUIApplication->SetTimer(TIMER_TIME_TOSCROLL, m_pISystemScrollBar);

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_LINEDOWN);
}
void  SystemScrollBar::OnBtn1LButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
	}
	if (0 != m_nTimer1IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer1IDScrolling);
	}

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
}

void  SystemScrollBar::OnBtn1KillFocus(IObject* pNewFocusObj)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer1IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer1IDToScroll);
	}
	if (0 != m_nTimer1IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer1IDScrolling);
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
}

void  SystemScrollBar::OnBtn2LButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll);
	}
	if (0 != m_nTimer2IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer2IDScrolling);
	}

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
}
void  SystemScrollBar::OnBtn2KillFocus(IObject* pNewFocusObj)
{
	SetMsgHandled(FALSE);

    IUIApplication* pUIApplication = m_pISystemScrollBar->GetUIApplication();
	if (0 != m_nTimer2IDToScroll)
	{
		pUIApplication->KillTimer(m_nTimer2IDToScroll );
	}
	if (0 != m_nTimer2IDScrolling)
	{
		pUIApplication->KillTimer(m_nTimer2IDScrolling);
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
}

void  SystemScrollBar::CalcScrollBarVisible()
{
	int nRange = m_pMgrScrollBar->GetScrollRange(GetDirType());
	int nPage = m_pMgrScrollBar->GetScrollPage(GetDirType());

	SCROLLBAR_VISIBLE_TYPE eVisibleType = m_pMgrScrollBar->GetScrollBarVisibleType(GetDirType());
	if (nPage >= nRange)
	{
		switch (eVisibleType)
		{
		case SCROLLBAR_VISIBLE_AUTO:
			if (m_pISystemScrollBar->IsMySelfVisible())
				m_pISystemScrollBar->SetVisible(false, false/*true*/);
			break;

		case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
			this->m_pBtnThumb->SetVisible(false, false/*true*/);
			break;
		}
	}
	else
	{
		switch (eVisibleType)
		{
		case SCROLLBAR_VISIBLE_AUTO:
			if (false == m_pISystemScrollBar->IsMySelfVisible())
				this->m_pISystemScrollBar->SetVisible(true, false/*true*/);
			break;

		case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
			this->m_pBtnThumb->SetVisible(true, false/*true*/);
			break;
		}
	}
}

bool  SystemScrollBar::CalcThumbButtonPos(bool bNeedUpdateThumbButtonSize)
{
	if (NULL == m_pBtnThumb)
		return false;

	float nPos = (float)m_pMgrScrollBar->GetScrollPos(GetDirType());
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(GetDirType());
	float nPage = (float)m_pMgrScrollBar->GetScrollPage(GetDirType());

	int nNewSize = 0;
	if (bNeedUpdateThumbButtonSize)
	{
		nNewSize = this->CalcThumbButtonSize();
	}
	else
	{
		if (VSCROLLBAR == GetDirType())
			nNewSize = this->m_pBtnThumb->GetHeight();
		else 
			nNewSize = this->m_pBtnThumb->GetWidth();
	}

	if (nNewSize < MIN_TRACK_BTN_SIZE)
		nNewSize = MIN_TRACK_BTN_SIZE;

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);
	if (rcChannel.IsRectEmpty())
		return false;

	if (VSCROLLBAR == GetDirType())
	{
		int nThumbBtnPos = (int)(nPos / (nRange-nPage) * (float)(rcChannel.Height() - nNewSize)) + rcChannel.top;
		this->m_pBtnThumb->SetObjectPos(rcChannel.left, nThumbBtnPos, rcChannel.Width(), nNewSize, SWP_NOREDRAW);
	}
	else
	{
		int nThumbBtnPos = (int)(nPos / (nRange-nPage) * (float)(rcChannel.Width() - nNewSize)) + rcChannel.left;
		this->m_pBtnThumb->SetObjectPos(nThumbBtnPos, rcChannel.top, nNewSize, rcChannel.Height(), SWP_NOREDRAW);
	}
	return true;
}


void  SystemScrollBar::CalcChannelRect(CRect* prc)
{
	m_pISystemScrollBar->GetClientRect(prc);

	if (GetDirType() == VSCROLLBAR)
	{
		if (m_pBtnLineUpLeft)
		{
			int nHeight = m_pBtnLineUpLeft->GetHeight();
			prc->top += nHeight;
		}
		if (m_pBtnLineDownRight)
		{
			int nHeight = m_pBtnLineDownRight->GetHeight();
			prc->bottom -= nHeight;
		}
	}
	else
	{
		if (m_pBtnLineUpLeft)
			prc->left += m_pBtnLineUpLeft->GetWidth();
		if (m_pBtnLineDownRight)
			prc->right -= m_pBtnLineDownRight->GetWidth();
	}
}


int   SystemScrollBar::CalcThumbButtonSize()
{
	if (NULL == m_pBtnThumb)
		return -1;

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);

	float nPage = (float)m_pMgrScrollBar->GetScrollPage(GetDirType());
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(GetDirType());
	if (0 == nRange)
		return -1;

	if (GetDirType() == VSCROLLBAR)
	{
		int nBtnH = (int)((float)rcChannel.Height()*nPage/nRange + 0.5);
		return nBtnH;
	}
	else
	{
		int nBtnW = (int)((float)rcChannel.Width()*nPage/nRange + 0.5);
		return nBtnW;
	}
}

//////////////////////////////////////////////////////////////////////////
SystemVScrollBar::SystemVScrollBar()
{
    m_pISystemVScrollBar = NULL;
	if (m_pBtnThumb)
		m_pBtnThumb->SetButtonStyle(BUTTON_STYLE_VSCROLLTHUMB);
}

HRESULT  SystemVScrollBar::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = SystemScrollBar::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    if (m_pBtnThumb)
        m_pBtnThumb->SetButtonStyle(BUTTON_STYLE_VSCROLLTHUMB);

    return S_OK;
}

void  SystemVScrollBar::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	__super::SetAttribute(pMapAttrib, bReload);

    IUIApplication* pUIApp = m_pISystemScrollBar->GetUIApplication();
	if (m_pBtnLineUpLeft && NULL == m_pBtnLineUpLeft->GetBkRender())
	{
		m_pBtnLineUpLeft->SetButtonStyle(BUTTON_STYLE_SCROLLLINEUP);
        
		IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_UP_BUTTON, m_pBtnLineUpLeft, &pRender);
        if (pRender)
        {
            pRender->SetAttribute(pMapAttrib, XML_SCROLLBAR_LINE_BUTTON1_ATTR_PRIFIX XML_BACKGND_RENDER_PREFIX, true);
		    m_pBtnLineUpLeft->SetBkgndRender(pRender);
		    SAFE_RELEASE(pRender);
        }
	}
	if (m_pBtnLineDownRight && NULL == m_pBtnLineDownRight->GetBkRender())
	{
		m_pBtnLineDownRight->SetButtonStyle(BUTTON_STYLE_SCROLLLINEDOWN);

        IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_DOWN_BUTTON, m_pBtnLineDownRight, &pRender);
        if (pRender)
        {
            pRender->SetAttribute(pMapAttrib, XML_SCROLLBAR_LINE_BUTTON2_ATTR_PRIFIX XML_BACKGND_RENDER_PREFIX, true);
    		m_pBtnLineDownRight->SetBkgndRender(pRender);
		    SAFE_RELEASE(pRender);
        }
	}

	if (m_pBtnThumb && NULL == m_pBtnThumb->GetBkRender())
	{
        IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_VSCROLLBAR_THUMB_BUTTON, m_pBtnThumb, &pRender);
        if (pRender)
        {
            pRender->SetAttribute(pMapAttrib, XML_SCROLLBAR_THUMB_BUTTON_ATTR_PRIFIX XML_BACKGND_RENDER_PREFIX, true);
    		m_pBtnThumb->SetBkgndRender(pRender);
	    	SAFE_RELEASE(pRender);
        }
	}

	// 默认背景(theme)
	if (NULL == m_pISystemScrollBar->GetBkRender())
	{
        IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_VSCROLLBARBACKGND, m_pISystemScrollBar, &pRender);
        if (pRender)
        {
            pRender->SetAttribute(pMapAttrib, XML_BACKGND_RENDER_PREFIX, true);
    		m_pISystemScrollBar->SetBkgndRender(pRender);
	    	SAFE_RELEASE(pRender);
        }
	}
}

void  SystemVScrollBar::OnLButtonDown(UINT nFlags, POINT point)
{
	if (NULL == m_pBtnThumb)
		return;

	POINT ptObj;
	m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj);
	
	if (ptObj.y < 0)
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGEUP);
	else
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGEDOWN);

}
void  SystemVScrollBar::OnLButtonUp(UINT nFlags, POINT point)
{
	if (NULL == m_pBtnThumb)
		return;

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
}

// 将thumb button的最前面的位置转换为当前位置 pt相对于scrollbar
int   SystemVScrollBar::WindowPoint2TrackPos(int nUIPos)
{
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(VSCROLLBAR);
	float nPage = (float)m_pMgrScrollBar->GetScrollPage(VSCROLLBAR);

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);
	int nUIRange = rcChannel.Height();

	if (m_pBtnThumb)
		nUIRange -= m_pBtnThumb->GetHeight();

	if (0 == nUIRange)
		return 0;

	float nPos =  ((float)(nUIPos - rcChannel.top))/((float)nUIRange)*(nRange-nPage);
	return (int)nPos;
}

void  SystemVScrollBar::OnThumbBtnLButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	// 将窗口坐标转换为相对于控件的坐标
	POINT  ptObj;
	m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj);
	m_nClickDiff = ptObj.y;
}
void  SystemVScrollBar::OnThumbBtnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (m_bTracking)
	{
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
	m_bTracking = false;
	m_nClickDiff = 0;
}
void  SystemVScrollBar::OnThumbBtnMousemove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (!(nFlags & MK_LBUTTON))
		return;

	POINT ptObj;
	m_pISystemScrollBar->WindowPoint2ObjectPoint(&point, &ptObj);
	int nNewPos = this->WindowPoint2TrackPos(ptObj.y - m_nClickDiff);

	m_pMgrScrollBar->FireScrollMessage(VSCROLLBAR, SB_THUMBTRACK, nNewPos);
	m_bTracking = true;
}


//////////////////////////////////////////////////////////////////////////

SystemHScrollBar::SystemHScrollBar()
{
    m_pISystemHScrollBar = NULL;
	
}
HRESULT  SystemHScrollBar::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = SystemScrollBar::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    if (m_pBtnThumb)
        m_pBtnThumb->SetButtonStyle(BUTTON_STYLE_HSCROLLTHUMB);
    
    return S_OK;
}

void  SystemHScrollBar::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	__super::SetAttribute(pMapAttrib, bReload);
    IUIApplication* pUIApp = m_pISystemScrollBar->GetUIApplication();

	if (m_pBtnLineUpLeft && NULL == m_pBtnLineUpLeft->GetBkRender())
	{
		m_pBtnLineUpLeft->SetButtonStyle(BUTTON_STYLE_SCROLLLINELEFT);

        IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_LEFT_BUTTON, m_pBtnLineUpLeft, &pRender);
        if (pRender)
        {
    		m_pBtnLineUpLeft->SetBkgndRender(pRender);
	    	SAFE_RELEASE(pRender);
        }
	}
	if (m_pBtnLineDownRight && NULL == m_pBtnLineDownRight->GetBkRender())
	{
		m_pBtnLineDownRight->SetButtonStyle(BUTTON_STYLE_SCROLLLINERIGHT);

        IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_SCROLLBAR_LINE_RIGHT_BUTTON, m_pBtnLineDownRight, &pRender);
        if (pRender)
        {
		    m_pBtnLineDownRight->SetBkgndRender(pRender);
		    SAFE_RELEASE(pRender);
        }
	}
	if (m_pBtnThumb && NULL == m_pBtnThumb->GetBkRender())
	{
        IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_HSCROLLBAR_THUMB_BUTTON, m_pBtnThumb, &pRender);
        if (pRender)
        {
	    	m_pBtnThumb->SetBkgndRender(pRender);
		    SAFE_RELEASE(pRender);
        }
	}

	// 默认背景(theme)
	if (NULL == m_pISystemScrollBar->GetBkRender())
	{
        IRenderBase* pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_HSCROLLBARBACKGND, m_pISystemScrollBar, &pRender);
        if (pRender)
        {
            m_pISystemScrollBar->SetBkgndRender(pRender);
		    SAFE_RELEASE(pRender);
        }
	}
}

void  SystemHScrollBar::OnLButtonDown(UINT nFlags, POINT point)
{
	if (NULL == m_pBtnThumb)
		return;

	POINT ptObj;
	m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj);

	if (ptObj.x < 0)
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGELEFT);
	else
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_PAGERIGHT);
}
void  SystemHScrollBar::OnLButtonUp(UINT nFlags, POINT point)
{
	if (NULL == m_pBtnThumb)
		return;

	m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
}

// 将thumb button的最前面的位置转换为当前位置  pt相对于scrollbar
int   SystemHScrollBar::WindowPoint2TrackPos(int nUIPos)
{
	float nRange = (float)m_pMgrScrollBar->GetScrollRange(HSCROLLBAR);
	float nPage = (float)m_pMgrScrollBar->GetScrollPage(HSCROLLBAR);

	CRect rcChannel;
	this->CalcChannelRect(&rcChannel);
	int nUIRange = rcChannel.Width();
	if (m_pBtnThumb)
		nUIRange -= m_pBtnThumb->GetWidth();
	if (0 == nUIRange)
		return 0;

	float nPos =  ((float)(nUIPos - rcChannel.left))/((float)nUIRange)*(nRange-nPage);
	return (int)nPos;
}

void  SystemHScrollBar::OnThumbBtnLButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	// 将窗口坐标转换为相对于控件的坐标
	POINT  ptObj;
	m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj);
	m_nClickDiff = ptObj.x;
}
void  SystemHScrollBar::OnThumbBtnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (m_bTracking)
	{
		m_pMgrScrollBar->FireScrollMessage(GetDirType(), SB_ENDSCROLL);
	}
	m_bTracking = false;
	m_nClickDiff = 0;
}
void  SystemHScrollBar::OnThumbBtnMousemove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (!(nFlags & MK_LBUTTON))
		return;

	POINT ptObj;
	m_pISystemScrollBar->WindowPoint2ObjectPoint(&point, &ptObj);
	int nNewPos = this->WindowPoint2TrackPos(ptObj.x - m_nClickDiff);

	m_pMgrScrollBar->FireScrollMessage(HSCROLLBAR, SB_THUMBTRACK, nNewPos);
	m_bTracking = true;
}


//////////////////////////////////////////////////////////////////////////

LRESULT  SystemVScrollBar::OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IObject* pBindObj = this->GetBindObject();
	if (NULL == pBindObj)
		return 0;

	if (!m_pISystemScrollBar->IsMySelfVisible())
		return 0;

    int cx = pBindObj->GetWidth();
    int cy = pBindObj->GetHeight();

	int nLeft = cx - pBindObj->GetNonClientR() + pBindObj->GetPaddingR();
	int nRight = cx - pBindObj->GetBorderR();
	int nTop = pBindObj->GetBorderT();
	int nBottom = cy - pBindObj->GetNonClientB() + pBindObj->GetPaddingB();

	m_pISystemScrollBar->SetObjectPos(nLeft, nTop, nRight-nLeft, nBottom-nTop, SWP_NOREDRAW);
    return 0;
}

LRESULT  SystemHScrollBar::OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IObject* pBindObj = this->GetBindObject();
	if (NULL == pBindObj)
		return 0;

	if (!m_pISystemScrollBar->IsMySelfVisible())
		return 0;

    int cx = pBindObj->GetWidth();
    int cy = pBindObj->GetHeight();

	int nTop = cy - pBindObj->GetNonClientB() + pBindObj->GetPaddingB();
	int nBottom = cy - pBindObj->GetBorderB();
	int nLeft = pBindObj->GetBorderL();
	int nRight = cx - pBindObj->GetNonClientR() + pBindObj->GetPaddingR();

	m_pISystemScrollBar->SetObjectPos(nLeft, nTop, nRight-nLeft, nBottom-nTop, SWP_NOREDRAW);
    return 0;
}


void  SystemHScrollBar::OnSize(UINT nType, int cx, int cy)
{
	CRect rcClient;
	m_pISystemScrollBar->GetClientRect(&rcClient);

	int nX1 = rcClient.left;
	int nX2 = rcClient.right;
	if (m_pBtnLineUpLeft)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineUpLeft->GetWidth();
		s.cy = m_pBtnLineUpLeft->GetHeight();

		if (s.cx == 0 || s.cy == 0)
			s = m_pBtnLineUpLeft->GetDesiredSize();

		m_pBtnLineUpLeft->SetObjectPos( 
			rcClient.left /*+ m_pBtnLineUpLeft->GetMarginL()*/,
			rcClient.top /*+ m_pBtnLineUpLeft->GetMarginT()*/, 
			s.cx,
			rcClient.Height() /*- m_pBtnLineUpLeft->GetMarginH()*/,
			SWP_NOREDRAW);

		nX1 += s.cx + m_pBtnLineUpLeft->GetMarginW();
	}
	if (m_pBtnLineDownRight)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineDownRight->GetWidth();
		s.cy = m_pBtnLineDownRight->GetHeight();

		if (s.cx == 0 || s.cy == 0)
			s = m_pBtnLineDownRight->GetDesiredSize();

		m_pBtnLineDownRight->SetObjectPos( 
			rcClient.right - s.cx /*- m_pBtnLineDownRight->GetMarginW()*/,
			rcClient.top /*+ m_pBtnLineDownRight->GetMarginT()*/, 
			s.cx,
			rcClient.Height() /*- m_pBtnLineDownRight->GetMarginH()*/,
			SWP_NOREDRAW);

		nX2 -= s.cx /*+ m_pBtnLineDownRight->GetMarginW()*/;
	}
	this->CalcThumbButtonPos(true);
}

void  SystemVScrollBar::OnSize(UINT nType, int cx, int cy)
{
	CRect rcClient;
	m_pISystemScrollBar->GetClientRect(&rcClient);

	int nY1 = rcClient.top;
	int nY2 = rcClient.bottom;
	if (m_pBtnLineUpLeft)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineUpLeft->GetWidth();
		s.cy = m_pBtnLineUpLeft->GetHeight();

		if (s.cx == 0 || s.cy == 0)
		{
			s = m_pBtnLineUpLeft->GetDesiredSize();
		}

		// 加了margin之后不好显示，暂不处理该margin
		m_pBtnLineUpLeft->SetObjectPos( 
			rcClient.left/* + m_pBtnLineUpLeft->GetMarginL()*/,
			rcClient.top /*+ m_pBtnLineUpLeft->GetMarginT()*/, 
			rcClient.Width() /*- m_pBtnLineUpLeft->GetMarginW()*/, 
			s.cy, 
			SWP_NOREDRAW);
		nY1 += s.cy /*+ m_pBtnLineUpLeft->GetMarginH()*/;
	}
	if(NULL != m_pBtnLineDownRight)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineDownRight->GetWidth();
		s.cy = m_pBtnLineDownRight->GetHeight();

		if (s.cx == 0 || s.cy == 0)
		{
			s = m_pBtnLineDownRight->GetDesiredSize();
		}

		m_pBtnLineDownRight->SetObjectPos(
			rcClient.left /*+ m_pBtnLineDownRight->GetMarginL()*/ , 
			rcClient.bottom -s.cy /*- m_pBtnLineDownRight->GetMarginH()*/,
			rcClient.Width() /*- m_pBtnLineDownRight->GetMarginW()*/, 
			s.cy, 
			SWP_NOREDRAW);
		nY2 -= s.cy /*+ m_pBtnLineDownRight->GetMarginH()*/;
	}
	this->CalcThumbButtonPos(true);
}
void SystemHScrollBar::OnCalcParentNonClientRect(CRegion4* prcNonClientRegion)
{
	if (false == m_pISystemScrollBar->IsMySelfVisible())
		return;

	int nHeight = m_pISystemScrollBar->GetHeight();
	if (0 == nHeight)
	{
		SIZE s = m_pISystemScrollBar->GetDesiredSize();
		nHeight = s.cy;
	}
	prcNonClientRegion->bottom += nHeight;
}
void SystemVScrollBar::OnCalcParentNonClientRect(CRegion4* prcNonClientRegion)
{
	if (false == m_pISystemScrollBar->IsMySelfVisible())
		return;

	int nWidth = m_pISystemScrollBar->GetWidth();
	if (0 == nWidth)
	{
		SIZE s = m_pISystemScrollBar->GetDesiredSize();
		nWidth = s.cx;
	}
	prcNonClientRegion->right += nWidth;
}

}
