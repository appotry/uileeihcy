#include "stdafx.h"

RichEditBase::RichEditBase():m_wrapRichEidt(this)
{
	m_MgrScrollbar.SetBindObject(this);
}

SIZE RichEditBase::GetAutoSize( HRDC hRDC )
{ 
	SIZE  s = {0,0}; 
	return s; 
}
bool RichEditBase::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	if (NULL == m_pBkgndRender)
	{
		m_pBkgndRender = RenderFactory::GetRender(RENDER_TYPE_COLOR, this);
		ColorRender* pRender = dynamic_cast<ColorRender*>(m_pBkgndRender);
		if (NULL != pRender)
		{
			pRender->SetBkColor(GetSysColor(COLOR_BTNFACE));
		}
	}

	m_MgrScrollbar.SetAttribute(m_mapAttribute, bReload);
	if (NULL != m_MgrScrollbar.GetVScrollBar())
	{
		m_MgrScrollbar.GetVScrollBar()->SetScrollWheelLine(10);
	}
	return true;
}
void RichEditBase::ResetAttribute()
{
	__super::ResetAttribute();
	m_MgrScrollbar.ResetAttribute();
}
void RichEditBase::OnObjectLoaded()
{
	// 初始化字体(注：如果先创建textservice,再直接设置字体，
	// 调用OnTxPropertyBitsChange会崩溃
	IRenderFont* pFont = this->GetFont();
	if (NULL != pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		m_wrapRichEidt.SetFont(&lf);
	}

	m_wrapRichEidt.Create(GetHWND());
}
void RichEditBase::OnEraseBkgnd( HRDC hRDC )
{
	SetMsgHandled(FALSE);
}
void RichEditBase::OnPaint( HRDC hRDC )
{
	HDC hDC = GetHDC(hRDC);
	m_wrapRichEidt.Draw(hDC);
	ReleaseHDC(hRDC, hDC);
}
// LRESULT RichEditBase::OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
// 	LRESULT lRet = 0;
// 	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
// 	if (!bRet)
// 	{
// 		SetMsgHandled(FALSE);
// 		return 0;
// 	}
// 	return lRet;
// }
void RichEditBase::OnSetFocus( Object* pOldFocusObj )
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_SETFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return;
	}
}
void RichEditBase::OnKillFocus( Object* pNewFocusObj )
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_KILLFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
	}
}

LRESULT RichEditBase::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return 0;
	}
	return lRet;
}

void RichEditBase::OnSize(UINT nType, int cx, int cy)
{
	// 更新滚动条的属性
	CRect rcClient;
	this->GetClientRect(&rcClient);

	SIZE sizeContent;

	this->m_MgrScrollbar.GetScrollRange((int*)&sizeContent.cx, (int*)&sizeContent.cy);
	this->m_MgrScrollbar.OnBindObjectSize(&sizeContent, &rcClient, m_pCurMsg);
}


// 因为richedit会自己负责刷新richedit的内容，因些
LRESULT  RichEditBase::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nSBCode = LOWORD(wParam);
	int nOldPos = m_MgrScrollbar.GetVScrollPos();

	OnForwardMessage(uMsg, wParam, lParam);

	// 解决当拖拽滑块响应SB_THUMBPOSITION，richedit不会调用txSetScrollPos的问题（只会在SB_ENDSCROLL调用一次）
	if (nSBCode == SB_THUMBTRACK)
	{
		this->SetReDraw(false);
		m_MgrScrollbar.ProcessMessage(m_pCurMsg);
		this->SetReDraw(true);
	}

	// 刷新滚动条
	int nPos = m_MgrScrollbar.GetVScrollPos();
	if (nPos != nOldPos)
	{
		m_MgrScrollbar.GetVScrollBar()->UpdateObject();
	}

	return 0;
}

// 不使用 mgr scroll提供的ui偏移信息，直接使用richedit内部的偏移，richedit已经实现了该功能
LRESULT RichEditBase::OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}