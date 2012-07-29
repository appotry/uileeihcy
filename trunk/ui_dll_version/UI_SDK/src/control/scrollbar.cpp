#include "stdafx.h"


#define TIMER_ID_BTN1_TOSCROLL   1
#define TIMER_ID_BTN2_TOSCROLL   2
#define TIMER_ID_BTN1_SCROLLING  3
#define TIMER_ID_BTN2_SCROLLING  4

#define TIMER_TIME_TOSCROLL      500
#define TIMER_TIME_SCROLLING     50

#define ALT_MSG_ID_BUTTON1       1
#define ALT_MSG_ID_BUTTON2       2
#define ALT_MSG_ID_THUMB_BTN     3
#define ALT_MSG_ID_BINDOBJ       4

IScrollBarRender* CreateScrollBarRenderInstance(ScrollBarBase* pScrollBar, SCROLLBAR_TYPE eType, SCROLLBAR_DIRECTION_TYPE eScrollDirection);

//////////////////////////////////////////////////////////////////////////

ScrollBarMgr::ScrollBarMgr()
{
	m_pBindObject = NULL;
	m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
	m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
	m_pVScrollBar = NULL;
	m_pHScrollBar = NULL;
}
ScrollBarMgr::~ScrollBarMgr()
{
// 	SAFE_DELETE(m_pHScrollBar);  // 由parent object负责删除
// 	SAFE_DELETE(m_pVScrollBar);  

	m_pHScrollBar = NULL;
	m_pVScrollBar = NULL;
	m_pBindObject = NULL;
}


void ScrollBarMgr::ResetAttribute()
{
	m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
	m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;	

	if(NULL != m_pHScrollBar)
	{
		m_pHScrollBar->ResetAttribute();
	}
	if(NULL != m_pVScrollBar)
	{
		m_pVScrollBar->ResetAttribute();
	}
}
bool ScrollBarMgr::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	if (mapAttrib.count(XML_HSCROLLBAR))
	{
		String str = mapAttrib[XML_HSCROLLBAR];
		if (XML_SCROLLBAR_NONE == str) 
		{
			m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_NONE;
		}
		else if (XML_SCROLLBAR_AUTO == str)
		{
			m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
		}	
		else if (XML_SCROLLBAR_ALWAYSSHOW == str)
		{
			m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_SHOW_ALWAYS;
		}
		else if (XML_SCROLLBAR_ALWAYSHIDE == str)
		{
			m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_HIDE_ALWAYS;
		}
		
		m_pBindObject->EraseAttribute(XML_HSCROLLBAR);
	}
	if (mapAttrib.count(XML_VSCROLLBAR))
	{
		String str = mapAttrib[XML_VSCROLLBAR];
		if (XML_SCROLLBAR_NONE == str) 
		{
			m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_NONE;
		}
		else if (XML_SCROLLBAR_AUTO == str)
		{
			m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
		}	
		else if (XML_SCROLLBAR_ALWAYSSHOW == str)
		{
			m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_SHOW_ALWAYS;
		}
		else if (XML_SCROLLBAR_ALWAYSHIDE == str)
		{
			m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_HIDE_ALWAYS;
		}
		m_pBindObject->EraseAttribute(XML_VSCROLLBAR);
	}

	// 根据类型创建实例
	if (SCROLLBAR_VISIBLE_NONE == m_ehScrollbarVisibleType)
	{
		SAFE_DELETE(m_pHScrollBar);
		m_pBindObject->ModifyStyle(0, OBJECT_STYLE_HSCROLL);
	}
	else if (NULL == m_pHScrollBar)
	{
		m_pHScrollBar = new HScrollBar();
		m_pHScrollBar->m_strID = _T("hscrollbar");
		m_pBindObject->ModifyStyle(OBJECT_STYLE_HSCROLL);
	}

	if (SCROLLBAR_VISIBLE_NONE == m_evScrollbarVisibleType)
	{
		SAFE_DELETE(m_pVScrollBar);
		m_pBindObject->ModifyStyle(0, OBJECT_STYLE_VSCROLL);
	}
	else if (NULL == m_pVScrollBar)
	{
		m_pVScrollBar = new VScrollBar();
		m_pVScrollBar->m_strID = _T("vscrollbar");
		m_pBindObject->ModifyStyle(OBJECT_STYLE_VSCROLL);
	}

	// 设置属性
	if(NULL != m_pHScrollBar)
	{
		m_pHScrollBar->Init(this);
		m_pBindObject->SetChildObjectAttribute(m_pHScrollBar, XML_HSCROLLBAR_PRIFIX, mapAttrib, bReload);
		m_pHScrollBar->SetVisible(false, false);
	}
	if(NULL != m_pVScrollBar)
	{
		m_pVScrollBar->Init(this);
		m_pBindObject->SetChildObjectAttribute(m_pVScrollBar, XML_VSCROLLBAR_PRIFIX, mapAttrib, bReload);
		m_pVScrollBar->SetVisible(false, false);
	}

	return true;
}
void ScrollBarMgr::SetBindObject(Object* pBindObj)
{
	UIASSERT(NULL != pBindObj);
	m_pBindObject = pBindObj;
}	

void ScrollBarMgr::UpdateBindObjectNonClientRect()
{
	CRect rcNonClient(0,0,0,0);
	if (NULL != m_pVScrollBar)
	{
		UISendMessage(m_pVScrollBar, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&rcNonClient,0,0, ALT_MSG_ID_BINDOBJ);
	}
	if (NULL != m_pHScrollBar)
	{
		UISendMessage(m_pHScrollBar, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&rcNonClient,0,0, ALT_MSG_ID_BINDOBJ);
	}
	rcNonClient.left   = abs(rcNonClient.left);
	rcNonClient.top    = abs(rcNonClient.top);
	rcNonClient.right  = abs(rcNonClient.right);
	rcNonClient.bottom = abs(rcNonClient.bottom);
	this->m_pBindObject->SetNonClientRegion( &rcNonClient );
}

BOOL ScrollBarMgr::ProcessMessage(UIMSG* pMsg, int nMsgMapID)
{
	// 如果bindobject没有处理，则在这里采用默认的处理
	if (WM_VSCROLL == pMsg->message)
	{
		if (m_pVScrollBar != pMsg->pObjMsgFrom)
			return FALSE;

		int nSBCode = pMsg->wParam;
		int nTrackPos = pMsg->lParam;

		int nOldPos = GetVScrollPos();
		switch (nSBCode)
		{
		case SB_LINEUP:
			this->m_pVScrollBar->ScrollLineUpLeft();
			break;

		case SB_LINEDOWN:
			this->m_pVScrollBar->ScrollLineDownRight();
			break;

		case SB_PAGEUP:
			this->m_pVScrollBar->ScrollPageUpLeft();
			break;

		case SB_PAGEDOWN:
			this->m_pVScrollBar->ScrollPageDownRight();
			break;

		case SB_THUMBTRACK:    // Drag scroll box to specified position. The current position is provided in nPos.
		case SB_THUMBPOSITION: // Scroll to the absolute position. The current position is provided in nPos
			this->m_pVScrollBar->SetScrollPos(nTrackPos);
			break;
		}

		if (nOldPos != GetVScrollPos())
		{
			this->GetBindObject()->UpdateObject();
		}
	}
	else if (WM_HSCROLL == pMsg->message)
	{
		if (m_pHScrollBar != pMsg->pObjMsgFrom)
			return FALSE;

		int nSBCode = pMsg->wParam;
		int nTrackPos = pMsg->lParam;

		int nOldPos = GetHScrollPos();
		switch (nSBCode)
		{
		case SB_LINEUP:
			this->m_pHScrollBar->ScrollLineUpLeft();
			break;

		case SB_LINEDOWN:
			this->m_pHScrollBar->ScrollLineDownRight();
			break;

		case SB_PAGEUP:
			this->m_pHScrollBar->ScrollPageUpLeft();
			break;

		case SB_PAGEDOWN:
			this->m_pHScrollBar->ScrollPageDownRight();
			break;

		case SB_THUMBTRACK:    // Drag scroll box to specified position. The current position is provided in nPos.
		case SB_THUMBPOSITION: // Scroll to the absolute position. The current position is provided in nPos
			this->m_pHScrollBar->SetScrollPos(nTrackPos);
			break;
		}

		if (nOldPos != GetHScrollPos())
		{
			this->GetBindObject()->UpdateObject();
		}
	}
	else if (WM_MOUSEWHEEL == pMsg->message)  
	{
		if (NULL != m_pVScrollBar)
		{
// 			UIMSG msg = *pMsg;
// 			msg.pObjMsgTo = m_pVScrollBar;
// 			UISendMessage(&msg, ALT_MSG_ID_BINDOBJ);
			short nDelta = HIWORD(pMsg->wParam);
			int   nOldPos = GetVScrollPos();

			if (nDelta < 0)
				m_pVScrollBar->ScrollWheelLineDown();
			else
				m_pVScrollBar->ScrollWheelLineUp();

			if (nOldPos != GetVScrollPos())
			{
				this->GetBindObject()->UpdateObject();
			}
		}
		else if (NULL != m_pHScrollBar)
		{
// 			UIMSG msg = *pMsg;
// 			msg.pObjMsgTo = m_pHScrollBar;
// 			UISendMessage(&msg, ALT_MSG_ID_BINDOBJ);

			int nDelta = HIWORD(pMsg->wParam);
			int nOldPos = GetHScrollPos();

			if (nDelta < 0)
				m_pHScrollBar->ScrollWheelLineDown();
			else
				m_pHScrollBar->ScrollWheelLineUp();

			if (nOldPos != GetHScrollPos())
			{
				this->GetBindObject()->UpdateObject();
			}
		}

		// 重新发送一个MOUSEMOVE消息给obj，用于定位滚动后的hover对象
		if (NULL != m_pHScrollBar || NULL != m_pVScrollBar)
		{
			POINT ptCursor;
			ptCursor.x = GET_X_LPARAM(pMsg->lParam); 
			ptCursor.y = GET_Y_LPARAM(pMsg->lParam);
			MapWindowPoints(NULL, GetBindObject()->GetHWND(), &ptCursor, 1);
			UISendMessage(GetBindObject(), WM_MOUSEMOVE, 0, MAKELPARAM(ptCursor.x,ptCursor.y));
		}
	}
	else if (WM_SIZE == pMsg->message)
	{
		CRect rcClient;
		m_pBindObject->GetClientRect(&rcClient);
		if (true == SetScrollPage(rcClient.Width(), rcClient.Height()))
		{
		 	if (NULL != m_pVScrollBar)
		 	{
		 		UIMSG msg = *pMsg;
		 		msg.pObjMsgTo = m_pVScrollBar;
		 		UISendMessage(&msg, ALT_MSG_ID_BINDOBJ);
		 	}
		 
		 	if (NULL != m_pHScrollBar)
		 	{
		 		UIMSG msg = *pMsg;
		 		msg.pObjMsgTo = m_pHScrollBar;
		 
		 		UISendMessage(&msg, ALT_MSG_ID_BINDOBJ);
		 	}
		}

	}
	else if (pMsg->message == UI_WM_GETSCROLLOFFSET)
	{
		this->GetScrollPos((int*)pMsg->wParam, (int*)pMsg->lParam);
	}
// 	else if (pMsg->message == UI_WM_GETSCROLLBAROBJECT)
// 	{
// 		long& lRet = pMsg->lRet;
// 		if (HSCROLLBAR == pMsg->wParam)
// 		{
// 			if (NULL == m_pHScrollBar)
// 				lRet = NULL;
// 			else
// 				lRet = (long)m_pHScrollBar->GetObjectPtr();
// 		}
// 		else if (VSCROLLBAR == pMsg->wParam)
// 		{
// 			if (NULL == m_pVScrollBar)
// 				lRet =  NULL;
// 			else
// 				lRet =  (long)m_pVScrollBar->GetObjectPtr();
// 		}
// 
// 		return TRUE;
// 	}

	return FALSE;
}

// UINT ScrollBarMgr::OnHitTest(POINT* pt)
// {
// 	if (NULL != m_pVScrollBar)
// 	{
// 		UINT nRet = UISendMessage(m_pVScrollBar, UI_WM_HITTEST, pt->x, pt->y);
// 		if (HTNOWHERE != nRet)
// 			return HTVSCROLL;
// 	}
// 	if (NULL != m_pHScrollBar)
// 	{
// 		UINT nRet = UISendMessage(m_pHScrollBar, UI_WM_HITTEST, pt->x, pt->y);
// 		if (HTNOWHERE != nRet)
// 			return HTHSCROLL;
// 	}
// 
// 	return HTNOWHERE;
// }

void ScrollBarMgr::MakeYVisible(int ny, bool bTopOrBottom)
{
	// TODO:
// 	if (NULL != m_pVScrollBar)
// 	{
// 		m_pVScrollBar->MakeVisible(ny, bTopOrBottom);
// 	}
}

void ScrollBarMgr::SetScrollPos(int nxPos, int nyPos)
{
	this->SetHScrollPos(nxPos);
	this->SetVScrollPos(nyPos);
}
void ScrollBarMgr::GetScrollPos(int* pnxOffset, int* pnyOffset)
{
	UIASSERT(NULL != pnxOffset && NULL != pnyOffset);

	*pnyOffset = GetVScrollPos();
	*pnxOffset = GetHScrollPos();
}
int ScrollBarMgr::GetHScrollPos()
{
	if (NULL != m_pHScrollBar)
		return m_pHScrollBar->GetScrollPos();

	return 0;
}
int ScrollBarMgr::GetVScrollPos()
{
	if (NULL != m_pVScrollBar)
		return m_pVScrollBar->GetScrollPos();

	return 0;
}
void ScrollBarMgr::SetScrollRange(int nX, int nY)
{
	bool bNeedUpdateNonClientRect = false;
	if (NULL != m_pVScrollBar)
	{
		bool bOldVisible = m_pVScrollBar->IsMySelfVisible();
		this->m_pVScrollBar->SetScrollRange(nY);
		bool bNowVisible = m_pVScrollBar->IsMySelfVisible();

		if (bOldVisible != bNowVisible)
		{
			bNeedUpdateNonClientRect = true;
		}
	}
	if (NULL != m_pHScrollBar)
	{
		bool bOldVisible = m_pHScrollBar->IsMySelfVisible();
		m_pHScrollBar->SetScrollRange(nX);
		bool bNowVisible = m_pHScrollBar->IsMySelfVisible();

		if (bOldVisible != bNowVisible)
		{
			bNeedUpdateNonClientRect = true;
		}
	}
	if (bNeedUpdateNonClientRect)
	{
		this->UpdateBindObjectNonClientRect();
		UISendMessage(this->GetBindObject(), WM_SIZE, 0,
			MAKELPARAM(
			this->GetBindObject()->GetWidth(), 
			this->GetBindObject()->GetHeight())
			);
	}

	if (NULL != m_pHScrollBar)
	{
		m_pHScrollBar->UpdateObject();
	}
	if (NULL != m_pVScrollBar)
	{
		m_pVScrollBar->UpdateObject();
	}
	return;
}
void ScrollBarMgr::SetHScrollRange(int nX)
{
}
void ScrollBarMgr::SetVScrollRange(int nY)
{
}
void ScrollBarMgr::SetVScrollLine(int nLine)
{
	if (NULL != m_pVScrollBar)
	{
		m_pVScrollBar->SetScrollButtonLine(nLine);
	}
}
void ScrollBarMgr::SetHScrollLine(int nLine)
{
}
void ScrollBarMgr::SetVScrollPage(int nPage)
{
}
bool ScrollBarMgr::SetScrollPage(int nxPage, int nyPage)
{
	if (NULL != m_pVScrollBar)
	{
		bool bOldVisible = m_pVScrollBar->IsMySelfVisible();
 		this->m_pVScrollBar->SetScrollPage(nyPage);
		bool bNowVisible = m_pVScrollBar->IsMySelfVisible();

		if (bOldVisible != bNowVisible)
		{
			this->UpdateBindObjectNonClientRect();
			UISendMessage(this->GetBindObject(), WM_SIZE, 0,
				MAKELPARAM(
				this->GetBindObject()->GetWidth(), 
				this->GetBindObject()->GetHeight())
				);
			return false;
		}
	}
	if (NULL != m_pHScrollBar)
	{
		bool bOldVisible = m_pHScrollBar->IsMySelfVisible();
 		this->m_pHScrollBar->SetScrollPage(nxPage);
		bool bNowVisible = m_pHScrollBar->IsMySelfVisible();
		
		if (bOldVisible != bNowVisible)
		{
			this->UpdateBindObjectNonClientRect();
			UISendMessage(this->GetBindObject(), WM_SIZE, 0,
				MAKELPARAM(
				this->GetBindObject()->GetWidth(), 
				this->GetBindObject()->GetHeight())
				);
			return false;
		}
	}

	if (NULL != m_pHScrollBar)
	{
		m_pHScrollBar->UpdateObject();
	}
	if (NULL != m_pVScrollBar)
	{
		m_pVScrollBar->UpdateObject();
	}
	return true;
}
void ScrollBarMgr::SetHScrollPage(int nPage)
{
	if (NULL != m_pHScrollBar)
	{
		m_pHScrollBar->SetScrollPage(nPage);
	}
}
void ScrollBarMgr::SetHScrollPos(int nX)
{
	if (NULL != m_pHScrollBar)
	{
		m_pHScrollBar->SetScrollPos(nX);
	}
	
}
void ScrollBarMgr::SetVScrollPos(int nY)
{
	if (NULL != m_pVScrollBar)
	{
		m_pVScrollBar->SetScrollPos(nY);
	}
}

SCROLLBAR_VISIBLE_TYPE ScrollBarMgr::GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType)
{ 
	if(HSCROLLBAR==eType)
		return m_ehScrollbarVisibleType;
	else 
		return m_evScrollbarVisibleType; 
}

void ScrollBarMgr::SetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eDirType, SCROLLBAR_VISIBLE_TYPE eVisType)
{
	if(HSCROLLBAR==eDirType)
		m_ehScrollbarVisibleType = eVisType;
	else 
		m_evScrollbarVisibleType = eVisType; 
}

//////////////////////////////////////////////////////////////////////////

ScrollBarBase::ScrollBarBase()
{
	m_pScrollBarMgr = NULL;
	m_pScrollBarRender = NULL;
	m_eScrollDirection = HSCROLLBAR;

	m_nRange = 1;
	m_nPos   = 0;
	m_nPage  = 1;
	m_nButtonLine = 1;
	m_nWheelLine = 3;
}
ScrollBarBase::~ScrollBarBase()
{
	SAFE_DELETE(m_pScrollBarRender);
}

bool ScrollBarBase::SetAttribute(ATTRMAP& mapAttrib, bool bReload )
{
	if (NULL == m_pScrollBarMgr || NULL == m_pScrollBarMgr->GetBindObject())
		return false;

	bool bRet = Control::SetAttribute(mapAttrib, bReload);
	if( false == bRet )
		return false;

	SCROLLBAR_TYPE eType = SCROLLBAR_TYPE_SYSTEM;
	if (mapAttrib.count(XML_HSCROLLBAR_PRIFIX XML_SCROLLBAR_TYPE) )
	{
		String strAttr = mapAttrib[XML_HSCROLLBAR_PRIFIX XML_SCROLLBAR_TYPE];
		if (XML_SCROLLBAR_TYPE_SLIDER == strAttr)
		{
			eType = SCROLLBAR_TYPE_SLIDER;
		}
		else if(XML_SCROLLBAR_TYPE_LOGIC == strAttr)
		{
			eType = SCROLLBAR_TYPE_LOGIC;
		}
		m_pScrollBarMgr->GetBindObject()->EraseAttribute(XML_HSCROLLBAR_PRIFIX XML_SCROLLBAR_TYPE);
	}
	m_pScrollBarRender = CreateScrollBarRenderInstance(this, eType, m_eScrollDirection);

	if (NULL != m_pScrollBarRender)
	{
		bRet = this->m_pScrollBarRender->SetAttribute(mapAttrib);
		if (false == bRet)
			return false;
	}

	return true;
}


void ScrollBarBase::ResetAttribute()
{
	Control::ResetAttribute();
	SAFE_DELETE(m_pScrollBarRender);
}

SIZE ScrollBarBase::GetAutoSize(HRDC hRDC)
{
// 	if (NULL != m_pScrollBarRender)
// 	{
// 		return m_pScrollBarRender->GetAutoSize();
// 	}

	// TODO:
	SIZE s = {12,12};
	return s;
}
void ScrollBarBase::Init(ScrollBarMgr* pMgr) 
{
	m_pScrollBarMgr = pMgr; 
	pMgr->GetBindObject()->AddNcChild(this);
}

int ScrollBarBase::GetScrollPos()
{
	return m_nPos;
}

void ScrollBarBase::SetScrollButtonLine(int nLine)
{
	m_nButtonLine = nLine;
}
void ScrollBarBase::SetScrollWheelLine(int nLine)
{
	m_nWheelLine = nLine;
}

bool ScrollBarBase::SetScrollInfo(LPUISCROLLINFO lpsi, bool bUpdate)
{
	if (NULL == lpsi || 0 == lpsi->nMask)
		return false;

	if (lpsi->nMask & UISIF_RANGE)
		m_nRange = lpsi->nRange;

	if (lpsi->nMask & UISIF_POS)
		m_nPos = lpsi->nPos;

	if (lpsi->nMask & UISIF_PAGE)
		m_nPage = lpsi->nPage;

	if (lpsi->nMask & UISIF_BUTTONLINE)
		m_nButtonLine = lpsi->nButtonLine;

	if (lpsi->nMask & UISIF_WHEELLINE)
		m_nWheelLine = lpsi->nWheelLine;

	if (bUpdate)
		this->UpdateObject();

	return true;
}

bool ScrollBarBase::SetScrollPos(int nPos/*, bool bUpdate*/)
{
	if (nPos > m_nRange-m_nPage)
	{
		nPos = m_nRange-m_nPage;
	}
	if (nPos < 0)
	{
		nPos = 0;
	}

	if (m_nPos != nPos)
	{
		m_nPos = nPos;

// 		if (bUpdate)
// 			this->UpdateObject();
	}
	return true;
}


void ScrollBarBase::SetScrollRange(int nRange)
{
	if (nRange < 0)
	{
		nRange = 0;
	}
	
	if (m_nRange != nRange)
	{
		m_nRange = nRange;
		if (NULL != m_pScrollBarRender)
			m_pScrollBarRender->UpdateScrollBarVisible();
	}
}

void ScrollBarBase::SetScrollPage(int nPage)
{
 	if (nPage < 0)
 	{
 		nPage = 0;
 	}
// 	if (nPage > m_nRange)  // page就是有可能超出range大小
// 	{
// 		nPage = m_nRange;
// 	}
 	if (nPage != m_nPage)
 	{
 		m_nPage = nPage;
 		if (NULL != m_pScrollBarRender)
 			m_pScrollBarRender->UpdateScrollBarVisible();
 	}
}

int ScrollBarBase::GetScrollRange()
{
	return m_nRange;
}
int ScrollBarBase::GetScrollPage()
{
	return m_nPage;
}
void ScrollBarBase::ScrollLineUpLeft()
{
	this->SetScrollPos(m_nPos-m_nButtonLine);
}
void ScrollBarBase::ScrollLineDownRight()
{
	this->SetScrollPos(m_nPos+m_nButtonLine);
}
void ScrollBarBase::ScrollWheelLineUp()
{
	this->SetScrollPos(m_nPos-m_nWheelLine);
}
void ScrollBarBase::ScrollWheelLineDown()
{
	this->SetScrollPos(m_nPos+m_nWheelLine);
}

void ScrollBarBase::ScrollPageDownRight()
{
	this->SetScrollPos(m_nPos+m_nPage);
}
void ScrollBarBase::ScrollPageUpLeft()
{
	this->SetScrollPos(m_nPos-m_nPage);
}
HScrollBar::HScrollBar()
{
	m_eScrollDirection = HSCROLLBAR;
}
HScrollBar::~HScrollBar()
{

}


VScrollBar::VScrollBar()
{
	m_eScrollDirection = VSCROLLBAR;
}
VScrollBar::~VScrollBar()
{

}

void HScrollBar::ResetAttribute()
{
	ScrollBarBase::ResetAttribute();
	m_eScrollDirection = HSCROLLBAR;
}
void VScrollBar::ResetAttribute()
{
	ScrollBarBase::ResetAttribute();
	m_eScrollDirection = VSCROLLBAR;
}

//////////////////////////////////////////////////////////////////////////


// 操作系统类型滚动条：两侧两个按钮 + 中间一个THUBM按钮
class SystemScrollBarRender : public IScrollBarRender
{
public:
	SystemScrollBarRender(ScrollBarBase* p):IScrollBarRender(p)
	{
		m_pBtnLineUpLeft = m_pBtnLineDownRight = NULL;
		m_nTimer1IDToScroll = m_nTimer2IDToScroll = m_nTimer1IDScrolling = m_nTimer2IDScrolling = 0;

		m_pBtnThumb = new ButtonBase;
		m_pBtnThumb->m_strID = _T("thumbbtn");
		this->m_pScrollBar->AddChild(m_pBtnThumb);
		m_pBtnThumb->AddHook(this,0,ALT_MSG_ID_THUMB_BTN);

		m_nOldPage = m_nOldRange = m_nOldPos = 0;
	}
	~SystemScrollBarRender()
	{
		SAFE_DELETE(m_pBtnLineDownRight);
		SAFE_DELETE(m_pBtnLineUpLeft);
		SAFE_DELETE(m_pBtnThumb);
	}

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_STATECHANGED(OnStateChanged)
		UIMSG_WM_TIMER(OnTimer)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON1)
		UIMSG_WM_LBUTTONDOWN(OnBtn1LButtonDown)	
		UIMSG_WM_LBUTTONUP(OnBtn1LButtonUp)
		UIMSG_WM_KILLFOCUS(OnBtn1KillFocus)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON2)
		UIMSG_WM_LBUTTONDOWN(OnBtn2LButtonDown)	
		UIMSG_WM_LBUTTONUP(OnBtn2LButtonUp)
		UIMSG_WM_KILLFOCUS(OnBtn2KillFocus)

	UIALT_MSG_MAP(ALT_MSG_ID_BINDOBJ)

	UI_END_MSG_MAP

	int  m_nOldPage;
	int  m_nOldRange;
	int  m_nOldPos;

	void OnPaint(HRDC)
	{
		SetMsgHandled(FALSE);

		if (m_nOldRange != this->m_pScrollBar->GetScrollRange() ||
			m_nOldPage != this->m_pScrollBar->GetScrollPage()   ||
			m_nOldPos != this->m_pScrollBar->GetScrollPos())
		{
			bool bNeedUpdateThumbButtonSize = false;
			if (m_nOldRange != this->m_pScrollBar->GetScrollRange() ||
				m_nOldPage != this->m_pScrollBar->GetScrollPage() )
			{
				bNeedUpdateThumbButtonSize = true;
			}
			if (this->UpdateThumbButtonPos(bNeedUpdateThumbButtonSize))
			{
				m_nOldRange = this->m_pScrollBar->GetScrollRange();
				m_nOldPage = this->m_pScrollBar->GetScrollPage();
				m_nOldPos = this->m_pScrollBar->GetScrollPos();
			}
		}
	}
	virtual bool UpdateThumbButtonPos(bool bNeedUpdateThumbButtonSize) = 0; 

	virtual  bool  SetAttribute(ATTRMAP& mapAttrib)
	{
// 		if( mapAttrib.count(XML_SCROLLBAR_HIDE_LINEBTN) )
// 		{
// 			SAFE_DELETE(m_pBtnLineUpLeft);
// 			SAFE_DELETE(m_pBtnLineDownRight);
// 		}
// 		else
//		{
		if (NULL == m_pBtnLineUpLeft)
		{
			m_pBtnLineUpLeft = new ButtonBase;
			m_pBtnLineUpLeft->m_strID = _T("lineupleftbtn");
			this->m_pScrollBar->AddChild(m_pBtnLineUpLeft);
			m_pBtnLineUpLeft->AddHook(this,0,ALT_MSG_ID_BUTTON1);
			m_pBtnLineUpLeft->SetTabstop(false);
		}
		if (NULL == m_pBtnLineDownRight)
		{
			m_pBtnLineDownRight = new ButtonBase;
			m_pBtnLineDownRight->m_strID = _T("linedownrightbtn");
			this->m_pScrollBar->AddChild(m_pBtnLineDownRight);
			m_pBtnLineDownRight->AddHook(this,0,ALT_MSG_ID_BUTTON2);
			m_pBtnLineDownRight->SetTabstop(false);
		}

		this->m_pScrollBar->SetChildObjectAttribute( m_pBtnLineUpLeft,    XML_SCROLLBAR_LINE_BUTTON1_ATTR_PRIFIX, mapAttrib, false );
		this->m_pScrollBar->SetChildObjectAttribute( m_pBtnLineDownRight, XML_SCROLLBAR_LINE_BUTTON2_ATTR_PRIFIX, mapAttrib, false );
		this->m_pScrollBar->SetChildObjectAttribute( m_pBtnThumb,         XML_SCROLLBAR_THUMB_BUTTON_ATTR_PRIFIX, mapAttrib, false );

		return true;
	}
	virtual  SIZE  GetAutoSize()
	{
		// TODO:
		SIZE s = {12,12};
		return s;
	}


protected:
	void     OnTimer(UINT_PTR nIDEvent);
	void     OnStateChanged(int nOld, int nNew);
	void     OnBtn1LButtonDown(UINT nFlags, POINT point);
	void     OnBtn2LButtonDown(UINT nFlags, POINT point);
	void     OnBtn1LButtonUp(UINT nFlags, POINT point);
	void     OnBtn1KillFocus( Object* pNewFocusObj );
	void     OnBtn2LButtonUp(UINT nFlags, POINT point);
	void     OnBtn2KillFocus( Object* pNewFocusObj );
	Object*  GetBindObject() { return m_pScrollBar->GetScrollMgr()->GetBindObject(); }

private:
	UINT_PTR         m_nTimer1IDToScroll;    // 按住lineup button后，500ms后开始滚动
	UINT_PTR         m_nTimer2IDToScroll;    // 按住linedown button后，500ms后开始滚动
	UINT_PTR         m_nTimer1IDScrolling;   // 按住lineup button后，连续滚动
	UINT_PTR         m_nTimer2IDScrolling;   // 按住linedown button后，连续滚动

protected:
	ButtonBase*      m_pBtnLineUpLeft;
	ButtonBase*      m_pBtnLineDownRight;
	ButtonBase*      m_pBtnThumb;
//	SliderCtrlBase*  m_pSliderCtrl;
};

class SystemVScrollBarRender : public SystemScrollBarRender
{
public:
	SystemVScrollBarRender(ScrollBarBase* p):SystemScrollBarRender(p)
	{
//		m_eScrollDirection = VSCROLLBAR;
//		if( NULL != m_pSliderCtrl )
//			m_pSliderCtrl->SetDirectionType(/*SCROLL_BOTTOM_2_TOP*/PROGRESS_SCROLL_TOP_2_BOTTOM);
		m_nClickDiff = 0;
		m_bTracking = false;
	}

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)

	UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
		UIMSG_WM_LBUTTONDOWN(OnThumbBtnLButtonDown)	
		UIMSG_WM_LBUTTONUP(OnThumbBtnLButtonUp)	
		UIMSG_WM_MOUSEMOVE(OnThumbBtnMousemove)

	UIALT_MSG_MAP(ALT_MSG_ID_BINDOBJ)
		UIMSG_WM_SIZE(OnBindObjSize)
		UIMSG_WM_NCCALCSIZE(OnNcCalcSize)

	UI_BEGIN_CHAIN_ALL_MSG_MAP
		UICHAIN_MSG_MAP(SystemScrollBarRender)
	UI_END_CHAIN_ALL_MSG_MAP

	UI_END_MSG_MAP

protected:
	virtual SCROLLBAR_DIRECTION_TYPE GetScrollBarDirType() { return VSCROLLBAR; }
	void    OnSize(UINT nType, int cx, int cy);
	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lprc);
	void    OnBindObjSize(UINT nType, int cx, int cy);

	void OnLButtonDown(UINT nFlags, POINT point)
	{
		if (NULL == m_pBtnThumb)
			return;

		POINT ptObj;
		m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj);
		if (ptObj.y < 0)
			this->m_pScrollBar->FireScrollMessage(SB_PAGEUP);
		else
			this->m_pScrollBar->FireScrollMessage(SB_PAGEDOWN);
	}
	void OnLButtonUp(UINT nFlags, POINT point)
	{
		if (NULL == m_pBtnThumb)
			return;

		this->m_pScrollBar->FireScrollMessage(SB_ENDSCROLL);
	}
	

	// 将thumb button的最前面的位置转换为当前位置
	// pt相对于scrollbar
	int     WindowPoint2TrackPos(int nUIPos)
	{
		float nRange = (float)m_pScrollBar->GetScrollRange();
		float nPage = (float)m_pScrollBar->GetScrollPage();

		CRect rcChannel;
		this->CalcChannelRect(&rcChannel);
		int nUIRange = rcChannel.Height();
		if (NULL != m_pBtnThumb)
			nUIRange -= m_pBtnThumb->GetHeight();
		if (0 == nUIRange)
			return 0;

		float nPos =  ((float)(nUIPos - rcChannel.top))/((float)nUIRange)*(nRange-nPage);
		return (int)nPos;
	}
	int m_nClickDiff;
	bool m_bTracking;

	void    OnThumbBtnLButtonDown(UINT nFlags, POINT point)
	{
		SetMsgHandled(FALSE);

		// 将窗口坐标转换为相对于控件的坐标
		POINT  ptObj;
		m_pBtnThumb->WindowPoint2ObjectPoint(&point, &ptObj);
		m_nClickDiff = ptObj.y;
	}
	void    OnThumbBtnLButtonUp(UINT nFlags, POINT point)
	{
		SetMsgHandled(FALSE);

		if (m_bTracking)
		{
			m_pScrollBar->FireScrollMessage(SB_ENDSCROLL);
		}
		m_bTracking = false;
		m_nClickDiff = 0;
	}
	void    OnThumbBtnMousemove(UINT nFlags, POINT point)
	{
		SetMsgHandled(FALSE);

		if(! (nFlags & MK_LBUTTON) )
			return;

		POINT ptObj;
		m_pScrollBar->WindowPoint2ObjectPoint(&point, &ptObj);
		int nNewPos = this->WindowPoint2TrackPos(ptObj.y - m_nClickDiff);

		m_pScrollBar->FireScrollMessage(SB_THUMBTRACK, nNewPos);
		m_bTracking = true;
	}

	virtual void UpdateScrollBarVisible()
	{
		float nPos = (float)m_pScrollBar->GetScrollPos();
		float nRange = (float)m_pScrollBar->GetScrollRange();
		float nPage = (float)m_pScrollBar->GetScrollPage();

		if (0 >= nRange - nPage)
		{
			switch (this->m_pScrollBar->GetScrollMgr()->GetScrollBarVisibleType(VSCROLLBAR))
			{
			case SCROLLBAR_VISIBLE_AUTO:
				if (this->m_pScrollBar->IsVisible())
					this->m_pScrollBar->SetVisible(false, true);
				break;
				
			case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
				this->m_pBtnThumb->SetVisible(false, true);
				break;
			}
		}
		else
		{
			switch (this->m_pScrollBar->GetScrollMgr()->GetScrollBarVisibleType(VSCROLLBAR))
			{
			case SCROLLBAR_VISIBLE_AUTO:
				if (false == this->m_pScrollBar->IsMySelfVisible())
					this->m_pScrollBar->SetVisible(true, true);
				break;

			case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
				this->m_pBtnThumb->SetVisible(true, true);
				break;
			}
		}
	}
	bool  UpdateThumbButtonPos(bool bNeedUpdateThumbButtonSize)
	{
		float nPos = (float)m_pScrollBar->GetScrollPos();
		float nRange = (float)m_pScrollBar->GetScrollRange();
		float nPage = (float)m_pScrollBar->GetScrollPage();

		int nNewSize = 0;
		if (bNeedUpdateThumbButtonSize)
			nNewSize = this->CalcThumbButtonSize();
		else
			nNewSize = this->m_pBtnThumb->GetHeight();

		if (nNewSize < 10)
			nNewSize = 10;

		CRect rcChannel;
		this->CalcChannelRect(&rcChannel);
		if (rcChannel.IsRectEmpty())
			return false;

		int nThumbBtnPos = (int)(nPos / (nRange-nPage) * (float)(rcChannel.Height() - nNewSize)) + rcChannel.top;
		this->m_pBtnThumb->SetObjectPos(rcChannel.left, nThumbBtnPos, rcChannel.Width(), nNewSize);
		return true;
	}

	void  CalcChannelRect(CRect* prc)
	{
		m_pScrollBar->GetClientRect(prc);

		if (NULL != m_pBtnLineUpLeft)
			prc->top += m_pBtnLineUpLeft->GetHeight();
		if (NULL != m_pBtnLineDownRight)
			prc->bottom -= m_pBtnLineDownRight->GetHeight();
	}
	int   CalcThumbButtonSize()
	{
		if (NULL == m_pBtnThumb)
			return -1;
		
		CRect rcChannel;
		this->CalcChannelRect(&rcChannel);
		
		float nPage = (float)m_pScrollBar->GetScrollPage();
		float nRange = (float)m_pScrollBar->GetScrollRange();
		if (0 == nRange)
			return -1;

		int nBtnH = (int)((float)rcChannel.Height()*nPage/nRange + 0.5);
		return nBtnH;
	}
};
class SystemHScrollBarRender : public SystemScrollBarRender
{
public:
	SystemHScrollBarRender(ScrollBarBase* p):SystemScrollBarRender(p)
	{
//		m_eScrollDirection = HSCROLLBAR;
//		if( NULL != m_pSliderCtrl )
//			m_pSliderCtrl->SetDirectionType(PROGRESS_SCROLL_LEFT_2_RIGHT);
	}
	UI_BEGIN_MSG_MAP
		UIMSG_WM_SIZE(OnSize)
	UIALT_MSG_MAP(ALT_MSG_ID_BINDOBJ)
		UIMSG_WM_SIZE(OnBindObjSize)
		UIMSG_WM_NCCALCSIZE(OnNcCalcSize)
	UI_BEGIN_CHAIN_ALL_MSG_MAP
		UICHAIN_MSG_MAP(SystemScrollBarRender)
	UI_END_CHAIN_ALL_MSG_MAP
	UI_END_MSG_MAP

protected:
	virtual SCROLLBAR_DIRECTION_TYPE GetScrollBarDirType() { return HSCROLLBAR; }
	void    OnSize(UINT nType, int cx, int cy);
	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lprc);
	void    OnBindObjSize(UINT nType, int cx, int cy);

	virtual void UpdateScrollBarVisible()
	{
		float nPos = (float)m_pScrollBar->GetScrollPos();
		float nRange = (float)m_pScrollBar->GetScrollRange();
		float nPage = (float)m_pScrollBar->GetScrollPage();

		if (0 >= nRange-nPage)
		{
			switch (this->m_pScrollBar->GetScrollMgr()->GetScrollBarVisibleType(HSCROLLBAR))
			{
			case SCROLLBAR_VISIBLE_AUTO:
				if (this->m_pScrollBar->IsMySelfVisible())
					this->m_pScrollBar->SetVisible(false, true);
				break;

			case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
				this->m_pBtnThumb->SetVisible(false, true);
				break;
			}
		}
		else
		{
			switch (this->m_pScrollBar->GetScrollMgr()->GetScrollBarVisibleType(HSCROLLBAR))
			{
			case SCROLLBAR_VISIBLE_AUTO:
				if (false == this->m_pScrollBar->IsMySelfVisible())
					this->m_pScrollBar->SetVisible(true, true);
				break;

			case SCROLLBAR_VISIBLE_SHOW_ALWAYS:
				this->m_pBtnThumb->SetVisible(true, true);
				break;
			}
		}
	}
	bool UpdateThumbButtonPos(bool bNeedUpdateThumbButtonSize)
	{
		float nPos = (float)m_pScrollBar->GetScrollPos();
		float nRange = (float)m_pScrollBar->GetScrollRange();
		float nPage = (float)m_pScrollBar->GetScrollPage();

		int nNewSize = 0;
		if (bNeedUpdateThumbButtonSize)
			nNewSize = this->CalcThumbButtonSize();
		else
			nNewSize = this->m_pBtnThumb->GetWidth();

		if (nNewSize < 10)
			nNewSize = 10;

		CRect rcChannel;
		this->CalcChannelRect(&rcChannel);
		if (rcChannel.IsRectEmpty())
			return false;

		int nThumbBtnPos = (int)(nPos / (nRange-nPage) * (float)(rcChannel.Width() - nNewSize)) + rcChannel.left;
		this->m_pBtnThumb->SetObjectPos(nThumbBtnPos, rcChannel.top, nNewSize, rcChannel.Height());
		return true;
	}

	void    CalcChannelRect(CRect* prc)
	{
		m_pScrollBar->GetClientRect(prc);

		if (NULL != m_pBtnLineUpLeft)
			prc->left += m_pBtnLineUpLeft->GetWidth();
		if (NULL != m_pBtnLineDownRight)
			prc->right -= m_pBtnLineDownRight->GetWidth();
	}
	int     CalcThumbButtonSize()
	{
		if (NULL == m_pBtnThumb)
			return -1;

		CRect rcChannel;
		this->CalcChannelRect(&rcChannel);

		float nPage = (float)m_pScrollBar->GetScrollPage();
		float nRange = (float)m_pScrollBar->GetScrollRange();
		if (0 == nRange)
			return -1;

		int nBtnW = (int)((float)rcChannel.Width()*nPage/nRange + 0.5);
		return nBtnW;
	}
};


void SystemScrollBarRender::OnTimer(UINT_PTR idEvent)
{	
	if(idEvent == m_nTimer1IDToScroll)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer1IDToScroll);
		m_nTimer1IDScrolling = TimerHelper::GetInstance()->SetNewTimer(TIMER_TIME_SCROLLING, (Message*)this);
	}
	else if(idEvent == m_nTimer2IDToScroll)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer2IDToScroll);
		m_nTimer2IDScrolling = TimerHelper::GetInstance()->SetNewTimer(TIMER_TIME_SCROLLING, (Message*)this);
	}
	else if(idEvent == m_nTimer1IDScrolling)
	{
		// 检测鼠标是否在其上面
		POINT pt, ptObj;
		::GetCursorPos(&pt);
		::MapWindowPoints(NULL, this->GetBindObject()->GetHWND(), &pt, 1);
		this->m_pScrollBar->WindowPoint2ObjectPoint(&pt, &ptObj);
		if (NULL != m_pBtnLineUpLeft)
		{
			if(HTNOWHERE != UISendMessage(m_pBtnLineUpLeft, UI_WM_HITTEST, ptObj.x, ptObj.y))
			{
				int nOldPos = m_pScrollBar->GetScrollPos();
				this->m_pScrollBar->ScrollLineUpLeft();
				if (nOldPos != m_pScrollBar->GetScrollPos())
				{
					this->GetBindObject()->UpdateObject();
				}
			}
		}
	}
	else if(idEvent == m_nTimer2IDScrolling)
	{
		if (NULL != m_pBtnLineDownRight)
		{
			// 检测鼠标是否在其上面
			POINT pt, ptObj;
			::GetCursorPos(&pt);
			::MapWindowPoints(NULL, this->GetBindObject()->GetHWND(), &pt, 1);
			this->m_pScrollBar->WindowPoint2ObjectPoint(&pt, &ptObj);
			if(HTNOWHERE != UISendMessage(m_pBtnLineDownRight, UI_WM_HITTEST, ptObj.x, ptObj.y))
			{
				int nOldPos = m_pScrollBar->GetScrollPos();
				this->m_pScrollBar->ScrollLineDownRight();
				if (nOldPos != m_pScrollBar->GetScrollPos())
				{
					this->GetBindObject()->UpdateObject();
				}
			}
		}
	}
}


void SystemScrollBarRender::OnStateChanged(int nOld, int nNew)
{

}
void SystemScrollBarRender::OnBtn1LButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if( 0 != m_nTimer1IDToScroll )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer1IDToScroll);
		m_nTimer1IDToScroll = 0;
	}
	if( 0 != m_nTimer1IDScrolling )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer1IDScrolling);
		m_nTimer1IDScrolling = 0;
	}
	m_nTimer1IDToScroll = TimerHelper::GetInstance()->SetNewTimer(TIMER_TIME_TOSCROLL, (Message*)this);
	this->m_pScrollBar->FireScrollMessage(SB_LINEUP);
}

void SystemScrollBarRender::OnBtn2LButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (0 != m_nTimer2IDToScroll)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer2IDToScroll);
	}
	if (0 != m_nTimer2IDScrolling)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer2IDScrolling);
	}
	m_nTimer2IDToScroll = TimerHelper::GetInstance()->SetNewTimer(TIMER_TIME_TOSCROLL, (Message*)this);
	this->m_pScrollBar->FireScrollMessage(SB_LINEDOWN);
}
void SystemScrollBarRender::OnBtn1LButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if( 0 != m_nTimer1IDToScroll )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer1IDToScroll );
	}
	if( 0 != m_nTimer1IDScrolling )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer1IDScrolling);
	}
	this->m_pScrollBar->FireScrollMessage(SB_ENDSCROLL);
}
void SystemScrollBarRender::OnBtn1KillFocus(Object* pNewFocusObj)
{
	SetMsgHandled(FALSE);

	if( 0 != m_nTimer1IDToScroll )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer1IDToScroll );
	}
	if( 0 != m_nTimer1IDScrolling )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer1IDScrolling);
		this->m_pScrollBar->FireScrollMessage(SB_ENDSCROLL);
	}
}

void SystemScrollBarRender::OnBtn2LButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if( 0 != m_nTimer2IDToScroll )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer2IDToScroll);
	}
	if( 0 != m_nTimer2IDScrolling )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer2IDScrolling);
	}
	this->m_pScrollBar->FireScrollMessage(SB_ENDSCROLL);
}
void SystemScrollBarRender::OnBtn2KillFocus(Object* pNewFocusObj)
{
	SetMsgHandled(FALSE);

	if( 0 != m_nTimer2IDToScroll )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer2IDToScroll );
	}
	if( 0 != m_nTimer2IDScrolling )
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimer2IDScrolling);
		this->m_pScrollBar->FireScrollMessage(SB_ENDSCROLL);
	}
}

void ScrollBarBase::FireScrollMessage(int nSBCode, int nThackPos)
{
	UIMSG msg;
	msg.message = m_eScrollDirection==HSCROLLBAR? WM_HSCROLL:WM_VSCROLL;
	msg.code = 0;
	msg.wParam = nSBCode;
	msg.lParam = nThackPos;
	msg.pObjMsgFrom = this;
	msg.pObjMsgTo = this->GetBindObject();
	DoNotify(&msg);
}

void SystemVScrollBarRender::OnBindObjSize(UINT nType, int cx, int cy)
{
	Object* pBindObj = this->GetBindObject();
	if (NULL == pBindObj)
		return;

	int nNonClientV = pBindObj->GetNonClientR() - pBindObj->GetPaddingR();
	int nNonClientH = pBindObj->GetNonClientB() - pBindObj->GetPaddingB();

	if (m_pScrollBar->IsVisible())
		m_pScrollBar->SetObjectPos(cx-nNonClientV, 0, nNonClientV, cy - nNonClientH);
}
void SystemHScrollBarRender::OnBindObjSize(UINT nType, int cx, int cy)
{
	Object* pBindObj = this->GetBindObject();
	if (NULL == pBindObj)
		return;

	int nNonClientV = pBindObj->GetNonClientR() - pBindObj->GetPaddingR();
	int nNonClientH = pBindObj->GetNonClientB() - pBindObj->GetPaddingB();

	if (m_pScrollBar->IsVisible())
		m_pScrollBar->SetObjectPos(0, cy-nNonClientH, cx - nNonClientV, nNonClientH);
}

void SystemHScrollBarRender::OnSize(UINT nType, int cx, int cy)
{
	CRect rcClient;
	this->m_pScrollBar->GetClientRect(&rcClient);

	int nX1 = rcClient.left;
	int nX2 = rcClient.right;
	if (NULL != m_pBtnLineUpLeft)
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineUpLeft->GetWidth();
		s.cy = m_pBtnLineUpLeft->GetHeight();

		if( s.cx == 0 || s.cy == 0 )
			s = m_pBtnLineUpLeft->GetDesiredSize(NULL);

		// TODO: 减去margin
		m_pBtnLineUpLeft->SetObjectPos( 
			rcClient.left,rcClient.top, 
			s.cx, rcClient.Height(), 0);

		nX1 += s.cx;
	}
	if( NULL != m_pBtnLineDownRight )
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineDownRight->GetWidth();
		s.cy = m_pBtnLineDownRight->GetHeight();

		if( s.cx == 0 || s.cy == 0 )
			s = m_pBtnLineDownRight->GetDesiredSize(NULL);

		// TODO: 减去margin
		m_pBtnLineDownRight->SetObjectPos( 
			rcClient.right - s.cx,rcClient.top, 
			s.cx, rcClient.Height(), 0 );

		nX2 -= s.cx;
	}
}

void SystemVScrollBarRender::OnSize(UINT nType, int cx, int cy)
{
	CRect rcClient;
	this->m_pScrollBar->GetClientRect(&rcClient);

	int nY1 = rcClient.top;
	int nY2 = rcClient.bottom;
	if( NULL != m_pBtnLineUpLeft )
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineUpLeft->GetWidth();
		s.cy = m_pBtnLineUpLeft->GetHeight();

		if( s.cx == 0 || s.cy == 0 )
		{
			s = m_pBtnLineUpLeft->GetDesiredSize(NULL);
		}

		// TODO: 减去margin
		m_pBtnLineUpLeft->SetObjectPos( 
			rcClient.left,rcClient.top, 
			rcClient.Width(), s.cy, 0 );
		nY1 += s.cy;
	}
	if( NULL != m_pBtnLineDownRight )
	{
		SIZE s = {0,0};
		s.cx = m_pBtnLineDownRight->GetWidth();
		s.cy = m_pBtnLineDownRight->GetHeight();

		if( s.cx == 0 || s.cy == 0 )
		{
			s = m_pBtnLineDownRight->GetDesiredSize(NULL);
		}

		// TODO: 减去margin
		m_pBtnLineDownRight->SetObjectPos(
			rcClient.left, rcClient.bottom-s.cy,
			rcClient.Width(), s.cy, 0 );
		nY2 -= s.cy;
	}
}
LRESULT SystemHScrollBarRender::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lprc)
{
	if (false == this->m_pScrollBar->IsVisible())
		return 0;

	int nHeight = this->m_pScrollBar->GetHeight();
	if (0 == nHeight)
	{
		SIZE s = this->m_pScrollBar->GetDesiredSize(NULL);
		nHeight = s.cy;
	}
	((RECT*)lprc)->bottom -= nHeight;
	return 0;
}
LRESULT SystemVScrollBarRender::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lprc)
{
	if (false == this->m_pScrollBar->IsVisible())
		return 0;

	int nWidth = this->m_pScrollBar->GetWidth();
	if (0 == nWidth)
	{
		SIZE s = this->m_pScrollBar->GetDesiredSize(NULL);
		nWidth = s.cx;
	}
	((RECT*)lprc)->right -= nWidth;
	return 0;
}

IScrollBarRender* CreateScrollBarRenderInstance(ScrollBarBase* pScrollBar, SCROLLBAR_TYPE eType, SCROLLBAR_DIRECTION_TYPE eScrollDirection)
{
	switch (eType)
	{
	case SCROLLBAR_TYPE_SYSTEM:
		if (HSCROLLBAR == eScrollDirection)
			return new SystemHScrollBarRender(pScrollBar);
		else
			return new SystemVScrollBarRender(pScrollBar);
	}
	return NULL;
}