#include "stdafx.h"
#include "listview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Control\Src\Control\ListView\layout\listviewitemreportlayout.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"

namespace UI
{

ListView::ListView()
{
	m_pHeaderCtrl = NULL;
    m_pIListView = NULL;
}

HRESULT  ListView::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IListView, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

	m_pIListView->ModifyStyle(LISTCTRLBASE_STYLE_MULTIPLE_SEL, 0, false);

    m_pIListView->SetLayout(ListViewItemReportLayout::CreateInstance());

    IHeaderListCtrl::CreateInstance(p, &m_pHeaderCtrl);
	if (m_pHeaderCtrl)
	{
		m_pIListView->AddNcChild(m_pHeaderCtrl);
		m_pHeaderCtrl->SetNotify(m_pIListView, 0);
	}
	return S_OK;
}
ListView::~ListView()
{
	m_pHeaderCtrl = NULL;
}


void ListView::ResetAttribute()
{
	DO_PARENT_PROCESS(IListView, IListCtrlBase);
	
    if (m_pHeaderCtrl)
	{
		UISendMessage(m_pHeaderCtrl, UI_WM_RESETATTRIBUTE);
	}
	m_pIListView->SetNonClientRegionExcludePaddingBorderT(0);
}

void  ListView::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    IUIApplication*  pUIApp = m_pIListView->GetUIApplication();
    const TCHAR*  szText = pMapAttr->GetAttr(XML_TEXTRENDER_TYPE, true);
	if (NULL == szText && NULL == m_pIListView->GetTextRender())
	{
        ITextRenderBase*  pTextRenderBase = NULL;
        pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_THEME_LISTVIEW, m_pIListView, &pTextRenderBase);
        if (pTextRenderBase)
        {
            pTextRenderBase->SetAttribute(pMapAttr, NULL, true);
            pTextRenderBase->SetTextAlignment(DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
            m_pIListView->SetTextRender(pTextRenderBase);
            SAFE_RELEASE(pTextRenderBase);
        }
    }

    // 当使用theme主题并且没有定义border时，将border默认设置为2
    bool bBorderConfiged = false;
    if (pMapAttr->GetAttr(XML_BORDER, false))
        bBorderConfiged = true;

	DO_PARENT_PROCESS(IListView, IListCtrlBase);

	if (NULL == m_pIListView->GetBkRender())
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTBOX_BKGND, m_pIListView, &pRenderBase);
		if (pRenderBase)
        {
            pRenderBase->SetAttribute(pMapAttr, XML_BACKGND_RENDER_PREFIX, true);
            m_pIListView->SetBkgndRender(pRenderBase);

            if (!bBorderConfiged)
            {
                CRegion4 r(2,2,2,2);
                m_pIListView->SetBorderRegion(&r);
            }

            SAFE_RELEASE(pRenderBase);
        }
	}

	if (NULL == m_pIListView->GetForeRender())
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTVIEW_ITEM, m_pIListView, &pRenderBase);
        if (pRenderBase)
        {
            pRenderBase->SetAttribute(pMapAttr, XML_BACKGND_RENDER_PREFIX, true);
            m_pIListView->SetForegndRender(pRenderBase);
            SAFE_RELEASE(pRenderBase);
        }
	}

	if (NULL == m_pIListView->GetFocusRender())
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTVIEW_FOCUS, m_pIListView, &pRenderBase);
        if (pRenderBase)
        {
            m_pIListView->SetFocusRender(pRenderBase);
            SAFE_RELEASE(pRenderBase);
        }
	}

    IScrollBarManager*  pScrollBarMgr = m_pIListView->GetIScrollBarMgr();
    if (pScrollBarMgr)
    {
        if (!pScrollBarMgr->GetHScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(HSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemHScrollBar*  p = NULL;
            ISystemHScrollBar::CreateInstance(pUIApp, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListView->AddNcChild(p);
            p->SetID(XML_HSCROLLBAR_PRIFIX);

            p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttr, bReload, true);
            p->SetVisible(false, false, false);
        }

        if (!pScrollBarMgr->GetVScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(VSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemVScrollBar*  p = NULL;
            ISystemVScrollBar::CreateInstance(pUIApp, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListView->AddNcChild(p);
            p->SetID(XML_VSCROLLBAR_PRIFIX);

            p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttr, bReload, true);
            p->SetVisible(false, false, false);
        }
    }

    if (m_pHeaderCtrl)
    {
        m_pHeaderCtrl->SetAttributeByPrefix(XML_LISTVIEWCTRL_HEADERCTRL_PREFIX, pMapAttr, bReload, true);
    }
	m_pIListView->UpdateObjectNonClientRegion();
}

void  ListView::GetAllSubItemRect(ListViewItemBase* pItem, CRect* prcArray)
{
	if (NULL == pItem || NULL == prcArray)
		return;
#if 0 // TODO:
	int  nColumnCount = m_pHeaderCtrl->GetColumnCount();
	if (0 == nColumnCount)
		return;
	
	int* parrWidth = new int[nColumnCount];
	m_pHeaderCtrl->GetColumnWidthArray(parrWidth);

	CRect rcItem;
	pItem->GetParentRect(&rcItem);

	int nLastLeft = 0;
	for (int i = 0; i < nColumnCount; i++)
	{
		prcArray[i].left = nLastLeft;
		prcArray[i].top = rcItem.top;
		prcArray[i].bottom = rcItem.bottom;
		nLastLeft += parrWidth[i];
		prcArray[i].right = nLastLeft;
	}
	SAFE_ARRAY_DELETE(parrWidth);
#endif
}
#if 0 
void ListView::OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p)
{
	ListViewItemBase* pItem = (ListViewItemBase*)p;
	if (NULL == pRenderTarget || NULL == pItem)
		return;

	CRect rcItem;
	p->GetParentRect(&rcItem);

	bool bFocus = m_pIListView->IsFocus();
	UINT nRenderState = p->GetItemRenderState();
	if (m_pIListView->GetForeRender())
		m_pIListView->GetForeRender()->DrawState(pRenderTarget, &rcItem, nRenderState);

	if (bFocus && m_pFocusItem == pItem && m_pFocusRender)
		m_pFocusRender->DrawState(pRenderTarget, &rcItem, 0);

	int  nColumnCount = m_pHeaderCtrl->GetColumnCount();
	if (0 == nColumnCount)
		return;

	// 获取每个item的RECT
	CRect* prcArray = new CRect[nColumnCount];
	this->GetAllSubItemRect(pItem, prcArray);

	// 绘制各个子列
	ListViewSubItemBase* pSubItem = pItem->GetFirstSubItem();
	while (pSubItem)
	{
		ListViewSubItemBase* pNext = pSubItem->GetNextColumnItem();  // Save

		int nColumnIndex = pSubItem->GetColumnIndex();
		if (prcArray[nColumnIndex].Width() > 0)
		{
			this->OnDrawSubItem(pRenderTarget, pSubItem, &prcArray[nColumnIndex], nRenderState);
		}
		pSubItem = pNext;
	}

	SAFE_ARRAY_DELETE(prcArray);

}

void ListView::OnDrawSubItem(IRenderTarget* pRenderTarget, ListViewSubItemBase* p, const CRect* prcItem, int nRenderState)
{
	CRect rc(prcItem);
	if (m_pIListView->GetTextRender())
	{
		rc.left += m_nTextIndentLeft;
		rc.right -= m_nTextIndentRight;
		m_pIListView->GetTextRender()->DrawState(pRenderTarget, &rc, nRenderState, p->GetText());
	}
}

SIZE ListView::OnMeasureItem(ListItemBase* p)
{
	SIZE s = {0, 20};
	return s;
}
#endif
void ListView::OnSize(UINT nType, int cx, int cy)
{
	SetMsgHandled(FALSE);
	this->UpdateHeaderCtrlPos();
}

void ListView::UpdateHeaderCtrlPos()
{
	if (m_pHeaderCtrl)
	{
		int nHeaderCtrlHeight = m_pHeaderCtrl->GetHeight();
		if (0 == nHeaderCtrlHeight)
		{
			nHeaderCtrlHeight = 24;//m_pHeaderCtrl->GetDesiredSize().cy;
		}
		CRegion4 r;
		m_pIListView->GetNonClientRegion(&r);

		m_pHeaderCtrl->SetObjectPos(
			m_pHeaderCtrl->GetMarginL() + m_pIListView->GetBorderL(),
			m_pHeaderCtrl->GetMarginT() + m_pIListView->GetBorderT(), 
			m_pIListView->GetWidth()-m_pIListView->GetNonClientW(), nHeaderCtrlHeight, SWP_NOREDRAW);

        // TODO: 在这里设置一次是为了解决listview第一次显示时，headerctrl没有通知listview totalwidth的问题
        //       关键是headerctrl不知道第一次如何去通知。外部插入一列后，不一定就会updateitemrect
        ListViewItemReportLayout* pReportLayout = static_cast<ListViewItemReportLayout*>(m_pIListView->GetLayout());
        if (pReportLayout)
        {
            pReportLayout->SetHeaderCtrlWidth(m_pHeaderCtrl->GetColumnTotalWidth());
        }
	}
}
void ListView::OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    IScrollBarManager*  pScrollBarMgr = m_pIListView->GetIScrollBarMgr();
    pScrollBarMgr->ProcessMessage(GetCurMsg());

    if (m_pHeaderCtrl)
    {
        m_pHeaderCtrl->SetHScrollOffset(pScrollBarMgr->GetHScrollPos());
        m_pHeaderCtrl->UpdateObject();
    }
}

// 如果是纵向滚动条不可见，横向滚动条可见的情况下，使用mousewheel则是横向滚动，这个时候需要更新header偏移
BOOL  ListView::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
    IScrollBarManager*  pScrollBarMgr = m_pIListView->GetIScrollBarMgr();
    
    int nPosSave = pScrollBarMgr->GetHScrollPos();
    DO_PARENT_PROCESS(IListView, IListCtrlBase);
    int nPos = pScrollBarMgr->GetHScrollPos();

    if (nPosSave != nPos)
    {
        m_pHeaderCtrl->SetHScrollOffset(nPos);
        m_pHeaderCtrl->UpdateObject();
    }
    return TRUE;
}

BOOL ListView::OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth)
{
	return TRUE;
}

void ListView::OnHeaderCtrlTotalWidthChanged(int nTotalWidth)
{ 
    ListViewItemReportLayout* pReportLayout = static_cast<ListViewItemReportLayout*>(m_pIListView->GetLayout());
	if (pReportLayout)
	{
		pReportLayout->SetHeaderCtrlWidth(nTotalWidth);
	}

	m_pIListView->UpdateItemRect();
	m_pHeaderCtrl->SetHScrollOffset(m_pIListView->GetIScrollBarMgr()->GetHScrollPos());
	m_pHeaderCtrl->UpdateObject();
}

LRESULT  ListView::OnGetColumnsInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    IColumnsInfo* pInfo = NULL;

    if (m_pHeaderCtrl)
        pInfo = m_pHeaderCtrl->GetColumnsInfo();
    return (LRESULT)pInfo;
}

bool ListView::SetItemText(int nItem, int nSubItem, const TCHAR* szText, bool bUpdate)
{
	if (nItem >= m_pIListView->GetItemCount())
		return false;

	if (nSubItem >= GetColumnCount())
		return false;

	IListItemBase* pItem = m_pIListView->GetItemByPos(nItem);
	if (NULL == pItem)
		return false;

    UISendMessage(pItem, UI_LVIM_SETSUBITEMTEXT, (WPARAM)nSubItem, (LPARAM)szText);

	if (bUpdate)
		m_pIListView->UpdateObject();
	
	return true;
}

int  ListView::GetColumnCount()
{
	if (NULL == m_pHeaderCtrl)
		return 0;
	
	return m_pHeaderCtrl->GetColumnCount();
}	

IListViewItemBase*  ListView::AddTextItem(const TCHAR* szText)
{
    IListViewItemBase* pItem = NULL;
    IListViewItemBase::CreateInstance(m_pIListView->GetUIApplication(), &pItem);

    m_pIListView->AddItem(pItem);
    if (szText)
    {
        pItem->SetText(szText);
    }
    return pItem;
}
IListViewItemBase*  ListView::InsertTextItem(int nIndex, const TCHAR* szText)
{
    IListViewItemBase* pItem = NULL;
    IListViewItemBase::CreateInstance(m_pIListView->GetUIApplication(), &pItem);

    m_pIListView->InsertItem(pItem, nIndex);
    if (szText)
    {
        pItem->SetText(szText);
    }
    return pItem;
}

}