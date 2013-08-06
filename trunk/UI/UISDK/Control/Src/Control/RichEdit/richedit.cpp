#include "stdafx.h"
#include "richedit.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"

namespace UI
{

RichEdit::RichEdit()
{
    m_pIRichEdit = NULL;
	m_rcInvalidate.SetRectEmpty();
    m_pMgrScrollBar = NULL;
    m_bNeedFixGdiAlpha = false;
}

RichEdit::~RichEdit()
{
    SAFE_DELETE(m_pMgrScrollBar);
}

HRESULT  RichEdit::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pMgrScrollBar = new IScrollBarManager();
	m_wrapRichEidt.BindControl(this);
    m_pMgrScrollBar->SetBindObject(m_pIRichEdit);
    m_pMgrScrollBar->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
    m_pMgrScrollBar->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);

	return S_OK;
}

void  RichEdit::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    IUIApplication*  pUIApplication = m_pIRichEdit->GetUIApplication();

    ITextRenderBase* pTextRender = NULL;
    pMapAttrib->GetAttr_TextRenderBase(XML_TEXTRENDER_TYPE, true, pUIApplication, m_pIRichEdit, &pTextRender);
	if (pTextRender)
	{
        m_pIRichEdit->SetTextRender(pTextRender);
        SAFE_RELEASE(pTextRender);
	}
	else if (NULL == m_pIRichEdit->GetTextRender())
	{
        pUIApplication->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, m_pIRichEdit, &pTextRender);
        if (pTextRender)
        {
            pTextRender->SetAttribute(pMapAttrib, NULL, true);
            m_pIRichEdit->SetTextRender(pTextRender);
            SAFE_RELEASE(pTextRender);
        }
	}

	if (NULL == m_pIRichEdit->GetBkRender() && !m_pIRichEdit->IsTransparent())  // 如果没有指定为透明属性，则默认用窗口背景色绘制背景
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_SYSCOLOR, m_pIRichEdit, &pRenderBase);
        if (pRenderBase)
        {
            ISysColorRender* pRender = (ISysColorRender*)pRenderBase->QueryInterface(uiiidof(ISysColorRender));
            if (pRender)
            {
                pRender->SetBkColor(COLOR_BTNFACE);
                m_pIRichEdit->SetBkgndRender(pRenderBase);
            }
            SAFE_RELEASE(pRenderBase);
        }
	}

    m_pMgrScrollBar->SetAttribute(pMapAttrib, bReload);
	m_pMgrScrollBar->SetVScrollWheel(10);

    if (!m_pMgrScrollBar->GetHScrollBar() && m_pMgrScrollBar->GetScrollBarVisibleType(HSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
    {
        ISystemHScrollBar*  p = NULL;
        ISystemHScrollBar::CreateInstance(pUIApplication, &p);
        p->SetIScrollBarMgr(m_pMgrScrollBar);
        m_pIRichEdit->AddNcChild(p);
        p->SetID(XML_HSCROLLBAR_PRIFIX);

        p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
        p->SetVisible(false, false, false);
    }

    if (!m_pMgrScrollBar->GetVScrollBar() && m_pMgrScrollBar->GetScrollBarVisibleType(VSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
    {
        ISystemVScrollBar*  p = NULL;
        ISystemVScrollBar::CreateInstance(pUIApplication, &p);
        p->SetIScrollBarMgr(m_pMgrScrollBar);
        m_pIRichEdit->AddNcChild(p);
        p->SetID(XML_VSCROLLBAR_PRIFIX);

        p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
        p->SetVisible(false, false, false);
    }
}
void RichEdit::ResetAttribute()
{
	DO_PARENT_PROCESS(IRichEdit, IControl);

    if (m_pMgrScrollBar)
    {
        m_pMgrScrollBar->ResetAttribute();
        m_pMgrScrollBar->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
        m_pMgrScrollBar->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);
    }
}

void RichEdit::OnObjectLoaded()
{
	// 初始化字体(注：如果先创建textservice,再直接设置字体，
	// 调用OnTxPropertyBitsChange会崩溃
	IRenderFont* pFont = m_pIRichEdit->GetRenderFont();
	if (pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		m_wrapRichEidt.SetFont(&lf);
	}

	m_wrapRichEidt.Create(m_pIRichEdit->GetHWND());
}
void RichEdit::OnEraseBkgnd(IRenderTarget*  pRendrTarget)
{
	//SetMsgHandled(FALSE);
    DO_PARENT_PROCESS(IRichEdit, IControl);

    // m_caret不能放在OnEraseBkgnd中，因为RE在txDraw过程中会去隐藏、显示、设置位置等光标操作
    // 另外有可能RE还有自己的背景，会覆盖掉光标，因此将光标还是放在OnPaint中，但需要处理坐标偏移问题

    // 因为m_caret的位置是基于edit的，而不是client
    m_caret.OnControlPaint(pRendrTarget);

}

void RichEdit::OnPaint(IRenderTarget*  pRendrTarget)
{
#if 1
	HDC hDC = pRendrTarget->GetBindHDC();
	m_wrapRichEidt.Draw(hDC);

    CRect  rcWnd;
    m_pIRichEdit->GetObjectVisibleRect(&rcWnd, false);

	if (m_rcInvalidate.IsRectEmpty())
	{
        if (m_bNeedFixGdiAlpha)
		    Util::FixGdiAlpha(hDC, &rcWnd);
	}
	else
	{
        // 注：m_rcInvalidate可能过期了，例如窗口在拖拽中，控件的范围变小了，但m_rcInvalidate
        //     记录的还是上一次的大小。因此在这取一次交集
        CRect rcFix;
        rcFix.IntersectRect(&m_rcInvalidate, &rcWnd);

        if (m_bNeedFixGdiAlpha)
            Util::FixGdiAlpha(hDC, &rcFix);

		m_rcInvalidate.SetRectEmpty();
	}
#else
	HDC hDC = pRendrTarget->GetHDC();
	m_wrapRichEidt.Draw(hDC);
	pRendrTarget->ReleaseHDC(hDC);
#endif
}

// 由WindowlessRichEdit::TxInvalidateRect post过来的消息，延迟刷新
LRESULT RichEdit::OnRedrawObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bRedrawScrollbar = false;
	if (false == m_wrapRichEidt.GetInvalidateRect(&m_rcInvalidate, &bRedrawScrollbar))
		return 0;

    IRenderLayer*  pRenderLayer = m_pIRichEdit->GetRenderLayer();
    HDC hDC = pRenderLayer->GetMemoryLayerDC();
    if (NULL == hDC)
        return 0;

	SaveDC(hDC);   // SetMetaRgn之前必须调用一次，配合restoredc还原metargn

	HRGN hRgn = CreateRectRgnIndirect(&m_rcInvalidate);
	::SelectClipRgn(hDC, hRgn);
	SAFE_DELETE_GDIOBJECT(hRgn);

	// SetMetaRgn使用当前clip rgn做为meta rgn。
	// 因为RedrawObject内部刷新一个对象时会重新设置裁剪区域为object区域，
	// 导致rcInvalidate clip rgn被覆盖。这里借用metargn来完成双重裁剪区域的设置。
	// PS: GDIPLUS貌似不支持MetaRgn

	::SetMetaRgn(hDC);   

	if (m_caret.GetCaretType()==CARET_TYPE_API)
		m_caret.HideCaret();  // 解决系统光标在输入文字时不显示的问题。加上Hide/Show光标即可跟随输入位置

	m_pIRichEdit->UpdateObject();   

	if (m_caret.GetCaretType()==CARET_TYPE_API)
		m_caret.ShowCaret();

	RestoreDC(hDC,-1);     

	if (bRedrawScrollbar)   // 避免每次都刷新滚动条
    {
        if (m_pMgrScrollBar->GetVScrollBar())
            UISendMessage(m_pMgrScrollBar->GetVScrollBar(), UI_WM_REDRAWOBJECT);
        if (m_pMgrScrollBar->GetHScrollBar())
            UISendMessage(m_pMgrScrollBar->GetHScrollBar(), UI_WM_REDRAWOBJECT);
    }

	return 0;
}

// LRESULT RichEdit::OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
void RichEdit::OnSetFocus(IObject* pOldFocusObj)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_SETFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return;
	}
}
void RichEdit::OnKillFocus(IObject* pNewFocusObj)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_KILLFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
	}
}

LRESULT RichEdit::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void RichEdit::OnSize(UINT nType, int cx, int cy)
{
	// 更新滚动条的属性
	CRect rcClient;
	m_pIRichEdit->GetClientRect(&rcClient);

	SIZE sizeContent;
    SIZE sizePage = { rcClient.Width(), rcClient.Height() };

//	rcClient.right = rcClient.left = 0;  // 解决richedit内部，横向滚动条允许滚动到range，而不是range-page的位置
	this->m_pMgrScrollBar->GetScrollRange((int*)&sizeContent.cx, (int*)&sizeContent.cy);
	this->m_pMgrScrollBar->SetScrollPageAndRange(&sizePage, &sizeContent);
}

void RichEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);  // 继续传递给windowless richedit

	if (!m_pIRichEdit->IsFocus())
	{
		IWindowBase* pWindow = m_pIRichEdit->GetWindowObject();
		if (pWindow)
		{
			pWindow->SetFocusObject(m_pIRichEdit);
		}
	}
}

// 因为richedit会自己负责刷新richedit的内容，因些
LRESULT  RichEdit::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nSBCode = LOWORD(wParam);
	int nOldPos = 0;
	if (uMsg==WM_HSCROLL)
		nOldPos = m_pMgrScrollBar->GetHScrollPos();
	else
		nOldPos = m_pMgrScrollBar->GetVScrollPos();

	OnForwardMessage(uMsg, wParam, lParam);

	// 解决当拖拽滑块响应SB_THUMBPOSITION，richedit不会调用txSetScrollPos的问题（只会在SB_ENDSCROLL调用一次）
	if (nSBCode == SB_THUMBTRACK)
	{
		m_pIRichEdit->SetCanRedraw(false);

        UIMSG msg = *GetCurMsg();
        msg.pMsgTo = m_pMgrScrollBar;
        UISendMessage(&msg);

		m_pIRichEdit->SetCanRedraw(true);
	}

	// 刷新滚动条
	int nPos = 0;
	if (uMsg==WM_HSCROLL)
		nPos = m_pMgrScrollBar->GetHScrollPos();
	else
		nPos = m_pMgrScrollBar->GetVScrollPos();

	if (nPos != nOldPos)
	{
		if (uMsg==WM_HSCROLL)
			UISendMessage(m_pMgrScrollBar->GetHScrollBar(), UI_WM_REDRAWOBJECT);
		else
			UISendMessage(m_pMgrScrollBar->GetVScrollBar(), UI_WM_REDRAWOBJECT);
	}

	return 0;
}

// 不使用 mgr scroll提供的ui偏移信息，直接使用richedit内部的偏移，richedit已经实现了该功能
LRESULT RichEdit::OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}


//
//	如果这里不处理的话，windowless richeidt将在收到
//  kill focus时销毁光标，但此时背景已被刷新，再调用DestroyCaret时，反而又会刷上
//  一个光标的形状，导致光标残留。
//
void RichEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
}
void RichEdit::OnVisibleChanged(BOOL bVisible, IObject* pParent)
{
    SetMsgHandled(FALSE);
	OnShowWindow(bVisible, 0);
}

LRESULT  RichEdit::OnWindowLayeredChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool bLayered = wParam?true:false;

    IRenderLayer*  pRenderLayer = m_pIRichEdit->GetRenderLayer();
    if ((pRenderLayer && pRenderLayer->IsTransparent()) || bLayered)
        m_bNeedFixGdiAlpha = true;
    else
        m_bNeedFixGdiAlpha = false;

    return 0;
}

HRESULT  RichEdit::OnTxNotify(DWORD iNotify, void* pv, BOOL& bHandled)
{
    UIMSG  msg;
    msg.message = UI_WM_RICHEDIT_TXNOTIFY;
    msg.wParam = iNotify;
    msg.lParam = (LPARAM)pv;
    
    long lRet = m_pIRichEdit->DoNotify(&msg);
    bHandled = msg.bHandled;
    if (msg.bHandled)
        return lRet;

    return S_OK;
}


}
