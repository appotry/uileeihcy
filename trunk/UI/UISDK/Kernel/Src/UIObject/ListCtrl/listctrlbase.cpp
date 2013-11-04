#include "stdafx.h"
#include "listctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemRootPanel\listitemrootpanel.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemBase\listitembase.h"
#include <algorithm>
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\MouseKeyboard\popuplistctrlmkmgr.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\MouseKeyboard\multisellistctrlmkmgr.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\MouseKeyboard\menumkmgr.h"

namespace UI
{

//
//  内部控件的刷新流程.
//  注. 内部控件的父对象虽然是LISTCTRL，但LISTCTRL没有将内部控件作为自己的子元素，这样鼠标
//      遍历起来时就能快很多。子对象的绘制由ITEM中直接调用RootPanel::DrawObject来实现，同
//      时将RootPanel设置为背景不透明，然后修改RootPanel.OnErasebkgnd为RedrawItem实现内部
//      部分的背景刷新
//
//  1. 正向刷新
//     . ListCtrl::OnPaint ->
//     . ListCtrl::OnDrawItem, ListItem::OnDrawItem, ListItem::DrawItemInnerControl ->
//	   . RootPanel::DrawObject
//
//  2. 正向ITEM REDRAW
//     . ListCtrl::RedrawItem
//     . ListCtrl::OnDrawItem, ListItem::OnDrawItem, ListItem::DrawItemInnerControl ->
//	   . RootPanel::DrawObject
//
//  3. 反向内部控件刷新
//     . Button.UpdateObject
//     . WindowBase._InnerRedraw(由于这里的begindraw，使用ListCtrl::RedrawItem中调用begindrawpart返回isdrawing为true)
//     . RootPanel.OnEraseBkgnd(rootpanel的背景不透明，不会再往向遍历了)
//     . ListCtrl::RedrawItem
//     . ListCtrl::OnDrawItem, ListItem::OnDrawItem (这里不再调用ListItem::OnDrawItemInnerControl了，因为这一部分在第2步已绘制)
//     . Button.ParentPanel.OnEraseBkgnd
//     . Button.DrawObject                                                                                                                                                                                                                                                                                                          
//     
//

//////////////////////////////////////////////////////////////////////////

ListItemTypeShareData::ListItemTypeShareData()
{
    m_pIListItemTypeShareData = NULL;
    m_pListCtrlBase = NULL;
}
ListItemTypeShareData::~ListItemTypeShareData()
{

}
void  ListItemTypeShareData::SetListCtrlBase(ListCtrlBase* p)
{
    m_pListCtrlBase = p;
}
IListCtrlBase*  ListItemTypeShareData::GetListCtrlBase()
{
    if (m_pListCtrlBase)
        return m_pListCtrlBase->GetIListCtrlBase();
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
    
ListCtrlBase::ListCtrlBase()
{
    m_pIListCtrlBase = NULL;

	m_pFirstItem         = NULL;
	m_pLastItem          = NULL;
	m_pFirstDrawItem     = NULL;
	m_pFirstSelectedItem = NULL;
	m_pLastDrawItem      = NULL;
	m_pFocusItem         = NULL;
	m_pCompareProc       = NULL;
    m_pFocusRender       = NULL;

	m_nItemCount         = 0;
	m_nItemHeight        = 20;
	m_nVertSpacing       = 0;
	m_nHorzSpacing       = 0;
    m_nChildNodeIndent   = 20;

	m_sizeMax.cx = NDEF;
	m_sizeMax.cy = NDEF;
	m_sizeMin.cx = NDEF;
	m_sizeMin.cy = NDEF;

	m_bNeedCalcFirstLastVisibleItem = false;
	m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_NONE;
	for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
		m_pInvalidateItems[i] = NULL;

    m_pMgrScrollBar = NULL;
    m_pMKMgr = NULL;

	m_pEditingItem = NULL;

    // m_pMgrScrollBar会在子类的resetattribute中用到，因此不能放在FinalConstruct中创建
    IScrollBarManager* pScrollBarMgr = new IScrollBarManager;
    m_pMgrScrollBar = pScrollBarMgr->GetImpl();
}
ListCtrlBase::~ListCtrlBase()
{
    SAFE_DELETE(m_pMKMgr);
    SAFE_RELEASE(m_pFocusRender);
    if (m_pMgrScrollBar)
    {
        delete m_pMgrScrollBar->GetIScrollBarMgr();
        m_pMgrScrollBar = NULL;
    }
}

HRESULT  ListCtrlBase::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IListCtrlBase, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_MgrLayout.SetIListCtrlBase(m_pIListCtrlBase);
	if (NULL == m_MgrLayout.GetLayout())
    {
		SetLayout(CreateListCtrlLayout(LISTCTRL_ITEM_LAYOUT_TYPE_1, m_pIListCtrlBase));
    }

    m_pMgrScrollBar->SetBindObject(static_cast<IObject*>(m_pIListCtrlBase));

    m_pMKMgr = new SingleSelListCtrlMKMgr;
    m_pMKMgr->SetListCtrlBase(p->GetImpl(), this);

	return S_OK;
}

void  ListCtrlBase::FinalRelease()
{
	DO_PARENT_PROCESS(IListCtrlBase, IControl);
    
    m_pMgrScrollBar->SetHScrollBar(NULL); // 滚动条被销毁了（_RemoveAllItem中会调用滚动函数）
    m_pMgrScrollBar->SetVScrollBar(NULL);  

	this->_RemoveAllItem();   // 因为_RemoveAllItem会调用虚函数，因此这个操作没有放在析构函数中执行

    // 销毁共享数据
    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.begin();
    for (; iter != m_mapItemTypeShareData.end(); iter++)
    {
        iter->second->delete_this();
    }
    m_mapItemTypeShareData.clear();
}

void ListCtrlBase::ResetAttribute()
{
	DO_PARENT_PROCESS(IListCtrlBase, IControl);

	m_nItemHeight      = 20; 
	m_nVertSpacing     = 0;
	m_nHorzSpacing     = 0;

	m_sizeMax.cx = NDEF;
	m_sizeMax.cy = NDEF;
	m_sizeMin.cx = NDEF;
	m_sizeMin.cy = NDEF;

    if (m_pMgrScrollBar)
        m_pMgrScrollBar->ResetAttribute();

    SAFE_RELEASE(m_pFocusRender);

    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.begin();
    for (; iter != m_mapItemTypeShareData.end(); iter++)
    {
        UISendMessage(iter->second, UI_WM_RESETATTRIBUTE);
    }
}

void ListCtrlBase::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    IUIApplication* pUIApp = m_pIListCtrlBase->GetUIApplication();
	DO_PARENT_PROCESS(IListCtrlBase, IControl);

	int nItemHeight = 0;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_int(XML_LISTCTRL_ITEM_HEIGHT, true, &nItemHeight))
        this->SetItemHeight(nItemHeight, false);

    pMapAttrib->GetAttr_int(XML_LISTCTRL_MIN_WIDTH, true, (int*)&m_sizeMin.cx);
    pMapAttrib->GetAttr_int(XML_LISTCTRL_MIN_HEIGHT, true, (int*)&m_sizeMin.cy);

    pMapAttrib->GetAttr_int(XML_LISTCTRL_MAX_WIDTH, true, (int*)&m_sizeMax.cx);
    pMapAttrib->GetAttr_int(XML_LISTCTRL_MAX_HEIGHT, true, (int*)&m_sizeMax.cy);

	// text render
	if (NULL == m_pIListCtrlBase->GetTextRender())
	{
        ITextRenderBase* pTextRender = NULL;
        pMapAttrib->GetAttr_TextRenderBase(XML_TEXTRENDER_TYPE, true, pUIApp, m_pIListCtrlBase, &pTextRender);
        if (pTextRender)
        {
            m_pIListCtrlBase->SetTextRender(pTextRender);
            SAFE_RELEASE(pTextRender);
        }
    }
    if (NULL == m_pIListCtrlBase->GetTextRender())
    {
        ITextRenderBase* pTextRender = NULL;
        pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, m_pIListCtrlBase, &pTextRender);
        if (pTextRender)
        {
            pTextRender->SetAttribute(pMapAttrib, NULL, true);
            m_pIListCtrlBase->SetTextRender(pTextRender);
            pTextRender->Release();
        }
	}

#if 0 // -- 架构改造
	if (m_pCombobox)
		this->SetChildObjectAttribute(m_pCombobox, XML_LISTCTRL_COMBOBOX_PRIFIX, m_mapAttribute, bReload);
	if (m_pButton)
		this->SetChildObjectAttribute(m_pButton, XML_LISTCTRL_BUTTON_PRIFIX, m_mapAttribute, bReload);
	if (m_pEdit)
		this->SetChildObjectAttribute(m_pEdit, XML_LISTCTRL_EDIT_PRIFIX, m_mapAttribute, bReload);
#endif

    m_pMgrScrollBar->SetAttribute(pMapAttrib, bReload);
	m_pMgrScrollBar->SetVScrollLine(m_nItemHeight);
	m_pMgrScrollBar->SetHScrollLine(3);

    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.begin();
    for (; iter != m_mapItemTypeShareData.end(); iter++)
    {
		SERIALIZEDATA data = {0};
		data.pMapAttrib = pMapAttrib;
		data.nFlag = SERIALIZEFLAG_LOAD;
		if (bReload)
			data.nFlag |= SERIALIZEFLAG_RELOAD;
		UISendMessage(iter->second, UI_WM_SERIALIZE, (WPARAM)&data);
        //UISendMessage(iter->second, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)bReload);
    }
}


IScrollBarManager*  ListCtrlBase::GetIScrollBarMgr()
{
    if (NULL == m_pMgrScrollBar)
        return NULL;

    return m_pMgrScrollBar->GetIScrollBarMgr();
}

void ListCtrlBase::SetLayout(IListCtrlLayout* pLayout)
{
    if (pLayout)
        pLayout->SetIListCtrlBase(m_pIListCtrlBase);
    
    m_MgrLayout.SetLayout(pLayout);
}

// 将布局类型设置为ListCtrlItemLayout1
void  ListCtrlBase::SetLayoutDefaultV()
{
    this->SetLayout(new ListCtrlItemLayout1());
}
// 将布局类型设置为ListCtrlItemLayout2
void  ListCtrlBase::SetLayoutDefaultHV()
{
    this->SetLayout(new ListCtrlItemLayout2());
}
IListCtrlLayout*  ListCtrlBase::GetLayout()
{
    return m_MgrLayout.GetLayout();
}

void ListCtrlBase::RemoveItem(int nIndex, int nRemoveFlag)
{
	ListItemBase* pItem = this->GetItemByPos(nIndex, true);
	if (NULL == pItem)
		return ;

	this->RemoveItem(pItem, nRemoveFlag);
}

void  ListCtrlBase::DelayRemoveItem(ListItemBase* pItem, int nRemoveFlag)
{
    IUIApplication* pUIApplication = m_pIListCtrlBase->GetUIApplication();
    if (NULL == pUIApplication)
        return;

    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_INNER_DELAY_REMOVE_ITEM;
    msg.wParam = (WPARAM)pItem;
    msg.lParam = (LPARAM)nRemoveFlag;
    msg.pMsgFrom = m_pIListCtrlBase;
    msg.pMsgTo = m_pIListCtrlBase;
    UIPostMessage(pUIApplication, &msg);
}

void ListCtrlBase::RemoveItem(ListItemBase* pItem, int nRemoveFlag)
{
	if (NULL == pItem)
		return;

	ListItemBase* pNextItem = pItem->GetNextItem();

	bool bRet = _RemoveItem(pItem);
	if (false == bRet)
		return;

	this->UpdateItemIndex(pNextItem);
	
	if (0 == (nRemoveFlag & LISTITEM_OPFLAG_NOUPDATEITEMRECT))
	{
		this->UpdateItemRect(pNextItem, false);
	}
	if (0 == (nRemoveFlag & LISTITEM_OPFLAG_NOREDRAW))
	{
		m_pIListCtrlBase->UpdateObject();
	}
}

void  ListCtrlBase::RemoveAllChildItems(ListItemBase* pParent, int nRemoveFlag)
{
    if (NULL == pParent)
        return;

    ListItemBase* pParentNext = pParent->GetNextItem();
    bool bRet = _RemoveAllChildItems(pParent);
    if (false == bRet)
        return;

    this->UpdateItemIndex(pParentNext);

    if (0 == (nRemoveFlag & LISTITEM_OPFLAG_NOUPDATEITEMRECT))
    {
        this->UpdateItemRect(pParentNext, false);
    }
    if (0 == (nRemoveFlag & LISTITEM_OPFLAG_NOREDRAW))
    {
        m_pIListCtrlBase->UpdateObject();
    }
}

bool  ListCtrlBase::_RemoveAllChildItems(ListItemBase* pParent)
{
    if (NULL == pParent)
        return false;

    ListItemBase*  pItem = pParent->GetChildItem();
    if (NULL == pItem)
        return false;

    while (pItem)
    {
        m_nItemCount--;

        if (m_pFocusItem == pItem)
            SetFocusItem(pParent);

        if (m_pMKMgr)
            m_pMKMgr->OnRemoveItem(pItem);

        // 从selection列表中删除，放在最后通知sel changed
        if (pItem->IsSelected())
        {
            RemoveSelectItem(pItem, false);
            if (NULL == m_pFirstSelectedItem)  // 保持至少有一个选项
            {
                if (0 == m_nItemCount)
                    FireSelectItemChanged(NULL);
                else
                    SelectItem(pParent, false);
            }
        }

        ListItemBase* pNext = pItem->GetNextItem();
        pItem->GetIListItemBase()->delete_this();;
        pItem = pNext;
    }

    pParent->SetChildItem(NULL);
    this->SetCalcFirstLastVisibleItemFlag();
    return true;
}


// 未刷新，未更新Item Rect, scroll bar
bool ListCtrlBase::_RemoveItem(ListItemBase* pItem)
{
	if (NULL == pItem)
		return false;
	
    long lIdSave = pItem->GetId();

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.wParam = (WPARAM)pItem->GetIListItemBase();
        msg.nCode = UI_LCN_PRE_ITEMREMOVE;
        UISendMessage(&msg);
    }

    m_nItemCount--;

	// 从列表中删除
    // 注：必须先从列表中删除，再清除selection，
    //     否则fire selection changed响应中还能拿到该ITEM，导致崩溃
	if (pItem->GetPrevItem())
	{
		pItem->GetPrevItem()->SetNextItem(pItem->GetNextItem());
	}
	if (pItem->GetNextItem())
	{
		pItem->GetNextItem()->SetPrevItem(pItem->GetPrevItem());
	}

    // 自己做为父结点的第一个子结点
    ListItemBase* pParent = pItem->GetParentItem();
    if (pParent && pParent->GetChildItem() == pItem)
    {
        pParent->SetChildItem(pItem->GetNextItem());
    }

	if (m_pFirstItem == pItem)
	{
		m_pFirstItem = pItem->GetNextItem();
	}
	if (m_pLastItem == pItem)
	{
		m_pLastItem = pItem->GetPrevItem();
	}

	this->SetCalcFirstLastVisibleItemFlag();
    if (pItem == m_pFirstDrawItem || pItem == m_pLastDrawItem)
    {
        m_pFirstDrawItem = NULL;
        m_pLastDrawItem = NULL;
    }

    if (m_pMKMgr)
        m_pMKMgr->OnRemoveItem(pItem);

	pItem->GetIListItemBase()->delete_this();;

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.nCode  = UI_LCN_POST_ITEMREMOVE;
        msg.wParam = (WPARAM)lIdSave;
        UISendMessage(&msg);
    }
	return true;
}
void ListCtrlBase::RemoveAllItem(int nRemoveFlag)
{
	if (false == _RemoveAllItem())
		return;

	if (0 == (nRemoveFlag&LISTITEM_OPFLAG_NOUPDATEITEMRECT))
		this->UpdateItemRect(m_pFirstDrawItem, false);

	if (0 == (nRemoveFlag&LISTITEM_OPFLAG_NOREDRAW))
		m_pIListCtrlBase->UpdateObject();
}
bool ListCtrlBase::_RemoveAllItem()
{
	if (NULL == m_pFirstItem)
		return false;

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_LCN_PRE_ALLITEMREMOVE;
        UISendMessage(&msg);
    }

	ListItemBase* p = m_pFirstItem;
	while (p)
	{
		ListItemBase* pNext = p->GetNextItem();  // Save
		p->GetIListItemBase()->delete_this();
		p = pNext;
	}

	m_pFirstItem = NULL;
	m_pLastItem = NULL;
	m_pFirstDrawItem = NULL;
	m_pFirstSelectedItem = NULL;
	m_pFocusItem = NULL;
	m_nItemCount = 0;

    if (m_pMKMgr)
        m_pMKMgr->OnRemoveAll();

	m_pMgrScrollBar->SetScrollRange(0,0);

    {
        UIMSG  msg;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_LCN_POST_ALLITEMREMOVE;
        UISendMessage(&msg);
    }
	return true;
}
void ListCtrlBase::SetSortCompareProc(ListItemCompareProc p)
{
	m_pCompareProc = p;
}
void  ListCtrlBase::Sort()
{
    if (NULL == m_pCompareProc)
        return;

    if (!m_pIListCtrlBase->TestStyleEx(LISTCTRLBASE_STYLE_SORT_ASCEND) &&
        !m_pIListCtrlBase->TestStyleEx(LISTCTRLBASE_STYLE_SORT_DESCEND))
        return;

    if (m_pFirstItem == m_pLastItem)  // 0个或1个
        return;

    // 将所有元素放在一个数组中，传递给std::sort进行排序
    int nSize = GetItemCount();
    UIASSERT(nSize > 1);

    IListItemBase**  pArray = new IListItemBase*[nSize];
    pArray[0] = m_pFirstItem->GetIListItemBase();
    for (int i = 1; i < nSize; i++)
    {
        pArray[i] = pArray[i-1]->GetNextItem();
    }

    std::sort(pArray, (pArray+nSize), m_pCompareProc);

    // 根据升降序重新整理列表
    if (m_pIListCtrlBase->TestStyleEx(LISTCTRLBASE_STYLE_SORT_ASCEND))
    {
        m_pFirstItem = pArray[0]->GetImpl();
        m_pLastItem = pArray[nSize-1]->GetImpl();

        m_pFirstItem->SetPrevItem(NULL);
        m_pFirstItem->SetNextItem(pArray[1]->GetImpl());

        m_pLastItem->SetNextItem(NULL);
        m_pLastItem->SetPrevItem(pArray[nSize-2]->GetImpl());

        for (int i = 1; i < nSize-1; i++)
        {
            pArray[i]->SetNextItem(pArray[i+1]);
            pArray[i]->SetPrevItem(pArray[i-1]);
        }
    }
    else
    {
        m_pLastItem = pArray[0]->GetImpl();
        m_pFirstItem = pArray[nSize-1]->GetImpl();

        m_pFirstItem->SetPrevItem(NULL);
        m_pFirstItem->SetNextItem(pArray[nSize-2]->GetImpl());

        m_pLastItem->SetNextItem(NULL);
        m_pLastItem->SetPrevItem(pArray[1]->GetImpl());

        for (int i = 1; i < nSize-1; i++)
        {
            pArray[i]->SetPrevItem(pArray[i+1]);
            pArray[i]->SetNextItem(pArray[i-1]);
        }
    }

    SAFE_ARRAY_DELETE(pArray);
    UpdateItemIndex(NULL);
    this->SetCalcFirstLastVisibleItemFlag();
}

// 交换两个item的位置
void  ListCtrlBase::SwapItemPos(ListItemBase*  p1, ListItemBase* p2)
{
    if (NULL == p1 || NULL == p2 || p1 == p2)
        return;

    if (p1->GetNextItem() == p2)
    {
        ListItemBase* p1Prev = p1->GetPrevItem();
        ListItemBase* p2Next = p2->GetNextItem();

        if (p1Prev)
            p1Prev->SetNextItem(p2);
        p2->SetPrevItem(p1Prev);

        if (p2Next)
            p2Next->SetPrevItem(p1);
        p1->SetNextItem(p2Next);

        p2->SetNextItem(p1);
        p1->SetPrevItem(p2);
    }
    else if (p1->GetPrevItem() == p2)
    {
        ListItemBase* p1Next = p1->GetNextItem();
        ListItemBase* p2Prev = p2->GetPrevItem();

        if (p2Prev)
            p2Prev->SetNextItem(p1);
        p1->SetPrevItem(p2Prev);

        if (p1Next)
            p1Next->SetPrevItem(p2);
        p2->SetNextItem(p1Next);

        p1->SetNextItem(p2);
        p2->SetPrevItem(p1);
    }
    else
    {
        ListItemBase* p1Prev = p1->GetPrevItem();
        ListItemBase* p1Next = p1->GetNextItem();

        ListItemBase* p2Prev = p2->GetPrevItem();
        ListItemBase* p2Next = p2->GetNextItem();

        p2Next->SetPrevItem(p1);
        p1->SetNextItem(p2Next);

        p2Prev->SetNextItem(p1);
        p1->SetPrevItem(p2Prev);

        p1Next->SetPrevItem(p2);
        p2->SetNextItem(p1Next);

        p1Prev->SetNextItem(p2);
        p2->SetPrevItem(p1Prev);    
    }

    // 重置起始项
    if (NULL == p1->GetPrevItem())
        m_pFirstItem = p1;
    if (NULL == p2->GetPrevItem())
        m_pFirstItem = p2;

    if (NULL == p1->GetNextItem())
        m_pLastItem = p1;
    if (NULL == p2->GetNextItem())
        m_pLastItem = p2;

    // 更换索引行
    int nIndex1 = p1->GetLineIndex();
    int nIndex2 = p2->GetLineIndex();
    p1->SetLineIndex(nIndex2);
    p2->SetLineIndex(nIndex1);

    // 更新可见
    this->SetCalcFirstLastVisibleItemFlag();

    // 刷新item rect
    this->UpdateItemRect();
}

int  ListCtrlBase::GetChildNodeIndent()
{
    return m_nChildNodeIndent;
}
void  ListCtrlBase::SetChildNodeIndent(int n)
{
    m_nChildNodeIndent = n;
}

void ListCtrlBase::SetItemHeight(int nHeight, bool bUpdate)
{
	if (m_nItemHeight == nHeight)
		return;

	m_nItemHeight = nHeight;
//	this->MeasureAllItem();
	this->UpdateItemRect(m_pFirstItem, false);

	if (bUpdate)
		m_pIListCtrlBase->UpdateObject();
}

ListItemBase* ListCtrlBase::GetItemByPos(UINT nIndex, bool bVisibleOnly)
{
	if (nIndex < 0)
		return NULL;

    if (bVisibleOnly)
    {
        ListItemBase* pItem = FindVisibleItemFrom(NULL);

        int i = 0;
        while (pItem)
        {
            if (i == nIndex)
                return pItem;

            i++;
            pItem = pItem->GetNextVisibleItem();
        }
    }
    else
    {
        if (nIndex >= m_nItemCount)
            return NULL;

	    ListItemBase* pItem = m_pFirstItem;

	    int i = 0;
	    while (pItem)
	    {
		    if (i == nIndex)
			    return pItem;

		    i++;
		    pItem = pItem->GetNextItem();
	    }
    }
    return NULL;
}

ListItemBase*  ListCtrlBase::GetItemById(long lId)
{
    ListItemBase* pItem = m_pFirstItem;

    while (pItem)
    {
        if (pItem->GetId() == lId)
            return pItem;

        pItem = pItem->GetNextTreeItem();
    }
    return NULL;
}

ListItemBase* ListCtrlBase::FindItemByText(const TCHAR* szText, ListItemBase* pStart)
{
    if (NULL == szText)
        return NULL;

    ListItemBase* p = pStart;
    if (NULL == pStart)
        p = m_pFirstItem;

    if (NULL == p)
        return NULL;

    while (p)
    {
        if (0 == _tcscmp(p->GetText(), szText))
        {
            return p;
        }
        p = p->GetNextTreeItem();
    }

    return NULL;
}

ListItemBase*  ListCtrlBase::EnumItemByProc(ListItemEnumProc pProc, ListItemBase* pEnumFrom, WPARAM w, LPARAM l)
{
    if (!pProc)
        return NULL;

    ListItemBase* p = pEnumFrom;
    if (NULL == pEnumFrom)
        p = m_pFirstItem;

    if (NULL == p)
        return NULL;

    while (p)
    {
        if (!pProc(p->GetIListItemBase(), w, l))
        {
            return p;
        }
        p = p->GetNextTreeItem();
    }

    return NULL;
}

// 仅在pParent下查找，如果pParent为NULL,则仅在最顶层查找
ListItemBase*  ListCtrlBase::FindChildItemByText(const TCHAR* szText, ListItemBase* pParent, ListItemBase* pStart)
{
    if (NULL == szText)
        return NULL;

    if (!pStart)
    {
        if (pParent)
            pStart = pParent->GetChildItem();
        else
            pStart = m_pFirstItem;
    }

    if (pStart)
    {
        ListItemBase* p = pStart;
        while (p)
        {
            if (0 == _tcscmp(p->GetText(), szText))
            {
                return p;
            }
            p = p->GetNextItem();
        }
    }
    return NULL;
}

//
//	在末尾添加一项，根据排序结果，最后调用InsertItem
//
bool ListCtrlBase::AddItem(ListItemBase* pItem, int nAddItemFlags)
{
	ListItemBase* pInsertAfter = m_pLastItem;

	bool bAscendSort = m_pIListCtrlBase->TestStyleEx(LISTCTRLBASE_STYLE_SORT_ASCEND);
	bool bDescendSort = m_pIListCtrlBase->TestStyleEx(LISTCTRLBASE_STYLE_SORT_DESCEND);

	if ((bAscendSort||bDescendSort) && 
        NULL != m_pCompareProc      && 
        0==(nAddItemFlags&LISTITEM_OPFLAG_NOSORT) )
	{
		// 排序决定位置(由于采用了链接的数据结构，不能采用二分查找的方式...)
		ListItemBase* pEnumItem = m_pFirstItem;
		while (NULL != pEnumItem)
		{
			int nResult = m_pCompareProc(pEnumItem->GetIListItemBase(), pItem->GetIListItemBase());
			if (bAscendSort)
			{
				// 查找第一个大于自己的对象
				if (nResult>0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}
			else if(bDescendSort)
			{
				// 查找第一个小于自己的对象 
				if(nResult<0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}

			pEnumItem = pEnumItem->GetNextItem();
		}
	}

	// 插入
	return this->InsertItem(pItem, pInsertAfter, nAddItemFlags);
}

// 更新每一个ITEM的索引计数
void  ListCtrlBase::UpdateItemIndex(ListItemBase* pStart)
{
#if 0  // 无子对象版本
	if (NULL == pStart)
		pStart = m_pFirstItem;

	ListItemBase* p = pStart;
	while (p != NULL)
	{
		if (NULL == p->GetPrevItem())   // 第一个
			p->SetLineIndex(0);
		else
			p->SetLineIndex(p->GetPrevItem()->GetLineIndex()+1);

		p = p->GetNextItem();
	}

#else // 树结构版本

    int nTreeIndex = 0;
    int nLineIndex = 0;
    ListItemBase* pItem = m_pFirstItem;
    while (pItem)
    {
        pItem->SetTreeIndex(nTreeIndex++);

        if (pItem->IsVisible())
            pItem->SetLineIndex(nLineIndex++);
        else
            pItem->SetLineIndex(-1);

        if (NULL == pItem->GetPrevItem())
            pItem->SetNeighbourIndex(0);
        else
            pItem->SetNeighbourIndex((pItem->GetPrevItem())->GetNeighbourIndex()+1);

        if (NULL == pItem->GetParentItem())
            pItem->SetDepth(0);
        else
            pItem->SetDepth(pItem->GetParentItem()->GetDepth()+1);

        pItem = pItem->GetNextTreeItem();
    }

#endif
}


void ListCtrlBase::UpdateItemRect()
{
	UpdateItemRect(m_pFirstItem, true);
}
void ListCtrlBase::UpdateItemRect(ListItemBase* pStart, bool bRedraw)
{
	SetCalcFirstLastVisibleItemFlag();

    bool bWidthNotConfiged = false;
    bool bHeightNotConfiged = false;
    if (!m_pLayoutParam)
    {
        bWidthNotConfiged = true;
        bHeightNotConfiged = true;
    }
    else if (m_pLayoutParam && m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
    {
        CanvasLayoutParam* pParam = static_cast<CanvasLayoutParam*>(m_pLayoutParam);
        bWidthNotConfiged = ((pParam->GetConfigLeft() == NDEF || pParam->GetConfigRight() == NDEF) && pParam->GetConfigWidth()==AUTO);
        bHeightNotConfiged = ((pParam->GetConfigTop() == NDEF || pParam->GetConfigBottom() == NDEF) && pParam->GetConfigHeight() == AUTO);
    }

	if (bWidthNotConfiged || bHeightNotConfiged)
	{
		m_pIListCtrlBase->UpdateLayout(bRedraw);
	}
	else
	{
		SIZE sizeContent = {0,0};
        IListItemBase*  p = NULL;
        if (pStart)
            p = pStart->GetIListItemBase();
		m_MgrLayout.Arrange(p, &sizeContent);
		
		this->m_pMgrScrollBar->SetScrollRange(sizeContent.cx, sizeContent.cy);

		if (bRedraw)
			m_pIListCtrlBase->UpdateObject();
	}
}

void ListCtrlBase::OnSize(UINT nType, int cx, int cy)
{
	this->SetCalcFirstLastVisibleItemFlag();

	SIZE sizeContent = {0,0};
	m_MgrLayout.Arrange(NULL, &sizeContent);

	// 更新滚动条的属性
	CRect rcClient;
	m_pIListCtrlBase->GetClientRect(&rcClient);
    SIZE sizePage = { rcClient.Width(), rcClient.Height() };

    // 可能新增加了一个滚动条，这时候需要将滚动条的位置加到object width/height上去，而不是再占用现有的位置
    bool bWidthNotConfiged = false;
    bool bHeightNotConfiged = false;

    if (!m_pLayoutParam)
    {
        bWidthNotConfiged = true;
        bHeightNotConfiged = true;
    }
    else if (m_pLayoutParam && m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
    {
        CanvasLayoutParam* pParam = static_cast<CanvasLayoutParam*>(m_pLayoutParam);
        bWidthNotConfiged = ((pParam->GetConfigLeft() == NDEF || pParam->GetConfigRight() == NDEF) && pParam->GetConfigWidth()==AUTO);
        bHeightNotConfiged = ((pParam->GetConfigTop() == NDEF || pParam->GetConfigBottom() == NDEF) && pParam->GetConfigHeight() == AUTO);
    }

	if (bWidthNotConfiged || bHeightNotConfiged)
	{
		CRect rcRegion1;
        CRect rcRegion2;

		m_pIListCtrlBase->GetNonClientRegion(&rcRegion1);
        m_pMgrScrollBar->SetScrollPageAndRange(&sizePage, &sizeContent);
		m_pIListCtrlBase->GetNonClientRegion(&rcRegion2);

		if (rcRegion1 != rcRegion2)  // 滚动条显隐发生改变，则按照当前的nonclient region再次布局
			m_pIListCtrlBase->UpdateLayout(false);  // 由SetObjectPos负责刷新
	}
	else
	{
		m_pMgrScrollBar->SetScrollPageAndRange(&sizePage, &sizeContent);
	}
}

// TODO: 以后想法去掉PopupListBoxMouseMgr类型
void ListCtrlBase::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
    SetMsgHandled(FALSE);
    if (nStyleType == GWL_EXSTYLE)
    {
        bool bMultiSelNew = lpStyleStruct->styleNew & LISTCTRLBASE_STYLE_MULTIPLE_SEL?true:false;
        bool bMultiSelOld = lpStyleStruct->styleOld & LISTCTRLBASE_STYLE_MULTIPLE_SEL?true:false;

        bool bPopupNew = lpStyleStruct->styleNew & LISTCTRLBASE_STYLE_POPUPLISTBOX?true:false;
        bool bPopupOld = lpStyleStruct->styleOld & LISTCTRLBASE_STYLE_POPUPLISTBOX?true:false;

		bool bMenuNew = lpStyleStruct->styleNew & LISTCTRLBASE_STYLE_MENU?true:false;
		bool bMenuOld = lpStyleStruct->styleOld & LISTCTRLBASE_STYLE_MENU?true:false;

        if (bMultiSelOld != bMultiSelNew || bPopupNew != bPopupOld || bMenuNew != bMenuOld)
        {
            SAFE_DELETE(m_pMKMgr);
            
            if (bMultiSelNew)
            {
                m_pMKMgr = new MultiSelListCtrlMKMgr;
            }
            else if (bPopupNew)
            {
                m_pMKMgr = new PopupListCtrlMKMgr;
            }
			else if (bMenuNew)
			{
				m_pMKMgr = new MenuMKMgr;
			}
            else
            {
                m_pMKMgr = new SingleSelListCtrlMKMgr;
            }
            m_pMKMgr->SetListCtrlBase(m_pIListCtrlBase->GetUIApplication()->GetImpl(), this);
        }
    }
}

#pragma  region  // selection

bool ListCtrlBase::IsSelected(ListItemBase* pItem)
{
	if (NULL == pItem)
		return false;

// 	if (m_pFirstSelectedItem == pItem || NULL != pItem->GetPrevSelection())
// 		return true;

    return pItem->IsSelected();
}

// 设置一个选择项(只选中一个)
void ListCtrlBase::SelectItem(ListItemBase* pItem, bool bUpdate)
{
	if (NULL == pItem)  
		return;

    // 定位到该ITEM，确保完全可见
	if (m_pFirstSelectedItem == pItem && NULL == m_pFirstSelectedItem->GetNextSelection())
	{
        bool bNeedUpdate = false;
		this->MakeItemVisible(m_pFirstSelectedItem, &bNeedUpdate);
        if (bNeedUpdate)
			this->SetInvalidateAllItems();
		return;
	}

	ListItemBase* pOldSelectoinItem = m_pFirstSelectedItem;
	ClearSelectItem(false);
	m_pFirstSelectedItem = pItem;
    if (pItem)
        pItem->SetSelected(true);

    bool bNeedUpdate = false;
	this->MakeItemVisible(m_pFirstSelectedItem, &bNeedUpdate);
    if (bNeedUpdate)
		this->SetInvalidateAllItems();

	if (m_pFirstSelectedItem != pOldSelectoinItem)
	{
		this->InvalidateItem(m_pFirstSelectedItem);
		this->InvalidateItem(pOldSelectoinItem);

		FireSelectItemChanged(pOldSelectoinItem);
	}

	if (bUpdate)
		this->Refresh();
}

// 对于多选样式，不再针对每一个ITEM发送一次选中消息，而是只发消息，不发状态，
// 由外部自己去判断所需要的状态
void  ListCtrlBase::FireSelectItemChanged(ListItemBase* pOldSelectoinItem)
{
    // 通知ctrl
    int nStyle = m_pIListCtrlBase->GetStyle();

	UIMSG  msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_LCN_SELCHANGED_SELF;
	if (nStyle & LISTCTRLBASE_STYLE_MULTIPLE_SEL)
	{

	}
	else
	{
        msg.wParam = (WPARAM)(pOldSelectoinItem?pOldSelectoinItem->GetIListItemBase():NULL);
        msg.lParam = (LPARAM)(m_pFirstSelectedItem?m_pFirstSelectedItem->GetIListItemBase():NULL);
	}
	msg.pMsgFrom = m_pIListCtrlBase;

    // 先交给子类处理
    msg.pMsgTo = m_pIListCtrlBase;
    UISendMessage(&msg, 0, 0);

    // 再通知外部处理 
    msg.pMsgTo = NULL;
    msg.bHandled = FALSE;
	msg.nCode = UI_LCN_SELCHANGED;
	m_pIListCtrlBase->DoNotify(&msg);
}

ListItemBase* ListCtrlBase::GetLastSelectItem()
{
	ListItemBase* pItem = m_pFirstSelectedItem;
	while (pItem)
	{
		ListItemBase* pNext = pItem->GetNextSelection();
		if (NULL == pNext)
			break;
		
		pItem = pNext;
	}

	return pItem;
}

// 如果当前没有选择项，只设置为选择项。如果已经有选择项，则增加为下一个选择项
void ListCtrlBase::AddSelectItem(ListItemBase* pItem)
{
	if (NULL == pItem)
		return;

    int nStyle = m_pIListCtrlBase->GetStyle();
	if (nStyle & LISTCTRLBASE_STYLE_MULTIPLE_SEL)
	{
		if (pItem->IsSelected())
			return;

		ListItemBase* pLastSelItem = this->GetLastSelectItem();
		if (NULL == pLastSelItem)
		{
			SelectItem(pItem, false);
		}
		else
		{
			pLastSelItem->SetNextSelection(pItem);
			pItem->SetPrevSelection(pLastSelItem);
            pItem->SetSelected(true);
			this->FireSelectItemChanged(NULL);
		}
	}
	else   // 单选
	{
		if (m_pFirstSelectedItem == pItem)
			return;

		SelectItem(pItem, false);
	}	
}
void ListCtrlBase::RemoveSelectItem(ListItemBase* pItem, bool bNotify)
{
	if (NULL == pItem || NULL == m_pFirstSelectedItem)
		return;

	if (!pItem->IsSelected())
		return;

	ListItemBase* pOldSelection = m_pFirstSelectedItem;
	if (pItem->GetPrevSelection())
		pItem->GetPrevSelection()->SetNextSelection(pItem->GetNextSelection());
	if (pItem->GetNextSelection())
		pItem->GetNextSelection()->SetPrevSelection(pItem->GetPrevSelection());

    if (m_pFirstSelectedItem == pItem)
        m_pFirstSelectedItem = pItem->GetNextSelection();

	pItem->SetPrevSelection(NULL);
	pItem->SetNextSelection(NULL);

    pItem->SetSelected(false);
	if (bNotify)
		this->FireSelectItemChanged(pOldSelection);
}
void ListCtrlBase::ClearSelectItem(bool bNotify)
{
	if (NULL == m_pFirstSelectedItem)
		return;

	ListItemBase* pOldSelection = m_pFirstSelectedItem;
	
	ListItemBase* pItem = m_pFirstSelectedItem;
	while (pItem)
	{
		this->InvalidateItem(pItem);
        pItem->SetSelected(false);

		ListItemBase* pNextItem = pItem->GetNextSelection();
		if (NULL == pNextItem)
			break;

		pNextItem->SetPrevSelection(NULL);
		pItem->SetNextSelection(NULL);
		pItem = pNextItem;
	}
	m_pFirstSelectedItem = NULL;

	if (bNotify)
	{
		this->FireSelectItemChanged(pOldSelection);
	}
}
#pragma endregion

ListItemBase* ListCtrlBase::GetHoverItem()
{
    if (NULL == m_pMKMgr)
        return NULL;

    return m_pMKMgr->GetHoverItem();
}
ListItemBase* ListCtrlBase::GetPressItem()
{
    if (NULL == m_pMKMgr)
        return NULL;

    return m_pMKMgr->GetPressItem();
}

ListItemBase* ListCtrlBase::GetFocusItem()
{
    if (NULL == m_pMKMgr)
        return NULL;

    return m_pMKMgr->GetFocusItem();
}
void  ListCtrlBase::SetFocusItem(ListItemBase* pItem)
{
	UIASSERT(0 && _T("focus已放到mkmgr中实现 "));
	if (m_pFocusItem == pItem)
		return;

	if (m_pFocusItem)
		this->InvalidateItem(m_pFocusItem);

    if (m_pFocusItem)
        m_pFocusItem->SetFocus(false);

	m_pFocusItem = pItem;

    if (m_pFocusItem)
        m_pFocusItem->SetFocus(true);

	if (m_pFocusItem)
		this->InvalidateItem(m_pFocusItem);
}

Object*  ListCtrlBase::GetHoverObject()
{
    return m_pMKMgr->GetHoverObject();
}
Object*  ListCtrlBase::GetPressObject()
{
    return m_pMKMgr->GetPressObject();
}
void  ListCtrlBase::SetFocusObject(Object* pObj)
{
    m_pMKMgr->SetFocusObject(pObj);
}
Object*  ListCtrlBase::GetFocusObject()
{
    return m_pMKMgr->GetFocusObject();
}
SIZE ListCtrlBase::GetAdaptWidthHeight(int nWidth, int nHeight)
{
	SIZE s = {nWidth,nHeight};

	if (m_sizeMin.cx != NDEF && nWidth < m_sizeMin.cx)
		s.cx = m_sizeMin.cx;
	if (m_sizeMax.cx != NDEF && nWidth > m_sizeMax.cx)
		s.cx = m_sizeMax.cx;

	if (m_sizeMin.cy != NDEF && nHeight < m_sizeMin.cy)
		s.cy = m_sizeMin.cy;
	if (m_sizeMax.cy != NDEF && nHeight > m_sizeMax.cy)
		s.cy = m_sizeMax.cy;

	return s;
}

void ListCtrlBase::Scroll2Y(int nY, bool bUpdate)
{
	this->SetScrollPos(-1, nY, bUpdate);
}
void ListCtrlBase::Scroll2X(int nX, bool bUpdate)
{
	this->SetScrollPos(nX, -1, bUpdate);
}
void  ListCtrlBase::ScrollY(int nY, bool bUpdate)
{
    this->SetScrollPos(-1, m_pMgrScrollBar->GetVScrollPos() + nY, bUpdate);
}
void  ListCtrlBase::ScrollX(int nX, bool bUpdate)
{
    this->SetScrollPos(m_pMgrScrollBar->GetHScrollPos() + nX, -1, bUpdate);
}
void ListCtrlBase::SetScrollPos(int nX, int nY, bool bUpdate)
{
    int nOldX = m_pMgrScrollBar->GetHScrollPos();
    int nOldY = m_pMgrScrollBar->GetVScrollPos();
    
    if (-1 != nX)
    {
        m_pMgrScrollBar->SetHScrollPos(nX);        
    }
    if (-1 != nY)
    {
        m_pMgrScrollBar->SetVScrollPos(nY);
    }

    if (nOldX != m_pMgrScrollBar->GetHScrollPos() ||
        nOldY != m_pMgrScrollBar->GetVScrollPos())
    {
        SetCalcFirstLastVisibleItemFlag();
        if (bUpdate)
            m_pIListCtrlBase->UpdateObject();
    }
}

bool ListCtrlBase::InsertItem(ListItemBase* pItem, UINT nPos, int nAddItemFlags)
{
	ListItemBase* pInsertAfter = NULL;
	if (nPos >= m_nItemCount)
		pInsertAfter = m_pLastItem;
	else
		pInsertAfter = this->GetItemByPos(nPos, true);

	return this->InsertItem(pItem, pInsertAfter, nAddItemFlags);
}
//
// 在pInsertAfter前面插入pItem，如果pInsertAfter为NULL表示插入最前面
//
bool ListCtrlBase::InsertItem(ListItemBase*  pItem, ListItemBase* pInsertAfter, int nAddItemFlags)
{
	if (NULL == pItem)
		return false;
	
	if (false == this->_InsertItem(pItem, pInsertAfter))
		return false;

//	this->MeasureItem(pItem);
	this->UpdateItemIndex(pItem);

	// 需要更新Item Rect
	if (0 == (nAddItemFlags & LISTITEM_OPFLAG_NOUPDATEITEMRECT))
	{
		this->UpdateItemRect(pItem, false);
	}
	if (0 == (nAddItemFlags & LISTITEM_OPFLAG_NOREDRAW))
	{
		m_pIListCtrlBase->UpdateObject();
	}

    UISendMessage(pItem->GetIListItemBase(), UI_WM_INITIALIZE);

    UIMSG  msg;
    msg.pMsgTo = m_pIListCtrlBase;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_ITEMADD;
    msg.wParam = (WPARAM)pItem->GetIListItemBase();
    msg.lParam = nAddItemFlags;
    UISendMessage(&msg);

	return true;
}


// 由InsertItem调用。仅做数据插入的工作，不处理SIZE
bool ListCtrlBase::_InsertItem(ListItemBase* pItem, ListItemBase* pInsertAfter)
{
	if (NULL == pItem)
		return false;

	if (NULL == pInsertAfter)// 插在首位
	{
		if (NULL == m_pFirstItem)
		{
			m_pFirstItem = m_pLastItem = pItem;
		}
		else
		{
			m_pFirstItem->SetPrevItem(pItem);
			pItem->SetNextItem(m_pFirstItem);
			m_pFirstItem = pItem;
		}
	}
	else
	{
		if (NULL == pInsertAfter->GetNextItem())
		{
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
			m_pLastItem = pItem;
		}
		else
		{
			pInsertAfter->GetNextItem()->SetPrevItem(pItem);
			pItem->SetNextItem(pInsertAfter->GetNextItem());
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
		}
	}
    
    pItem->SetIListCtrlBase(m_pIListCtrlBase);

	m_nItemCount++;
	SetCalcFirstLastVisibleItemFlag();

	return true;
}

void  ListCtrlBase::GetDesiredSize(SIZE* pSize) 
{
    m_MgrLayout.Measure(pSize);
}


#if 0 // -- 架构改造  -- 废弃
// 获取所有item中，最大的宽度值(如果pDesiredHeight不为空，则同时返回总共需要的高度)
int ListCtrlBase::GetMaxDesiredWidth(int* pDesiredHeight)
{
	if (pDesiredHeight)
		*pDesiredHeight = 0;

	int nRet = 0;
	ListItemBase* p = m_pFirstItem;
	while (p != NULL)
	{
		SIZE s = GetItemDesiredSize(p);
		if (s.cx > nRet)
			nRet = s.cx;

		if (pDesiredHeight)
		{
			*pDesiredHeight += s.cy;
			if (p != m_pFirstItem)
			{
				*pDesiredHeight += m_nVertSpacing;
			}
		}
		p = p->GetNextVisibleItem();
	}

	return nRet;
}
#endif

void  ListCtrlBase::SetCalcFirstLastVisibleItemFlag()
{
    m_bNeedCalcFirstLastVisibleItem = true; 
    m_pFirstDrawItem = m_pLastDrawItem = NULL;
}

// 计算当前的第一个可视ITEM
void  ListCtrlBase::CalcFirstLastVisibleItem()
{
	m_pFirstDrawItem = NULL;
	m_pLastDrawItem = NULL;

	CRect  rcClient;
	m_pIListCtrlBase->GetClientRectAsWin32(&rcClient);

	int xOffset = 0, yOffset = 0;
	m_pIListCtrlBase->GetScrollOffset(&xOffset, &yOffset);

	ListItemBase* pItem = this->GetFirstItem();
	while (pItem)
	{
		CRect  rcParent;
		pItem->GetParentRect(&rcParent);
		if (rcParent.bottom - yOffset < rcClient.top)  // top cover unvisible item
		{
			pItem = pItem->GetNextVisibleItem();
			continue;
		}
		if (NULL == m_pFirstDrawItem)
			m_pFirstDrawItem = pItem;

		if (pItem->GetParentRect()->top - yOffset >= rcClient.bottom)  // last visible item
			break;

        m_pLastDrawItem = pItem;
		pItem = pItem->GetNextVisibleItem();
	}
}

ListItemBase*  ListCtrlBase::GetFirstDrawItem()
{
	return m_pFirstDrawItem;   // 有可能这个时候m_nNeedCalcFirstLastVisibleItem为true，但仍然返回m_pFirstVisibleItem，因为可能还没有调用UpdateRectItem
}
ListItemBase*  ListCtrlBase::GetLastDrawItem()
{
	return m_pLastDrawItem;
}

ListItemBase*  ListCtrlBase::FindVisibleItemFrom(ListItemBase* pFindFrom)
{
    if (NULL == pFindFrom)
        pFindFrom = m_pFirstItem;

    while (pFindFrom)
    {
        if (pFindFrom->IsVisible())
            return pFindFrom;

        pFindFrom = pFindFrom->GetNextTreeItem();
    }

    return NULL;
}

bool ListCtrlBase::IsItemVisibleInScreen(ListItemBase* pItem)
{
	if (NULL == pItem)
		return false;

	if (false == pItem->IsVisible())
		return false;

	CRect rcClient;
	m_pIListCtrlBase->GetClientRect(&rcClient);

	int xOffset = 0, yOffset = 0;
	m_pMgrScrollBar->GetScrollPos(&xOffset, &yOffset);

	CRect rcParent;
	pItem->GetParentRect(&rcParent);
	if (rcParent.bottom - yOffset <= 0) 
		return false;

	if (rcParent.top - yOffset >= rcClient.Height())  
		return false;

	return true;
}
bool ListCtrlBase::IsItemVisibleInScreenEx(ListItemBase* pItem, LISTITEM_VISIBLE_POS_TYPE& ePos)
{
	if (NULL == pItem)
		return false;

	if (false == pItem->IsVisible())
		return false;

	CRect rcClient, rcItemParent;
	m_pIListCtrlBase->GetClientRect(&rcClient);
	pItem->GetParentRect(&rcItemParent);

	int xOffset = 0, yOffset = 0;
	m_pMgrScrollBar->GetScrollPos(&xOffset, &yOffset);

	int yTop = rcItemParent.top - yOffset;
	int yBottom = rcItemParent.bottom - yOffset;
	if (yBottom <= 0) 
	{
		ePos = LISTITEM_UNVISIBLE_TOP;
		return false;
	}
	else if (yTop >= rcClient.Height())
	{
		ePos = LISTITEM_UNVISIBLE_BOTTOM;
		return false;
	}
	else if (yTop < 0)
	{
		ePos = LISTITEM_VISIBLE_COVERTOP;
		return true;
	}
	else if (yBottom > rcClient.Height())
	{
		ePos = LISTITEM_VISIBLE_COVERBOTTOM;
		return true;
	}

	ePos = LISTITEM_VISIBLE;
	return true;
}

// 设置滚动条的位置，确保pItem可见
// 返回控件是否滚动了（是否需要刷新）
void  ListCtrlBase::MakeItemVisible(ListItemBase* pItem, bool* pbNeedUpdate)
{
	bool  bNeedUpdateObject = false;

	if (NULL == pItem)
		return ;

	LISTITEM_VISIBLE_POS_TYPE ePosType;
	this->IsItemVisibleInScreenEx(pItem, ePosType);

	if (LISTITEM_UNVISIBLE_TOP == ePosType || LISTITEM_VISIBLE_COVERTOP == ePosType)
	{
		bNeedUpdateObject = true;

		CRect rc;
		pItem->GetParentRect(&rc);
		m_pMgrScrollBar->SetVScrollPos(rc.top);
		this->SetCalcFirstLastVisibleItemFlag();
	}
	else if (LISTITEM_UNVISIBLE_BOTTOM == ePosType || LISTITEM_VISIBLE_COVERBOTTOM == ePosType)
	{
		bNeedUpdateObject = true;

		CRect rc, rcClient;
		pItem->GetParentRect(&rc);
		m_pIListCtrlBase->GetClientRect(&rcClient);

		m_pMgrScrollBar->SetVScrollPos(rc.bottom - rcClient.Height());
		this->SetCalcFirstLastVisibleItemFlag();
	}

    if (pbNeedUpdate)
        *pbNeedUpdate = bNeedUpdateObject;
}

void ListCtrlBase::ItemRect2WindowRect(const CRect* prc, CRect* prcRet)
{
	if (NULL == prc || NULL == prcRet)
		return;

	CRect rcItem(*prc);

	int xOffset = 0, yOffset = 0;
	m_pMgrScrollBar->GetScrollPos(&xOffset, &yOffset);
	rcItem.OffsetRect(-xOffset, -yOffset);

	CRect rcWindow;
	m_pIListCtrlBase->GetClientRectInWindow(&rcWindow);
	rcItem.OffsetRect(rcWindow.left, rcWindow.top);

	IntersectRect(prcRet, &rcItem, &rcWindow);
}


void ListCtrlBase::WindowPoint2ItemPoint(ListItemBase* pItem, const POINT* pt, POINT* ptRet)
{
    if (NULL == pItem || NULL == pt || NULL == ptRet)
        return;

    CRect rcItem;
    pItem->GetParentRect(&rcItem);

    m_pIListCtrlBase->WindowPoint2ObjectClientPoint_CalcScroll(pt, ptRet);
    ptRet->x -= rcItem.left;
    ptRet->y -= rcItem.top;
}

void ListCtrlBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
	case VK_PRIOR:
        OnKeyDown_Prev();
        break;
    
    case VK_NEXT:
        OnKeyDown_Next();
        break;

    case VK_HOME:
        OnKeyDown_Home();
        break;

    case VK_END:
        OnKeyDown_End();
        break;

    default:
        SetMsgHandled(FALSE);
        break;
    }
}

// 从当前FocusItem开始向前翻页
void  ListCtrlBase::OnKeyDown_Prev()
{
    if (NULL == m_pFirstItem || m_pFocusItem == m_pFirstItem)
        return;

    if (NULL == m_pFocusItem)
    {
        SetFocusItem(m_pFirstItem);
        SelectItem(m_pFirstItem, true);
        return;
    }

    // 计算向上翻的位置
    CRect rcItem;
    m_pFocusItem->GetParentRect(&rcItem);
    int nPos = rcItem.top - m_pMgrScrollBar->GetVScrollPage();

    // 查找该位置处的ITEM
    ListItemBase*  pItem = m_pFocusItem->GetPrevVisibleItem();
    while (pItem)
    {
        pItem->GetParentRect(&rcItem);
        if (rcItem.top <= nPos)
            break;

        ListItemBase* pTemp = pItem->GetPrevVisibleItem();
        if (NULL == pTemp)
            break;

        pItem = pItem->GetPrevVisibleItem();
    }
    if (NULL == pItem)
       return;

    // 选中
    SetFocusItem(pItem);
    SelectItem(pItem, true);
}
void  ListCtrlBase::OnKeyDown_Next()
{
    if (NULL == m_pFirstItem || m_pFocusItem == m_pLastItem)
        return;

    if (NULL == m_pFocusItem)
    {
        SetFocusItem(m_pFirstItem);
        SelectItem(m_pFirstItem, true);
        return;
    }

    // 计算向下翻的位置
    CRect rcItem;
    m_pFocusItem->GetParentRect(&rcItem);
    int nPos = rcItem.bottom + m_pMgrScrollBar->GetVScrollPage();

    // 查找该位置处的ITEM
    ListItemBase*  pItem = m_pFocusItem->GetNextVisibleItem();
    while (pItem)
    {
        pItem->GetParentRect(&rcItem);
        if (rcItem.bottom >= nPos)
            break;

        ListItemBase* pTemp = pItem->GetNextVisibleItem();
        if (NULL == pTemp)
            break;

        pItem = pItem->GetNextVisibleItem();
    }
    if (NULL == pItem)
        return;

    // 选中
    SetFocusItem(pItem);
    SelectItem(pItem, true);
}
void  ListCtrlBase::OnKeyDown_Home()
{
    if (m_pFirstItem)
    {
        SetFocusItem(m_pFirstItem);
        SelectItem(m_pFirstItem, true);
    }
}
void  ListCtrlBase::OnKeyDown_End()
{
    if (m_pLastItem)
    {
        SetFocusItem(m_pLastItem);
        SelectItem(m_pLastItem, true);
    }
}
void  ListCtrlBase::OnKeyDown_Up()
{
#if 0
    ListItemBase* pOldSelItem = m_pFirstSelectedItem;
    int nStyle = m_pIListCtrlBase->GetStyleEx();
    if (nStyle & LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE)  // 菜单或者弹出式列表框
    {
        if (NULL == GetHoverItem())
        {
            if (NULL == m_pFirstSelectedItem)
            {
                SelectItem(m_pFirstItem, false);
            }
            else 
            {
                ListItemBase* pPrevItem = m_pFirstSelectedItem->GetPrevVisibleItem();
                if (pPrevItem)
                    SelectItem(pPrevItem, false);
            }
        }
        else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
        {
            pOldSelItem = GetHoverItem();

            ListItemBase* pPrevItem = GetHoverItem()->GetPrevVisibleItem();
            if (pPrevItem)
                SelectItem(pPrevItem, false);
            else
                SelectItem(GetHoverItem(), false);
        }

        if (m_pFirstSelectedItem)  // 清除hover对象，显示selection对象
        {
            if (GetHoverItem() != NULL)
                this->InvalidateItem(GetHoverItem());

            SetHoverItem(NULL);
        }
    }
    else  // 正常列表框
    {
        if (NULL == m_pFocusItem)
        {
            SetFocusItem(m_pFirstItem);
            SelectItem(m_pFirstItem, false);
        }
        else
        {
            ListItemBase* pPrevItem = m_pFocusItem->GetPrevVisibleItem();
            if (pPrevItem)
            {
                SetFocusItem(pPrevItem);
                SelectItem(pPrevItem, false);
            }
        }
    }
    this->Refresh();
#endif
}
void  ListCtrlBase::OnKeyDown_Down()
{
#if 0
    ListItemBase* pOldSelItem = m_pFirstSelectedItem;
    int nStyle = m_pIListCtrlBase->GetStyleEx();
    if (nStyle & LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE)  // 弹出式列表框
    {
        if (NULL == GetHoverItem())
        {
            if (NULL == m_pFirstSelectedItem)
            {
                SelectItem(m_pFirstItem, false);
            }
            else 
            {
                ListItemBase* pNextItem = m_pFirstSelectedItem->GetNextVisibleItem();
                if (pNextItem)
                    SelectItem(pNextItem, false);		
            }
        }
        else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
        {
            pOldSelItem = GetHoverItem();

            ListItemBase* pNextItem = GetHoverItem()->GetNextVisibleItem();
            if (pNextItem)
                SelectItem(pNextItem, false);
            else
                SelectItem(GetHoverItem(), false);
        }

        if (m_pFirstSelectedItem)  // 清除hover对象，显示selection对象
        {
            if (GetHoverItem() != NULL)
                this->InvalidateItem(GetHoverItem());

            SetHoverItem(NULL);
        }
    }
    else  // 正常列表框
    {
        if (NULL == m_pFocusItem)
        {
            this->SetFocusItem(m_pFirstItem);
            this->SelectItem(m_pFirstItem, false);
        }
        else
        {
            ListItemBase* pNextItem = m_pFocusItem->GetNextVisibleItem();
            if (pNextItem)
            {
                SetFocusItem(pNextItem);
                SelectItem(pNextItem, false);
            }
        }
    }
    this->Refresh();
#endif
}

void ListCtrlBase::OnStateChanged(UINT nMask)
{
	if (nMask & OSB_FOCUS)
	{
		m_pIListCtrlBase->UpdateObject();
	}
}

void ListCtrlBase::OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
	SetMsgHandled(FALSE);
	SetCalcFirstLastVisibleItemFlag();
}
BOOL  ListCtrlBase::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
    int nhScrollPos = 0, nvScrollPos = 0;
    int nhScrollPos2 = 0, nvScrollPos2 = 0;

    m_pMgrScrollBar->GetScrollPos(&nhScrollPos, &nvScrollPos);
    m_pMgrScrollBar->nvProcessMessage(GetCurMsg(), 0, false);
    m_pMgrScrollBar->GetScrollPos(&nhScrollPos2, &nvScrollPos2);

    if (nvScrollPos != nvScrollPos2 || nhScrollPos != nhScrollPos2)
    {
        if (nvScrollPos2 != nvScrollPos)
            SetCalcFirstLastVisibleItemFlag();
        
        this->UpdateObject();
    }

    return 0;
}
LRESULT  ListCtrlBase::OnInertiaVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetCalcFirstLastVisibleItemFlag();
    this->UpdateObject();
    return 0;
}
void ListCtrlBase::OnPaint(IRenderTarget* pRenderTarget, RenderContext* proc)
{ 
	if (m_bNeedCalcFirstLastVisibleItem)
	{
		m_bNeedCalcFirstLastVisibleItem = false;
		this->CalcFirstLastVisibleItem();
	}

	this->ClearInvalidateItems();   // 因为OnPaint是全部重画，因此在这里将所有的invalidate items清除

#if 0
	ISelectRegionRender* pSelectRegionRender = m_pMouseMgr->GetSelectRegionRender();
	if (pSelectRegionRender)
	{
		pSelectRegionRender->PrePaint();
	}
#endif

//	OnPrePaint(pRenderTarget);
	ListItemBase* pItem = m_pFirstDrawItem;
	while (pItem)
	{
		this->OnDrawItem(pRenderTarget,pItem);                  // 绘制背景
        pItem->Draw(pRenderTarget, proc);                             // 子对象绘制
		pItem->DrawItemInnerControl(pRenderTarget, proc);             // 绘制内部控件

		if (pItem == m_pLastDrawItem)
			break;

		pItem = pItem->GetNextVisibleItem();
	}
//	OnPostPaint(pRenderTarget);
}

void  ListCtrlBase::OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p)
{
    if (m_pIListCtrlBase->TestStyle(CONTROL_STYLE_OWNERDRAW))
    {
        UIOWNERDRAWSTRUCT s;
        s.pItemDraw = p->GetIListItemBase();
        s.pObjDraw = m_pIListCtrlBase;
        s.pRenderTarget = pRenderTarget;

        UIMSG  msg;
        msg.message = UI_WM_OWNERDRAW;
        msg.wParam = (WPARAM)&s;

        m_pIListCtrlBase->DoNotify(&msg);
    }
    else
    {
        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_LCN_DRAWITEM;
        msg.pMsgFrom = m_pIListCtrlBase;
        msg.pMsgTo = m_pIListCtrlBase;
        msg.wParam = (WPARAM)pRenderTarget;
        msg.lParam = (LPARAM)p->GetIListItemBase();

        UISendMessage(&msg);
    }
}

void ListCtrlBase::SetHoverItem(ListItemBase* pItem)
{
	if (m_pMKMgr)
        m_pMKMgr->SetHoverItem(pItem);
}
void ListCtrlBase::SetPressItem(ListItemBase* pItem)
{
	if (m_pMKMgr)
        m_pMKMgr->SetPressItem(pItem);
}


// 响应UI_WM_GET_TOOLTIPINFO消息，获取提示条信息
LRESULT ListCtrlBase::OnGetToolTipInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TOOLTIPITEM*  pToolTipItem = (TOOLTIPITEM*)wParam;
	IToolTipUI*  pToolTip = (IToolTipUI*)lParam;

	if (NULL == pToolTipItem || NULL == pToolTip)
		return 0;

	IListItemBase* pItem = (IListItemBase*)pToolTipItem->pItemData;
	if (NULL == pItem)
		return 0;

    const TCHAR* szText = pItem->GetToolTip();
    if (NULL == szText || 0 == _tcslen(szText))
        return 0;

    // 判断鼠标位置是否位于Item内，有可能是通过方向键响应的SetHoverItem来显示提示条，
    POINT  ptCursor;
    CRect  rcItem, rcWindowItem;

    ::GetCursorPos(&ptCursor);
    pItem->GetParentRect(&rcItem);
    this->ItemRect2WindowRect(&rcItem, &rcWindowItem);

    MapWindowPoints(m_pIListCtrlBase->GetHWND(), NULL, (LPPOINT)&rcWindowItem, 2);
    if (!rcWindowItem.PtInRect(ptCursor))
    {
        return 0;
    }

    pToolTip->SetText(szText);
    return 1;
}
// void ListCtrlBase::OnFontModifyed(IRenderFont* pFont)
// {
// 	if (pFont == GetRenderFont() && NULL != pFont)
// 	{
// 		ListItemBase* pItem = m_pFirstItem;
// 		while (pItem)
// 		{
// 			this->MeasureItem(pItem);
// 			pItem = pItem->GetNextItem();
// 		}
// 	}
// 
// 	this->UpdateItemRect();
// }

#pragma region // Invalidate Item
void  ListCtrlBase::InvalidateItem(ListItemBase* pItem)
{
	if (NULL == pItem)
		return;

	if (m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_ALL)
		return;
	
	for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
	{
		if (m_pInvalidateItems[i] == pItem)
		{
			return;  // 已经存在
		}
		else if (m_pInvalidateItems[i] == NULL)
		{
			m_pInvalidateItems[i] = pItem;
			m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_ITEM;
			return;
		}
	}

	// 满了
	m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_ALL;
	for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
		m_pInvalidateItems[i] = NULL;
}

void ListCtrlBase::SetInvalidateAllItems()
{
	this->ClearInvalidateItems();
	m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_ALL;
}

// 清除无效项。例如在OnPaint之前
void  ListCtrlBase::ClearInvalidateItems()
{
	if (LISTCTRL_NEED_UPDATE_FLAG_NONE != m_eInvalidateFlag)
	{
		m_eInvalidateFlag = LISTCTRL_NEED_UPDATE_FLAG_NONE;
		for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
			m_pInvalidateItems[i] = NULL;
	}
}

int  ListCtrlBase::GetInvalidateItemCount()
{
	int nCount = 0;
	for (int i = 0; i < LISTCTRL_MAX_INVALIDATE_ITEM_COUNT; i++)
	{
		if (m_pInvalidateItems[i] != NULL)
			nCount++;
	}

	return nCount;
}

// 刷新invalidate item
void  ListCtrlBase::Refresh()
{
	if (m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_ALL)
	{
		m_pIListCtrlBase->UpdateObject();
		return;
	}
	else if (m_eInvalidateFlag == LISTCTRL_NEED_UPDATE_FLAG_NONE)
	{
		return;
	}
	else
	{
		this->RedrawItem(m_pInvalidateItems, LISTCTRL_MAX_INVALIDATE_ITEM_COUNT);
		this->ClearInvalidateItems();
	}
}

// 内部控件在刷新时，与RedrawItem有些不同：
// 1. 此时已有获取到pRenderTarget了，不需要调用begindrawpart. <<-- 但由于还得做一些绘制背景的操作，在这里仍然借用了BeginRedrawObjectPart的实现
// 2. 不需要commit到窗口上面
// 3. 不需要再绘制innerctrl，只需要listitem内容
void ListCtrlBase::RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, ListItemBase* pItem)
{
	if (NULL == pRenderTarget || NULL == pItem)
		return;

	IRenderChain* pRenderChain = m_pIListCtrlBase->GetRenderChain();
	if (NULL == pRenderChain)
	{
		m_pIListCtrlBase->UpdateObject();
		return;
	}

	if (!IsItemVisibleInScreen(pItem))
		return;

	bool bIsDrawing = false;  // 如果为true，表示当前有其它对象已经调用了begindraw，一般是列表控件内部的控件刷新导致
	RenderContext renderContext(NULL, false);
	IRenderTarget* pRenderTargetRet = pRenderChain->BeginRedrawObjectPart(m_pIListCtrlBase, NULL, 0, &bIsDrawing, &renderContext);
	UIASSERT(bIsDrawing);   // 因为知道这里必定为true，所以不需要再调用EndRedrawObjectPart

	this->OnDrawItem(pRenderTargetRet, pItem);
    pItem->Draw(pRenderTarget, &renderContext); 

// 	ISelectRegionRender* pSelectRegionRender = m_pMouseMgr->GetSelectRegionRender();
// 	if (pSelectRegionRender)
// 		pSelectRegionRender->PrePaint();
}
void  ListCtrlBase::RedrawItem(ListItemBase** ppItemArray, int nCount)
{
	if (NULL == ppItemArray && 0 == nCount)
		return;

	for (int i = 0; i < nCount; i++)  // 检测一下是否全是NULL
	{
		if (ppItemArray[i] == NULL)
		{
			nCount = i;
			break;
		}
	}
	if (0 == nCount)
		return;

	IRenderChain* pRenderChain = m_pIListCtrlBase->GetRenderChain();
	if (NULL == pRenderChain)
	{
		m_pIListCtrlBase->UpdateObject();
		return;
	}

    if (!this->IsVisible())
        return;

	CRect* pRectArray = new CRect[nCount];
    int nRealCount = 0;
	for (int i = 0; i < nCount; i++)
	{
		if (ppItemArray[i])
		{
            if (!IsItemVisibleInScreen(ppItemArray[i]))
                continue;

			this->ItemRect2WindowRect(ppItemArray[i]->GetParentRect(), &(pRectArray[nRealCount]));
            nRealCount++;
		}
	}
    if (0 == nRealCount)
    {
        SAFE_ARRAY_DELETE(pRectArray);
        return;
    }

	bool bIsDrawing = false;  // 如果为true，表示当前有其它对象已经调用了begindraw，一般是列表控件内部的控件刷新导致
	RenderContext roc(NULL,false);
	IRenderTarget* pRenderTarget = pRenderChain->BeginRedrawObjectPart(m_pIListCtrlBase, pRectArray, nRealCount, &bIsDrawing, &roc);
	if (NULL == pRenderTarget)
	{
		SAFE_ARRAY_DELETE(pRectArray);
		return;
	}

	for (int i = 0; i < nCount; i++)
	{
		ListItemBase* pItem = ppItemArray[i];
		if (NULL == pItem)
			continue;

		this->OnDrawItem(pRenderTarget, pItem);
        pItem->Draw(pRenderTarget, &roc);

		if (!bIsDrawing)
			pItem->DrawItemInnerControl(pRenderTarget, &roc);
	}

// 	ISelectRegionRender* pSelectRegionRender = m_pMouseMgr->GetSelectRegionRender();
// 	if (pSelectRegionRender)
// 		pSelectRegionRender->PrePaint();

	if (!bIsDrawing)
		pRenderChain->EndRedrawObjectPart(pRenderTarget, pRectArray, nRealCount);

	SAFE_ARRAY_DELETE(pRectArray);
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// 编辑控件
#if 0 // -- 架构改造
Edit*     ListCtrlBase::GetEditControl()
{
	if (m_pEdit)
		return m_pEdit;

	UICreateInstance(&m_pEdit, m_pUIApplication);
	if (NULL == m_pEdit)
		return NULL;

	m_pEdit->SetOutRef((Object**)&m_pEdit);
	m_pEdit->SetNotify(this, LISTCTRLBASE_EDIT_CONTROL_MSG_ID);

	this->SetChildObjectAttribute(m_pEdit, XML_LISTCTRL_EDIT_PRIFIX, m_mapAttribute, false);
	return NULL;
}
Combobox* ListCtrlBase::GetComboboxControl()
{
	if (m_pCombobox)
		return m_pCombobox;

	UICreateInstance(&m_pCombobox, m_pUIApplication);
	if (NULL == m_pCombobox)
		return NULL;

	m_pCombobox->SetOutRef((Object**)&m_pCombobox);
	m_pCombobox->SetNotify(this, LISTCTRLBASE_EDIT_CONTROL_MSG_ID);

	this->SetChildObjectAttribute(m_pCombobox, XML_LISTCTRL_COMBOBOX_PRIFIX, m_mapAttribute, false);

	return NULL;
}
Button*   ListCtrlBase::GetButtonControl()
{
	if (m_pButton)
		return m_pButton;

	UICreateInstance(&m_pButton, m_pUIApplication);
	if (NULL == m_pButton)
		return NULL;
	m_pButton->SetOutRef((Object**)&m_pButton);

	m_pButton->SetNotify(this, LISTCTRLBASE_EDIT_CONTROL_MSG_ID);

	this->SetChildObjectAttribute(m_pButton, XML_LISTCTRL_BUTTON_PRIFIX, m_mapAttribute, false);
    return NULL;
}
#endif

const TCHAR* ListCtrlBase::GetItemText(ListItemBase* pItem)
{
	if (NULL == pItem)
		return NULL;
	
	return pItem->GetText();
}

int  ListCtrlBase::GetVisibleItemCount()
{
    int nRet = 0;
    ListItemBase*  pItem = FindVisibleItemFrom(NULL);
    while (pItem)
    {
        nRet ++;
        pItem = pItem->GetNextVisibleItem();
    }
    return nRet;
}

// item share data op
IListItemTypeShareData*  ListCtrlBase::GetItemTypeShareData(int lType)
{
    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.find(lType);
    if (iter != m_mapItemTypeShareData.end())
        return iter->second;

    return NULL;
}
void  ListCtrlBase::SetItemTypeShareData(int lType, IListItemTypeShareData* pData)
{
    m_mapItemTypeShareData[lType] = pData;
    
    // 设置属性
    if (pData)
    {
        pData->GetImpl()->SetListCtrlBase(this);
        if (this->m_pIMapAttributeRemain)
        {
			SERIALIZEDATA data = {0};
			data.pMapAttrib = m_pIMapAttributeRemain;
			data.nFlag = SERIALIZEFLAG_LOAD;
			UISendMessage(pData, UI_WM_SERIALIZE, (WPARAM)&data);
//          UISendMessage(pData, UI_WM_SETATTRIBUTE, (WPARAM)this->m_pIMapAttributeRemain, 0);
        }
    }
}
void  ListCtrlBase::RemoveItemTypeShareData(int lType)
{
    map<int, IListItemTypeShareData*>::iterator iter = m_mapItemTypeShareData.find(lType);
    if (iter != m_mapItemTypeShareData.end())
    {
        m_mapItemTypeShareData.erase(iter);
    }
}

// pt为窗口坐标
ListItemBase* ListCtrlBase::HitTest(POINT ptWindow)
{
    CRect rcClient;
    m_pIListCtrlBase->GetClientRect(&rcClient);

    // 1. 转换为内部坐标

    POINT pt;
    m_pIListCtrlBase->WindowPoint2ObjectPoint(&ptWindow, &pt);
    if (FALSE == rcClient.PtInRect(pt))
        return NULL;

    m_pIListCtrlBase->ObjectPoint2ObjectClientPoint(&pt, &pt);

    int nxOffset=0, nyOffset=0;
    m_pIListCtrlBase->GetScrollOffset(&nxOffset, &nyOffset);
    pt.x += nxOffset;
    pt.y += nyOffset;

    // 2. 判断

    ListItemBase* p = m_pFirstDrawItem;
    while (p)
    {
        if (PtInRect(p->GetParentRect(), pt))
            return p;

        if (p == m_pLastDrawItem)
            break;

        p = p->GetNextVisibleItem();
    }

    return NULL;
}

// pt为控件内部坐标
LRESULT  ListCtrlBase::OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt = {GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam) };

    ListItemBase* p = m_pFirstDrawItem;
    if (!p && m_bNeedCalcFirstLastVisibleItem)
    {
        CalcFirstLastVisibleItem();
        m_bNeedCalcFirstLastVisibleItem= false;
    }
    while (p)
    {
        CRect rcParent;
        p->GetParentRect(&rcParent);

        if (PtInRect(&rcParent, pt))
        {
            return (LRESULT)p->GetIListItemBase();
        }
        if (p == m_pLastDrawItem)
            break;

        p = p->GetNextVisibleItem();
    }

    return NULL;
}
void  ListCtrlBase::SetFocusRender(IRenderBase* p)
{
    SAFE_RELEASE(m_pFocusRender);
    m_pFocusRender = p;

    if (m_pFocusRender)
        m_pFocusRender->AddRef();
}


#pragma region // tree 

bool  ListCtrlBase::InsertItem(ListItemBase* pNewItem, IListItemBase* pParent, IListItemBase* pAfter, int nInsertFlags)
{
    if (NULL == pParent)
        pParent = UITVI_ROOT;
    if (NULL == pAfter)
        pAfter = UITVI_LAST;
    if (UITVI_ROOT == pAfter)
    {
        pParent = UITVI_ROOT;
        pAfter = UITVI_FIRST;
    }

    // 现在pParent可取值: ROOT | xxx
    //      hAfter可取值 : FIRST | LAST | xxx
    bool bRet = false;
    do
    {
        if (UITVI_ROOT == pParent)
        {
            if (UITVI_FIRST == pAfter)
            {
                bRet = _InsertRoot(pNewItem);
            }
            else if (UITVI_LAST == pAfter)
            {
                ListItemBase* pLastRoot = GetLastItem();
                if (NULL == pLastRoot)
                    bRet = _InsertRoot(pNewItem);
                else
                    bRet = _InsertAfter(pNewItem, pLastRoot);
            }
            else
            {
                if (pAfter->GetParentItem() != NULL)  // 不是根结点
                    break;

                bRet = _InsertAfter(pNewItem, pAfter?pAfter->GetImpl():NULL);
            }
        }
        else
        {
            if (UITVI_FIRST == pAfter)
            {
                bRet = _InsertFirstChild(pNewItem, pParent?pParent->GetImpl():NULL);
            }
            else if (UITVI_LAST == pAfter)
            {
                bRet = _InsertLastChild(pNewItem, pParent?pParent->GetImpl():NULL);
            }
            else
            {
                if (pAfter->GetParentItem() != pParent)  // 不是对应子结点
                    break;

                bRet = _InsertAfter(pNewItem, pAfter?pAfter->GetImpl():NULL);
            }
        }
    }
    while(0);

    if (false == bRet)
    {
        return false;
    }
    else
    {
        m_nItemCount++;
        SetCalcFirstLastVisibleItemFlag();
        pNewItem->SetIListCtrlBase(m_pIListCtrlBase);
    }
    
    UpdateItemIndex(pNewItem);
    UISendMessage(pNewItem->GetIListItemBase(), UI_WM_INITIALIZE);

    if (0 == (nInsertFlags & LISTITEM_OPFLAG_NOUPDATEITEMRECT))
    {
        UpdateItemRect(pNewItem, false);
    }
    if (0 == (nInsertFlags & LISTITEM_OPFLAG_NOREDRAW))
    {
        m_pIListCtrlBase->UpdateObject();
    }

    UIMSG  msg;
    msg.pMsgTo = m_pIListCtrlBase;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_ITEMADD;
    msg.wParam = (WPARAM)pNewItem->GetIListItemBase();
    msg.lParam = nInsertFlags;
    UISendMessage(&msg);

    return true;
}

bool ListCtrlBase::_InsertRoot(ListItemBase* pNewItem)
{
    if (NULL == m_pFirstItem)
    {
        m_pFirstItem = pNewItem;
        m_pLastItem = pNewItem;
    }
    else
    {
        pNewItem->SetNextItem(m_pFirstItem);
        m_pFirstItem->SetPrevItem(pNewItem);
        m_pFirstItem = pNewItem;
    }
    return true;
}

bool ListCtrlBase::_InsertAfter(ListItemBase* pNewItem, ListItemBase* pInsertAfter)
{
    if (NULL == pInsertAfter)
        return false;

    ListItemBase* pOldNext = pInsertAfter->GetNextItem();
    pInsertAfter->SetNextItem(pNewItem);
    pNewItem->SetPrevItem(pInsertAfter);

    if (NULL != pOldNext)
    {
        pOldNext->SetPrevItem(pNewItem);
        pNewItem->SetNextItem(pOldNext);
    }
    if (m_pLastItem == pInsertAfter)
    {
        m_pLastItem = pNewItem;
    }

    return true;
}

bool ListCtrlBase::_InsertFront(ListItemBase* pNewItem, ListItemBase* pInsertFront)
{
    if (NULL == pInsertFront)
        return false;

    ListItemBase* pOldPrev = pInsertFront->GetPrevItem();
    pInsertFront->SetPrevItem(pNewItem);
    pNewItem->SetNextItem(pInsertFront);

    if (NULL != pOldPrev)
    {
        pOldPrev->SetNextItem(pNewItem);
        pNewItem->SetPrevItem(pOldPrev);
    }
    if (m_pLastItem == pInsertFront)
    {
        m_pLastItem = pNewItem;
    }
    if (m_pFirstItem == pInsertFront)
    {
        m_pFirstItem = pNewItem;
    }

    return true;
}

bool ListCtrlBase::_InsertFirstChild(ListItemBase* pNewItem, ListItemBase* pParent)
{
    if (NULL == pParent)
        return false;

	pParent->AddChildFront(pNewItem);
    return true;
}

bool ListCtrlBase::_InsertLastChild(ListItemBase* pNewItem, ListItemBase* pParent)
{
    if (NULL == pParent)
        return false;

    if (NULL == pParent->GetChildItem())
        return this->_InsertFirstChild(pNewItem, pParent);

    pParent->AddChild(pNewItem);
    return true;
}


void ListCtrlBase::ToggleItemExpand(ListItemBase* pItem, bool bUpdate)
{
    if (NULL == pItem)
        return;

    if (pItem->IsExpand())
        CollapseItem(pItem, bUpdate);
    else
        ExpandItem(pItem, bUpdate);
}

void ListCtrlBase::CollapseItem(ListItemBase* pItem, bool bUpdate)
{
    if (NULL == pItem)
        return ;

    if (pItem->IsCollapsed())
        return;

    pItem->SetExpand(false);

    if (NULL == pItem->GetChildItem())
        return;

    // 判断当前选择项是否被隐藏
    if (NULL != m_pFirstSelectedItem)
    {
        if (pItem->IsMyChildItem(GetFirstSelectItem(), true))
        {
            SelectItem(pItem, false);  // 将选择项转移到父结点
        }
    }
    if (NULL != m_pFocusItem)
    {
        if (pItem->IsMyChildItem(m_pFocusItem, true))
        {
            SetFocusItem(pItem);
        }
    }

    this->SetCalcFirstLastVisibleItemFlag();
    this->SetInvalidateAllItems();  // PS: 有些对象经折叠后被隐藏了，是不能再调用RedrawItem的，因此也必须全部刷新
    this->UpdateItemIndex(NULL);
    this->UpdateItemRect(m_pFirstItem, bUpdate);
}
void ListCtrlBase::ExpandItem(ListItemBase* pItem, bool bUpdate)
{
    if (NULL == pItem)
        return ;

    bool bDoExpandAction = false;
    ListItemBase* pParent = pItem;
    while (pParent)
    {
        if (pParent->IsExpand())
        {
            pParent = pParent->GetParentItem();    
            continue;
        }
         
        bDoExpandAction = true;
        pParent->SetExpand(true);
        pParent = pParent->GetParentItem();    
    }
    
    if (bDoExpandAction)
    {
        this->SetCalcFirstLastVisibleItemFlag();
        this->SetInvalidateAllItems();
        this->UpdateItemIndex(NULL);
        this->UpdateItemRect(m_pFirstItem, bUpdate);
    }
}

#pragma endregion

LRESULT  ListCtrlBase::OnGetMouseKeyboardMgr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (NULL == m_pMKMgr)
        return NULL;

    return (LRESULT)static_cast<IMKMgr*>(m_pMKMgr);
}

BOOL  ListCtrlBase::ProcessItemMKMessage(UIMSG* pMsg, ListItemBase* pItem)
{
    if (NULL == m_pMKMgr)
        return FALSE;

    return m_pMKMgr->ProcessItemMessage(pMsg, pItem);
}

LRESULT  ListCtrlBase::OnDelayRemoveItem(WPARAM w, LPARAM l)
{
    RemoveItem((ListItemBase*)w, (int)l);
    return 0;
}

}