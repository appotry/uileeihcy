#include "stdafx.h"
#include "listbox.h"
#include "listboxitem.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"

namespace UI
{

bool ListBoxCompareProc(IListItemBase* p1, IListItemBase* p2);

ListBoxBase::ListBoxBase()
{
    m_pIListBoxBase = NULL;
    m_pBindObject = NULL;
	
}
ListBoxBase::~ListBoxBase()
{
}
HRESULT  ListBoxBase::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(IListBoxBase, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIListBoxBase->SetSortCompareProc(ListBoxCompareProc);
    return S_OK;
}

void ListBoxBase::SetBindObject(IObject* pCombobox)
{
    m_pBindObject = pCombobox;
}

bool  ListBoxCompareProc(IListItemBase* pItem1, IListItemBase* pItem2)
{
	//return( pItem1->m_strText.compare(pItem2->m_strText) );  // <-- 注：该compare不支持中文拼音的排序
	return (_tcscoll(pItem1->GetText(), pItem2->GetText()) < 0);
}


bool ListBoxBase::SetSel(int nIndex)
{
	IListItemBase* pOldSelItem = m_pIListBoxBase->GetFirstSelectItem();

	// 清除当前选项
	if (-1 == nIndex && NULL != pOldSelItem)
	{
		m_pIListBoxBase->SelectItem(NULL, false);
	}
	else
	{
		IListItemBase* pItem = m_pIListBoxBase->GetItemByPos(nIndex);
		if (false == pItem)
			return false;

		m_pIListBoxBase->SelectItem(pItem, false);
	}

	m_pIListBoxBase->Refresh();
	return true;
}

int  ListBoxBase::GetListBoxStyle()
{
    return GETCONTROLSUBTYPE(m_pIListBoxBase->GetStyleEx());
}
void ListBoxBase::SetListBoxStyle(int n)
{
    m_pIListBoxBase->SetStyleEx(MAKECONTROLSUBTYPE(m_pIListBoxBase->GetStyleEx(), n));
}

bool  ListBoxBase::IsPopup()
{
    return m_pIListBoxBase->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE);
}

void ListBoxBase::ResetAttribute()
{
	DO_PARENT_PROCESS(IListBoxBase, IListCtrlBase);

    if (m_pIListBoxBase->GetIScrollBarMgr())
    {
	    m_pIListBoxBase->GetIScrollBarMgr()->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_AUTO/*SCROLLBAR_VISIBLE_NONE*/);
	    m_pIListBoxBase->GetIScrollBarMgr()->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);
    }

//    m_pIPopupListBox->ModifyStyleEx(LISTCTRLBASE_STYLE_SORT_ASCEND|LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE, 0, 0);
}
void ListBoxBase::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    IUIApplication* pUIApplication = m_pIListBoxBase->GetUIApplication();
    bool bPopup = IsPopup();

    // 当使用theme主题并且没有定义border时，将border默认设置为2
    bool bBorderConfiged = false;
    if (pMapAttrib->GetAttr(XML_BORDER, false))
        bBorderConfiged = true;

    const TCHAR* szText = pMapAttrib->GetAttr(XML_TEXTRENDER_TYPE, false);
	if (NULL == szText)
	{
		if (NULL == m_pIListBoxBase->GetTextRender())
		{
			IRenderFont* pRenderFont = m_pIListBoxBase->GetRenderFont();
            ITextRenderBase* pTextRender = NULL;
            pUIApplication->CreateTextRenderBase(TEXTRENDER_TYPE_COLORLIST, m_pIListBoxBase, &pTextRender);
            if (pTextRender)
            {
                IColorListTextRender* pColorListTextRender = (IColorListTextRender*)UISendMessage(pTextRender, UI_WM_QUERYINTERFACE, (WPARAM)&uiiidof(IColorListTextRender));
                if (pColorListTextRender)
                {
                    pColorListTextRender->SetCount(9);
                    pColorListTextRender->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL,  RGBA(255,255,255,255));
                    pColorListTextRender->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER,   RGBA(255,255,255,255));
                    pColorListTextRender->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS,   RGBA(255,255,255,255));
                    pColorListTextRender->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE, RGBA(255,255,255,255));
//                    pColorListTextRender->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS, RGB(255,255,255));
                
                    pColorListTextRender->SetRenderFont(pRenderFont);
                    pColorListTextRender->SetTextAlignment(DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS|DT_NOPREFIX);
                }

                pTextRender->SetAttribute(pMapAttrib, NULL, true);
                m_pIListBoxBase->SetTextRender(pTextRender);
                SAFE_RELEASE(pTextRender);
			}
		}
	}

    DO_PARENT_PROCESS(IListBoxBase, IListCtrlBase);

	if (NULL == m_pIListBoxBase->GetBkRender())
	{
        if (bPopup)
        {
            IRenderBase*  pRenderBase = NULL;
            pUIApplication->CreateRenderBase(RENDER_TYPE_COLOR, m_pIListBoxBase, &pRenderBase);
            if (pRenderBase)
            {
                IColorRender*  p = (IColorRender*)pRenderBase->QueryInterface(uiiidof(IColorRender));
                if (p)
                {
                    p->SetBkColor(RGBA(255,255,255,255));
                    p->SetBorderColor(RGBA(0,0,0,255));

                    if (!bBorderConfiged)
                    {
                        CRegion4 r(1,1,1,1);
                        p->SetBorderRegion(&r);
                    }
                }

                m_pIListBoxBase->SetBkgndRender(pRenderBase);
                SAFE_RELEASE(pRenderBase);
            }
        }
        else
        {
            IRenderBase* pRenderBase = NULL;
            pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_LISTBOX_BKGND, m_pIListBoxBase, &pRenderBase);
            if (pRenderBase)
            {
                pRenderBase->SetAttribute(pMapAttrib, XML_BACKGND_RENDER_PREFIX, true);
                m_pIListBoxBase->SetBkgndRender(pRenderBase);
                SAFE_RELEASE(pRenderBase);
            }
        }
        if (!bBorderConfiged)
        {
            CRegion4 r(2,2,2,2);
            m_pIListBoxBase->SetBorderRegion(&r);
        }
	}

	if (NULL == m_pIListBoxBase->GetForeRender())
	{
		if (0 == this->GetListBoxStyle())   // 默认的listbox，不是combobox中的listbox
		{
            IRenderBase* pRenderBase = NULL;
            pUIApplication->CreateRenderBase(RENDER_TYPE_COLORLIST, m_pIListBoxBase, &pRenderBase);
            if (pRenderBase)
            {
                IColorListRender* p = (IColorListRender*)UISendMessage(pRenderBase, UI_WM_QUERYINTERFACE, (WPARAM)&uiiidof(IColorListRender));
                if (p)
                {
                    if (bPopup)
                    {
                        p->SetCount(8);
                        p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER, RGBA(51,153,255,255),true, 0,false);
                        p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER, RGBA(51,153,255,255),true, 0,false);
                    }
                    else
                    {
                        p->SetCount(8);
                        p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL,  RGBA(51,153,255,255),true, 0,false);
                        p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER,   RGBA(51,153,255,255),true, 0,false);
                        p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS,   RGBA(51,153,255,255),true, 0,false);
                        p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE, RGBA(51,153,255,255),true, 0,false);
                    }
                }
                pRenderBase->SetAttribute(pMapAttrib, XML_BACKGND_RENDER_PREFIX, true);
                m_pIListBoxBase->SetForegndRender(pRenderBase);
                SAFE_RELEASE(pRenderBase);
            }
		}
	}

    IScrollBarManager*  pScrollBarMgr = m_pIListBoxBase->GetIScrollBarMgr();
    if (pScrollBarMgr)
    {
        if (!pScrollBarMgr->GetHScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(HSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemHScrollBar*  p = NULL;
            ISystemHScrollBar::CreateInstance(pUIApplication, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListBoxBase->AddNcChild(p);
            p->SetID(XML_HSCROLLBAR_PRIFIX);
            
            p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
        }

        if (!pScrollBarMgr->GetVScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(VSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemVScrollBar*  p = NULL;
            ISystemVScrollBar::CreateInstance(pUIApplication, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pIListBoxBase->AddNcChild(p);
            p->SetID(XML_VSCROLLBAR_PRIFIX);

            p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
            pScrollBarMgr->SetVScrollWheel(20);
        }
    }
}

LRESULT ListBoxBase::OnDrawItem(WPARAM w, LPARAM l)
{
    return 0;
}

LRESULT ListBoxBase::OnGetGraphicsRenderType()
{
    if (m_pBindObject)  // 对于弹出式的listbox，由于窗口是弹出时才创建的，因此listbox向它的owner获取graphics type
    {
        return GetRenderLibraryType(m_pBindObject);
    }
    return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
}


void ListBoxBase::OnInitPopupControlWindow()
{
    m_pIListBoxBase->SetMinWidth(m_pBindObject->GetWidth());  // 限制宽度最小与COMBOBOX一致
    if (NDEF == m_pIListBoxBase->GetMaxHeight())
        m_pIListBoxBase->SetMaxHeight(300);
}

void ListBoxBase::OnUnInitPopupControlWindow()
{
    m_pIListBoxBase->SetHoverItem(NULL);  // 清空被选中的item，以防下次再次弹出时，状态未更新，仍然显示上一次的hover item。（弹出窗口向listbox获取,listbox向combobox获取,combobox其实是向它所在的窗口获取）
}

//////////////////////////////////////////////////////////////////////////

ListBox::ListBox()
{
    m_pIListBox = NULL;
}
ListBox::~ListBox()
{

}

IListBoxItem*  ListBox::AddString(const TCHAR* szText, int nAddItemFlags)
{
    IListBoxItem* pItem = NULL;
    IListBoxItem::CreateInstance(m_pIListBox->GetUIApplication(), &pItem);

    if (NULL == pItem)
        return NULL;

    if (szText)
        pItem->SetText(szText);

    m_pIListBox->AddItem(pItem, nAddItemFlags);
    return pItem;
}
bool ListBox::RemoveItem(IListBoxItem* pItem, int nFlag)
{
    if (NULL == pItem)
        return false;

    m_pIListBox->RemoveItem(pItem, nFlag);
    return true;
}
}