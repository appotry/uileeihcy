#include "stdafx.h"
#include "UISDK\Control\Src\Control\ListTreeView\listtreeview.h"

namespace UI
{

ListTreeView::ListTreeView()
{
    m_pIListTreeView = NULL;
}
ListTreeView::~ListTreeView()
{

}

}
#if 0
ListTreeViewItem::ListTreeViewItem(ListTreeViewCtrlBase* pCtrl) : TreeItemBase(pCtrl)
{
	m_pFirstSubItem = NULL;	
}
ListTreeViewItem::~ListTreeViewItem()
{
	ListViewSubItemBase* p = m_pFirstSubItem;
	while (p)
	{
		ListViewSubItemBase* pNext = p->GetNextColumnItem();  // Save
//		this->OnDeleteItem(p);
		delete  p;
		p = pNext;
	}
	m_pFirstSubItem = NULL;
}

ListViewSubItemBase*  ListTreeViewItem::GetFirstSubItem()
{
	return m_pFirstSubItem;
}

ListViewSubItemBase* ListTreeViewItem::GetLastSubItem()
{
	ListViewSubItemBase* p = m_pFirstSubItem;
	while (p)
	{
		ListViewSubItemBase* pNext = p->GetNextColumnItem();  // Save
		if (NULL == pNext)
			break;
		p = pNext;
	}
	return p;
}

void ListTreeViewItem::AddColumnItem(ListViewSubItemBase* pSubItem)
{
	ListViewSubItemBase* pLastColumnItem = this->GetLastSubItem();
	if (NULL == pLastColumnItem)
	{
		m_pFirstSubItem = pSubItem;
	}
	else
	{
		pLastColumnItem->SetNextColumnItem(pSubItem);
		pSubItem->SetPrevColumnItem(pLastColumnItem);
	}
}


//////////////////////////////////////////////////////////////////////////

ListTreeViewCtrlBase::ListTreeViewCtrlBase()
{
	m_pHeaderCtrl = NULL;
	SetItemLayoutType(LISTCTRL_ITEM_LAYOUT_TYPE_3);
}
HRESULT ListTreeViewCtrlBase::InitialConstruct()
{
	__super::InitialConstruct();

	this->ModifyStyle(LISTCTRLBASE_STYLE_MULTIPLE_SEL, 0, false);

	UICreateInstance(&m_pHeaderCtrl, m_pUIApplication);
	if (NULL != m_pHeaderCtrl)
	{
		this->AddNcChild(m_pHeaderCtrl);
		m_pHeaderCtrl->SetNotify(this);
	}
	return S_OK;
}
ListTreeViewCtrlBase::~ListTreeViewCtrlBase()
{
	m_pHeaderCtrl = NULL;
}


void ListTreeViewCtrlBase::ResetAttribute()
{
	__super::ResetAttribute();
	if (NULL != m_pHeaderCtrl)
	{
		m_pHeaderCtrl->ResetAttribute();
	}
	m_nTextIndentLeft = m_nTextIndentRight = 5;

	this->SetNonClientRegionExcludePaddingBorderT(0);
}
bool ListTreeViewCtrlBase::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	if (false == __super::SetAttribute(mapAttrib, bReload))
		return false;

	if (NULL != m_pHeaderCtrl)
	{
		this->SetChildObjectAttribute(m_pHeaderCtrl, XML_LISTVIEWCTRL_HEADERCTRL_PREFIX, m_mapAttribute, bReload);
	}
	UpdateObjectNonClientRegion();
	return true;
}

void  ListTreeViewCtrlBase::GetSubItemRect(UIHTREEITEM hTreeItem, int nSubItem, CRect* prcOut)
{
	if (NULL == hTreeItem || NULL == prcOut)
		return;

	int  nColumnCount = m_pHeaderCtrl->GetColumnCount();
	if (nSubItem >= nColumnCount)
		return;

	int xLeft = 0, xRight = 0;
	m_pHeaderCtrl->GetColumnLeftRight(nSubItem, &xLeft, &xRight);

	CRect rcItem;
	hTreeItem->GetParentRect(&rcItem);

	prcOut->SetRect(xLeft, rcItem.top, xRight, rcItem.bottom);
}

void  ListTreeViewCtrlBase::GetAllSubItemRect(ListTreeViewItem* pItem, CRect* prcArray)
{
	if (NULL == pItem || NULL == prcArray)
		return;

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
}

void ListTreeViewCtrlBase::OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p)
{
	if (NULL == p)
		return;

	ListTreeViewItem* pItem = (ListTreeViewItem*)p;

	CRect rcItem;
	p->GetParentRect(&rcItem);

	bool bFocus = this->IsFocus();
	UINT nRenderState = __super::GetItemRenderState(p);
	
	if (NULL != m_pForegndRender)
		m_pForegndRender->DrawState(pRenderTarget, &rcItem, nRenderState);

	if (bFocus && m_pFocusItem == pItem && NULL != m_pFocusRender)
		m_pFocusRender->DrawState(pRenderTarget, &rcItem, 0);

	int  nColumnCount = m_pHeaderCtrl->GetColumnCount();
	if (0 == nColumnCount)
		return;

	// 获取每个item的RECT
	CRect* prcArray = new CRect[nColumnCount];
	this->GetAllSubItemRect(pItem, prcArray);

	// 绘制第一列
	OnDrawMainItem(pRenderTarget, pItem, &prcArray[0], nRenderState);

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

void ListTreeViewCtrlBase::OnDrawMainItem(IRenderTarget* pRenderTarget, ListTreeViewItem* pItem, const CRect* prcItem, int nRenderState)
{
	CRect rcItem(prcItem);

	// 绘制层级线条
	if (this->TestStyle(TREE_STYLE_HASLINE))
		this->OnDrawItemLine(pRenderTarget, pItem, &rcItem);

	rcItem.left += m_nChildNodeIndent*pItem->GetDepth();

	// 绘制展开收缩图标
	if (0 != m_nExpandIconWidth)
	{
		CRect rcExpandIcon = rcItem;
		rcExpandIcon.right = rcItem.left + m_nExpandIconWidth;

		// 如果创建了panelroot，则表明创建了UI CheckButton
		if (NULL == pItem->m_pPanelRoot && NULL != pItem->GetChildItem() && NULL != m_pExpandIcon)
		{
			if (pItem->IsExpand())
				m_pExpandIcon->DrawState(pRenderTarget, &rcExpandIcon, TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_NORMAL);
			else
				m_pExpandIcon->DrawState(pRenderTarget, &rcExpandIcon, TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_NORMAL);
		}

		rcItem.left = rcExpandIcon.right;
	}

	// 绘制图片
	if (NULL != pItem->GetIconRender() && m_nImageWidth > 0)
	{
		CRect rcImage(rcItem);
		rcImage.right = rcImage.left + m_nImageWidth;

		pItem->GetIconRender()->DrawState(pRenderTarget, &rcImage, 0);

		rcItem.left += m_nImageWidth;
	}

	// 绘制文本
	if (NULL != m_pTextRender)
	{
		rcItem.left += m_nTextIndentLeft;
		rcItem.right -= m_nTextIndentRight;

		m_pTextRender->DrawState(pRenderTarget, &rcItem, nRenderState, pItem->GetText());
	}
}
void ListTreeViewCtrlBase::OnDrawSubItem(IRenderTarget* pRenderTarget, ListViewSubItemBase* p, const CRect* prcItem, int nRenderState)
{
	CRect rc(prcItem);
	if (NULL != m_pTextRender)
	{
		rc.left += m_nTextIndentLeft;
		rc.right -= m_nTextIndentRight;
		m_pTextRender->DrawState(pRenderTarget, &rc, nRenderState, p->GetText());
	}
}

SIZE ListTreeViewCtrlBase::OnMeasureItem(ListItemBase* p)
{
	SIZE s = {0, 20};
	return s;
}

void ListTreeViewCtrlBase::OnSize(UINT nType, int cx, int cy)
{
	SetMsgHandled(FALSE);
	this->UpdateHeaderCtrlPos();
}

void ListTreeViewCtrlBase::UpdateHeaderCtrlPos()
{
	if (NULL != m_pHeaderCtrl)
	{
		int nHeaderCtrlHeight = m_pHeaderCtrl->GetHeight();
		if (0 == nHeaderCtrlHeight)
		{
			nHeaderCtrlHeight = m_pHeaderCtrl->GetDesiredSize().cy;
		}
		CRegion4 r;
		this->GetNonClientRegion(&r);

		m_pHeaderCtrl->SetObjectPos(
			m_pHeaderCtrl->GetMarginL() + this->GetBorderL(),
			m_pHeaderCtrl->GetMarginT() + this->GetBorderT(), 
			this->GetWidth()-this->GetNonClientW(), nHeaderCtrlHeight, SWP_NOREDRAW);
	}
}
void ListTreeViewCtrlBase::OnHScroll(int nSBCode, int nPos, Message* pMsgFrom)
{
	m_MgrScrollbar.ProcessMessage(m_pCurMsg);
	m_pHeaderCtrl->SetHScrollOffset(m_MgrScrollbar.GetHScrollPos());
	m_pHeaderCtrl->UpdateObject();
}

BOOL ListTreeViewCtrlBase::OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth)
{

	return TRUE;
}

void ListTreeViewCtrlBase::OnHeaderCtrlTotalWidthChanged(int nTotalWidth)
{
	if (NULL != m_pItemLayout)
	{
		ListCtrlItemLayout3* pLayout = dynamic_cast<ListCtrlItemLayout3*>(m_pItemLayout);
		if (NULL != pLayout)
		{
			pLayout->SetHeaderCtrlWidth(m_pHeaderCtrl->GetColumnTotalWidth());
		}
	}

	this->UpdateItemRect();
	m_pHeaderCtrl->SetHScrollOffset(m_MgrScrollbar.GetHScrollPos());
	m_pHeaderCtrl->UpdateObject();
}

bool ListTreeViewCtrlBase::InsertColumn(int nColumn, const TCHAR* szText, int nWidth, int nFormat)
{
	if (NULL == m_pHeaderCtrl)
	{
		UI_LOG_WARN(_T("%s m_pHeaderCtrl is Null."), FUNC_NAME);
		return false;
	}

	return m_pHeaderCtrl->InsertColumn(nColumn, szText, nWidth, nFormat);
}

bool ListTreeViewCtrlBase::SetItemText(UIHTREEITEM hTreeItem, int nSubItem, const TCHAR* szText, bool bUpdate)
{
	if (nSubItem >= GetColumnCount())
		return false;

	ListTreeViewItem* pItem = (ListTreeViewItem*)hTreeItem;
	if (NULL == pItem)
		return false;

	ListViewSubItemBase* pSubItem = this->GetSubItem(pItem, nSubItem);
	if (NULL == pSubItem)
	{
		pSubItem = new ListViewSubItemBase;
		pSubItem->SetColumnIndex(nSubItem);
		pItem->AddColumnItem(pSubItem);
	}
	pSubItem->SetText(szText);

	if (bUpdate)
		UpdateObject();
	
	return true;
}

ListViewSubItemBase* ListTreeViewCtrlBase::GetSubItem(int nRow, int nCol)
{
	ListTreeViewItem* pItem = (ListTreeViewItem*)GetItemBase(nRow);
	if (NULL == pItem)
		return NULL;

	return this->GetSubItem(pItem, nCol);
}
ListViewSubItemBase* ListTreeViewCtrlBase::GetSubItem(ListTreeViewItem* pItem, int nCol)
{
	if (NULL == pItem)
		return NULL;

	ListViewSubItemBase* pSubItem = pItem->GetFirstSubItem();
	while (pSubItem)
	{
		if (nCol == pSubItem->GetColumnIndex())
			return pSubItem;

		pSubItem = pSubItem->GetNextColumnItem();
	}
	return NULL;
}

int  ListTreeViewCtrlBase::GetColumnCount()
{
	if (NULL == m_pHeaderCtrl)
		return 0;

	return m_pHeaderCtrl->GetColumnCount();
}	

#endif
