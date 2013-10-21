#include "stdafx.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\propertyctrl.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"

#define PROPERTY_EDIT_PREFIX   _T("edit.")

namespace UI
{
PropertyCtrl::PropertyCtrl()
{
    m_nSplitterLinePercent = 512;
    m_nSplitterLinePos = 100;

    m_pEdit = NULL;
    m_pButton = NULL;
    m_pComboBox = NULL;
}
PropertyCtrl::~PropertyCtrl()
{
    
}

HRESULT  PropertyCtrl::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = __super::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    m_pIPropertyCtrl->SetLayoutDefaultV();

    IScrollBarManager*  pScrollBarMgr = m_pIPropertyCtrl->GetIScrollBarMgr();
    if (pScrollBarMgr)
    {
        pScrollBarMgr->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);
        pScrollBarMgr->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
    }

    if (NULL == m_pEdit)
    {
        IEdit::CreateInstance(p, &m_pEdit);
        m_pEdit->SetVisible(false, false, false);
        m_pEdit->SetOutRef((void**)&m_pEdit);
        // m_pIPropertyCtrl->AddChild(m_pEdit); 不做为控件的子对象，而是item的子对象，要不然刷新不好处理
        
        IRenderBase* pBkRender = NULL;
        p->CreateRenderBase(RENDER_TYPE_COLOR, m_pEdit, &pBkRender);
        if (pBkRender)
        {
            IColorRender* pColorRender = (IColorRender*)pBkRender->QueryInterface(uiiidof(IColorRender));
            if (pColorRender)
                pColorRender->SetBkColor(0xFFFFFFFF);

            m_pEdit->SetBkgndRender(pBkRender);
            SAFE_RELEASE(pBkRender);
        }
    }

    return S_OK;
}

void  PropertyCtrl::FinalRelease()
{
    DO_PARENT_PROCESS(IPropertyCtrl, ITreeView);
    SAFE_DELETE_Ixxx(m_pEdit);
}

void  PropertyCtrl::ResetAttribute()
{
    __super::ResetAttribute();

    if (m_pEdit)
    {
        UISendMessage(m_pEdit, UI_WM_RESETATTRIBUTE);
    }
    m_pIPropertyCtrl->ModifyStyle(OBJECT_STYLE_ENABLE_IME, 0, false);
}
void  PropertyCtrl::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    __super::SetAttribute(pMapAttr, bReload);

    if (m_pEdit)
    {
        // 为了能够让edit也拿到window的默认字体，这里先将其加入子结点，否则edit将拿到系统字体，
        // 而PropertyCtrl则拿到窗口字体，导致显示不一致
        m_pEdit->RemoveMeInTheTree();
        m_pIPropertyCtrl->AddChild(m_pEdit);  
        m_pEdit->SetAttributeByPrefix(PROPERTY_EDIT_PREFIX, pMapAttr, bReload, true);
        m_pEdit->RemoveMeInTheTree();
    }
}

IPropertyCtrlGroupItem*  PropertyCtrl::InsertGroupItem(
                                            const TCHAR* szName, 
                                            const TCHAR* szDesc, 
                                            IListItemBase* pParent, 
                                            IListItemBase* pInsertAfter, 
                                            LISTITEM_OPFLAGS nInsertFlags)
{
    IPropertyCtrlGroupItem*  pItem = NULL;
    IPropertyCtrlGroupItem::CreateInstance(m_pIPropertyCtrl->GetUIApplication(), &pItem);

    if (false == m_pIPropertyCtrl->InsertItem(pItem, pParent, pInsertAfter, nInsertFlags))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

    pItem->SetText(szName);
    pItem->SetToolTip(szDesc);

    return pItem;
}

IPropertyCtrlEditItem*  PropertyCtrl::InsertEditProperty(
                                            const TCHAR* szKey, 
                                            const TCHAR* szValue, 
                                            const TCHAR* szDesc, 
                                            IListItemBase* pParentItem, 
                                            IListItemBase* pInsertAfter, 
                                            LISTITEM_OPFLAGS nInsertFlags)
{
    // TODO: 先查找该key是否已经存在
    if (pParentItem)
    {

    }
    else
    {

    }


    IPropertyCtrlEditItem*  pItem = NULL;
    IPropertyCtrlEditItem::CreateInstance(m_pIPropertyCtrl->GetUIApplication(), &pItem);

    pItem->SetText(szKey);
    pItem->SetToolTip(szDesc);
    pItem->SetValueText(szValue);

    if (false == m_pIPropertyCtrl->InsertItem(pItem, pParentItem, pInsertAfter, nInsertFlags))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

    return pItem;
}

void  PropertyCtrl::OnSize(UINT nType, int cx, int cy)
{
    m_nSplitterLinePos = cx * m_nSplitterLinePercent >> 10;
    SetMsgHandled(FALSE);
}

LRESULT  PropertyCtrl::OnGetSplitterLinePosPtr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)&m_nSplitterLinePos;
}
LRESULT  PropertyCtrl::OnGetEditCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_pEdit;
}
LRESULT  PropertyCtrl::OnPreShowTooltip(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 1;
}

}
#if 0
//////////////////////////////////////////////////////////////////////////

PropertyCtrl::PropertyCtrl()
{
	m_pGroupPropTextRender = NULL;
	m_pPropValueTextRender = NULL;
	m_pGroupPropForegndRender = NULL;
	m_pColorGrid = NULL;
}

PropertyCtrl::~PropertyCtrl()
{
	SAFE_RELEASE(m_pGroupPropTextRender);
	SAFE_RELEASE(m_pPropValueTextRender);
	SAFE_RELEASE(m_pGroupPropForegndRender);
	SAFE_RELEASE(m_pColorGrid);
}
HRESULT PropertyCtrl::InitialConstruct()
{
	HRESULT hr = __super::InitialConstruct();
	if (FAILED(hr))
		return hr;

	this->ModifyStyle(0, LISTCTRLBASE_STYLE_MULTIPLE_SEL, false);

	this->GetEditControl();
	this->GetComboboxControl();
	this->GetButtonControl();

	m_pEdit->SetVisible(false, false, false);
	m_pButton->SetVisible(false, false, false);
	m_pCombobox->SetVisible(false, false, false);

	this->InsertColumn(PROP_KEY_COL, _T("Key"), 100);
	this->InsertColumn(PROP_VALUE_COL, _T("Value"), 100);

	return S_OK;
}

void PropertyCtrl::ResetAttribute()
{
	__super::ResetAttribute();
	m_nChildNodeIndent = 0;
	SAFE_RELEASE(m_pGroupPropTextRender);
	SAFE_RELEASE(m_pPropValueTextRender);
	SAFE_RELEASE(m_pGroupPropForegndRender);
	SAFE_RELEASE(m_pColorGrid);

	ColorRender* pRender = (ColorRender*)RenderFactory::GetRender(RENDER_TYPE_COLOR, m_pEdit);
	pRender->SetBkColor(RGB(255,255,255));
	if (NULL != m_pEdit)
	{
		m_pEdit->SetBkgndRender(pRender);
		CRegion4 r(0,0,0,0);
		m_pEdit->SetPaddingRegion(&r);
	}
	if (NULL != m_pCombobox)
		m_pCombobox->SetBkgndRender(pRender);
	SAFE_RELEASE(pRender);

	m_MgrScrollbar.SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
}

bool PropertyCtrl::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	ATTRMAP::iterator iter = mapAttrib.find(XML_FOREGND_RENDER_PREFIX XML_RENDER_TYPE);
	if (iter == mapAttrib.end())
	{
		ColorListRender* pRender = (ColorListRender*)RenderFactory::GetRender(RENDER_TYPE_COLORLIST, this);
		pRender->SetCount(9);
		pRender->SetStateColor(4, RGB(51,153,255), true, 0, false);
		pRender->SetStateColor(5, RGB(51,153,255), true, 0, false);
		pRender->SetStateColor(6, RGB(51,153,255), true, 0, false);
		pRender->SetStateColor(7, RGB(51,153,255), true, 0, false);
		pRender->SetStateColor(8, RGB(233,236,250), true, 0, false);
		this->SetForegndRender(pRender);
		SAFE_RELEASE(pRender);
	}
	iter = mapAttrib.find(XML_TEXTRENDER_TYPE);
	if (iter == mapAttrib.end())
	{
		ColorListTextRender* pRender = (ColorListTextRender*)TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_COLORLIST, this);
		IRenderFont* pRenderFont = this->GetRenderFont();
		pRender->SetRenderFont(pRenderFont);
		pRender->SetTextAlignment(DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);

		pRender->SetCount(9);
		pRender->SetColor(4, RGB(255,255,255));
		pRender->SetColor(5, RGB(255,255,255));
		pRender->SetColor(6, RGB(255,255,255));
		pRender->SetColor(7, RGB(255,255,255));
		pRender->SetColor(8, RGB(0,0,0));

		this->SetTextRender(pRender);
		SAFE_RELEASE(pRender);
	}

	m_pFocusRender = RenderFactory::GetRender(RENDER_TYPE_NULL, this);
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	if (NULL == m_pGroupPropTextRender)
	{
		TextRender* pTextRender = (TextRender*)TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_NORMAL, this);
		IRenderFont* pRenderFont = this->GetRenderFont();
		IRenderFont* pRenderFontBold = NULL;

		IFontRes* pFontRes = NULL;
		m_pUIApplication->GetActiveSkinFontRes(&pFontRes);
		pFontRes->GetFontEx(pRenderFont, FONTITEM_FLAG_BOLD, 0, pRenderFont->GetGraphicsRenderLibraryType(), &pRenderFontBold);
		pTextRender->SetRenderFont(/*pRenderFont*/pRenderFontBold);
		pRenderFontBold->Release();

		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
		pTextRender->SetColor(RGB(160,160,160));

		m_pGroupPropTextRender = pTextRender;
	}
	if (NULL == m_pPropValueTextRender)
	{
		TextRender* pTextRender = (TextRender*)TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_NORMAL, this);
		IRenderFont* pRenderFont = this->GetRenderFont();
		pTextRender->SetRenderFont(pRenderFont);
		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
		pTextRender->SetColor(RGB(0,0,0));

		m_pPropValueTextRender = pTextRender;
	}
	if (NULL == m_pGroupPropForegndRender)
	{
		ColorRender* pColorRender = (ColorRender*)RenderFactory::GetRender(RENDER_TYPE_COLOR, this);
		pColorRender->SetBkColor(RGB(233,236,250));
		m_pGroupPropForegndRender = pColorRender;
	}
	if (NULL == m_pColorGrid)
	{
		UIColor::CreateInstance(RGB(233,236,250), &m_pColorGrid);
	}
	return true;
}


void PropertyCtrl::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	__super::ProcessMessage(m_pCurMsg);

	if (NULL == m_pHeaderCtrl)
		return;

	int x = 0;
	m_pHeaderCtrl->GetColumnLeftRight(0, NULL, &x);

	x += m_pHeaderCtrl->GetMarginL() + this->GetBorderL();

	CRect rcClient;
	this->GetClientRect(&rcClient);

	IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, m_pColorGrid);
	pRenderTarget->DrawLine(x, rcClient.top, x, rcClient.bottom, pPen);
	pPen->Release();
}

void PropertyCtrl::OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p)
{
	if (NULL == p)
		return;

	PropertyCtrlItem* pItem = (PropertyCtrlItem*)p;

	CRect rcItem;
	p->GetParentRect(&rcItem);

	UINT nRenderState = GetItemRenderState(p);
	if (pItem->IsGroup())
	{
		if (NULL != m_pGroupPropForegndRender)
			m_pGroupPropForegndRender->DrawState(pRenderTarget, &rcItem, nRenderState);

		if (this->IsFocus() && m_pFocusItem == pItem && NULL != m_pFocusRender)
			m_pFocusRender->DrawState(pRenderTarget, &rcItem, 0);
	}

	int nColumnCount = m_pHeaderCtrl->GetColumnCount();
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

	// 绘制GRID
	if (!pItem->IsGroup())
	{
		IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, m_pColorGrid);
		pRenderTarget->DrawLine(rcItem.left, rcItem.bottom-1, rcItem.right,rcItem.bottom-1, pPen);
		pPen->Release();
	}
}

void PropertyCtrl::OnDrawMainItem(IRenderTarget* pRenderTarget, ListTreeViewItem* p, const CRect* prcItem, int nRenderState)
{
	PropertyCtrlItem* pItem = (PropertyCtrlItem*)p;

	CRect rcItem(prcItem);
	if (pItem->IsGroup())   // 父结点直接绘制整行，无视column
	{
		pItem->GetParentRect(&rcItem);
	}
	else
	{
		if (NULL != m_pForegndRender)
			m_pForegndRender->DrawState(pRenderTarget, prcItem, nRenderState);
	}

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

	// 绘制文本
	if (NULL != m_pTextRender)
	{
		rcItem.left += m_nTextIndentLeft;
		rcItem.right -= m_nTextIndentRight;

		if (pItem->IsGroup() &&NULL != m_pGroupPropTextRender)
			m_pGroupPropTextRender->DrawState(pRenderTarget, &rcItem, nRenderState, pItem->GetText());
		else
			m_pTextRender->DrawState(pRenderTarget, &rcItem, nRenderState, pItem->GetText());
	}
}

void PropertyCtrl::OnDrawSubItem(IRenderTarget* pRenderTarget, ListViewSubItemBase* p, const CRect* prcItem, int nRenderState)
{
	CRect rc(prcItem);
	if (NULL != m_pPropValueTextRender)
	{
		rc.left += m_nTextIndentLeft;
		rc.right -= m_nTextIndentRight;
		m_pPropValueTextRender->DrawState(pRenderTarget, &rc, nRenderState, p->GetText());
	}
}

UIHTREEITEM PropertyCtrl::InsertProperty(const TCHAR* szKey, const TCHAR* szValue, UIHTREEITEM hParent, int nFlags)
{
	if (NULL == szKey)
		return NULL;

	UIHTREEITEM hTreeItem = __super::InsertItem(szKey, hParent, UITVI_LAST, nFlags);
	if (NULL == hTreeItem)
		return hTreeItem;

	if (NULL != szValue)
		__super::SetItemText(hTreeItem, 1, szValue, nFlags&LISTCTRL_ITEM_OPERATOR_FLAG_NOREDRAW ? false:true);

	return hTreeItem;
}

UIHTREEITEM PropertyCtrl::InsertGroupProperty(const TCHAR* szName, UIHTREEITEM hParent, int nFlags)
{
	if (NULL == szName)
		return NULL;

	UIHTREEITEM hTreeItem = __super::InsertItem(szName, hParent, UITVI_LAST, nFlags);

	PropertyCtrlItem* pItem = (PropertyCtrlItem*)hTreeItem;
	pItem->SetGroup(true);

	return hTreeItem;
}

void PropertyCtrl::OnPreKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetMsgHandled(FALSE);

	if (VK_ESCAPE == nChar)
	{
		Object* pFocusObj = m_pKeyboardMgr->GetFocusObject();

		if (NULL != pFocusObj && NULL != m_pEditingItem)
		{
			if (pFocusObj == m_pEdit && m_pEdit->IsMySelfVisible())
			{
				m_pKeyboardMgr->SetFocusObject(NULL);
				RejestEditContent();
			}
			if (m_pButton->IsMySelfVisible())
			{
			}
			if (m_pCombobox->IsMySelfVisible())
			{
			}
			this->InvalidateItem(m_pEditingItem);
		}
	}
	else if (VK_RETURN == nChar)
	{
		Object* pFocusObj = m_pKeyboardMgr->GetFocusObject();
		if (NULL != pFocusObj && NULL != m_pEditingItem)
		{
			if (pFocusObj == m_pEdit && m_pEdit->IsMySelfVisible())
			{
				m_pKeyboardMgr->SetFocusObject(NULL);
				AcceptEditContent();				
			}
			if (m_pButton->IsMySelfVisible())
			{
			}
			if (m_pCombobox->IsMySelfVisible())
			{
			}
			this->InvalidateItem(m_pEditingItem);
		}
	}
}
void  PropertyCtrl::OnPreKillFocus(Object* pNewFocusObj)
{
	SetMsgHandled(FALSE);

	Object* pFocusObj = m_pKeyboardMgr->GetFocusObject();

	if (NULL != pFocusObj && NULL != m_pEditingItem)
	{
		if (pFocusObj == m_pEdit && m_pEdit->IsMySelfVisible())
		{
			AcceptEditContent();
		}
	}
}

void PropertyCtrl::OnPostLButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	Object*        pHoverObj = m_pMouseMgr->GetHoverObject();
	PropertyCtrlItem*  pPressItem = (PropertyCtrlItem*)m_pMouseMgr->GetPressItem();

	if (NULL != pHoverObj)
	{
		if (pHoverObj == m_pEdit || pHoverObj == m_pCombobox || pHoverObj == m_pButton)
			return;
	}
	if (NULL == pPressItem || pPressItem->IsGroup())
	{
		if (m_pEditingItem != NULL)
		{
			AcceptEditContent();
		}
		CancelEdit();
	}
	else if (m_pEditingItem != pPressItem)
	{
		if (m_pEditingItem != NULL)
		{
			AcceptEditContent();
			CancelEdit();
		}
		BeginEdit(pPressItem);
	}
	else if (m_pEditingItem == pPressItem)
	{
		if (NULL == m_pMouseMgr->GetHoverObject() && NULL != m_pKeyboardMgr->GetFocusObject())
		{
			AcceptEditContent();
			m_pKeyboardMgr->SetFocusObject(NULL);
		}
	}

}
BOOL PropertyCtrl::OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth)
{
	SetMsgHandled(FALSE);
	if (NULL != m_pEditingItem)
	{
		Object* pFocusObj = m_pKeyboardMgr->GetFocusObject();
		if (NULL != pFocusObj && (
			pFocusObj == m_pEdit || pFocusObj==m_pButton || pFocusObj == m_pCombobox))
		{
			m_pKeyboardMgr->SetFocusObject(NULL);
			AcceptEditContent();
		}
		CancelEdit();
	}
	return 0;
}

LRESULT PropertyCtrl::OnHeaderCtrlBeginTrack(int nColumnIndex)
{
	if (PROP_VALUE_COL == nColumnIndex)
		return 1;

	return 0;
}

// 自动将第二列充满
void PropertyCtrl::OnSize(UINT nType, int cx, int cy)
{
	__super::ProcessMessage(m_pCurMsg);

	int xLeft = 0, xRight = 0;
	m_pHeaderCtrl->GetColumnLeftRight(PROP_KEY_COL, &xLeft, &xRight);
	m_pHeaderCtrl->SetItemWidth(PROP_VALUE_COL, m_pHeaderCtrl->GetWidth() - (xRight-xLeft), true);
}

#endif