#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\listctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemBase\listitembase.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IListCtrlBase, ListCtrlBase, Control)
UI_IMPLEMENT_Ixxx_INTERFACE(IListItemTypeShareData, ListItemTypeShareData, Message)

bool  IListCtrlBase::AddItem(IListItemBase* pItem, int nAddItemFlags)              
{ 
    if (NULL == pItem)
        return false;
    return m_pListCtrlBaseImpl->AddItem(pItem->GetImpl(), nAddItemFlags); 
}
bool  IListCtrlBase::InsertItem(IListItemBase* pItem, int nPos, int nAddItemFlags) 
{ 
    if (NULL == pItem)
        return false;
    return m_pListCtrlBaseImpl->InsertItem(pItem->GetImpl(), nPos, nAddItemFlags); 
}
bool  IListCtrlBase::InsertItem(IListItemBase* pItem, IListItemBase* pInsertAfter, int nAddItemFlags) 
{ 
    if (NULL == pItem)
        return false;

    ListItemBase*  pAfter = NULL;
    if (pInsertAfter)
        pAfter = pInsertAfter->GetImpl();
    return m_pListCtrlBaseImpl->InsertItem(pItem->GetImpl(), pAfter, nAddItemFlags); 
}
bool  IListCtrlBase::InsertItem(IListItemBase* pItem, IListItemBase* pParent, IListItemBase* pInsertAfter , int nInsertFlags)
{ 
    if (NULL == pItem)
        return false;

    return m_pListCtrlBaseImpl->InsertItem(pItem->GetImpl(), pParent, pInsertAfter, nInsertFlags); 
}
void  IListCtrlBase::RemoveItem(IListItemBase* pItem, int nRemoveFlag)             
{ 
    if (NULL == pItem)
        return ;

    m_pListCtrlBaseImpl->RemoveItem(pItem->GetImpl(), nRemoveFlag); 
}   
void  IListCtrlBase::RemoveAllChildItems(IListItemBase* pParent, int nRemoveFlag)
{
    if (NULL == pParent)
        return;

    m_pListCtrlBaseImpl->RemoveAllChildItems(pParent->GetImpl(), nRemoveFlag);
}

void  IListCtrlBase::RemoveItem(int nIndex, int nRemoveFlag)              { m_pListCtrlBaseImpl->RemoveItem(nIndex, nRemoveFlag); }
void  IListCtrlBase::RemoveAllItem(int nRemoveFlag)                       { m_pListCtrlBaseImpl->RemoveAllItem(nRemoveFlag); }
void  IListCtrlBase::DelayRemoveItem(IListItemBase* pItem, int nRemoveFlag)
{ 
    if (NULL == pItem)
        return;

    m_pListCtrlBaseImpl->DelayRemoveItem(pItem->GetImpl(), nRemoveFlag);
}

int  IListCtrlBase::GetItemCount()                                        { return m_pListCtrlBaseImpl->GetItemCount(); }
int  IListCtrlBase::GetVisibleItemCount()                                 { return m_pListCtrlBaseImpl->GetVisibleItemCount(); }
void  IListCtrlBase::CalcFirstLastVisibleItem()                           { m_pListCtrlBaseImpl->CalcFirstLastVisibleItem(); } 
void  IListCtrlBase::SetCalcFirstLastVisibleItemFlag()                    { m_pListCtrlBaseImpl->SetCalcFirstLastVisibleItemFlag(); }


void  IListCtrlBase::ToggleItemExpand(IListItemBase* pItem, bool bUpdate) 
{ 
    if (!pItem)
        return;
    m_pListCtrlBaseImpl->ToggleItemExpand(pItem->GetImpl(), bUpdate);
}
void  IListCtrlBase::CollapseItem(IListItemBase* pItem, bool bUpdate)     
{
    if (!pItem)
        return;
    m_pListCtrlBaseImpl->CollapseItem(pItem->GetImpl(), bUpdate);
}
void  IListCtrlBase::ExpandItem(IListItemBase* pItem, bool bUpdate)      
{ 
    if (!pItem)
        return;
    m_pListCtrlBaseImpl->ExpandItem(pItem->GetImpl(), bUpdate); 
}


IListItemBase*  IListCtrlBase::GetHoverItem()           
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetHoverItem(); 
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListCtrlBase::GetPressItem()           
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetPressItem(); 
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
void  IListCtrlBase::SetHoverItem(IListItemBase* pItem)
{
    if (pItem)
        m_pListCtrlBaseImpl->SetHoverItem(pItem->GetImpl());
    else
        m_pListCtrlBaseImpl->SetHoverItem(NULL);
}
void  IListCtrlBase::SetPressItem(IListItemBase* pItem)
{
    if (pItem)
        m_pListCtrlBaseImpl->SetPressItem(pItem->GetImpl());
    else
        m_pListCtrlBaseImpl->SetPressItem(NULL);
}
IListItemBase*  IListCtrlBase::GetFocusItem()           
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetFocusItem(); 
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
void  IListCtrlBase::SetFocusItem(IListItemBase* pItem) 
{
    if (pItem)
        m_pListCtrlBaseImpl->SetFocusItem(pItem->GetImpl()); 
    else
        m_pListCtrlBaseImpl->SetFocusItem(NULL); 
}
IObject*  IListCtrlBase::GetHoverObject()                                
{ 
    Object* p = m_pListCtrlBaseImpl->GetHoverObject();
    if (p)
        return p->GetIObject();

    return NULL;
}
IObject*  IListCtrlBase::GetPressObject()                                 
{ 
    Object* p = m_pListCtrlBaseImpl->GetPressObject(); 
    if (p)
        return p->GetIObject();

    return NULL;
}
void  IListCtrlBase::SetFocusObject(IObject* pObj)
{   
    m_pListCtrlBaseImpl->SetFocusObject(pObj?pObj->GetImpl():NULL);
}
IObject*  IListCtrlBase::GetFocusObject()
{
    Object* p = m_pListCtrlBaseImpl->GetFocusObject(); 
    if (p)
        return p->GetIObject();

    return NULL;
}

IListItemBase*  IListCtrlBase::HitTest(POINT ptWindow)         
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->HitTest(ptWindow); 
    if (p)        
        return p->GetIListItemBase();
    else
        return NULL;
}
IScrollBarManager*  IListCtrlBase::GetIScrollBarMgr()                     { return m_pListCtrlBaseImpl->GetIScrollBarMgr(); }
void  IListCtrlBase::SetSortCompareProc(ListItemCompareProc p)            { m_pListCtrlBaseImpl->SetSortCompareProc(p); }
void  IListCtrlBase::Sort() { m_pListCtrlBaseImpl->Sort(); }
void  IListCtrlBase::SwapItemPos(IListItemBase*  p1, IListItemBase* p2) 
{ 
    if (NULL == p1 || NULL == p2)
        return;

    m_pListCtrlBaseImpl->SwapItemPos(p1->GetImpl(), p2->GetImpl()); 
}
void  IListCtrlBase::ItemRect2WindowRect(const CRect* prc, CRect* prcRet) { m_pListCtrlBaseImpl->ItemRect2WindowRect(prc, prcRet); }
IListItemBase*  IListCtrlBase::GetItemByPos(int i, bool bVisibleOnly) 
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetItemByPos(i, bVisibleOnly); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::GetItemById(long lId)
{
    ListItemBase* p = m_pListCtrlBaseImpl->GetItemById(lId); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::FindItemByText(const TCHAR* szText, IListItemBase* pStart)
{
    ListItemBase* pStartImpl = NULL;
    if (pStart)
        pStartImpl = pStart->GetImpl();

    ListItemBase* p = m_pListCtrlBaseImpl->FindItemByText(szText, pStartImpl);
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::FindChildItemByText(const TCHAR* szText, IListItemBase* pParent, IListItemBase* pStart)
{
    ListItemBase* pStartImpl = NULL;
    ListItemBase* pParentImpl = NULL;
    if (pStart)
        pStartImpl = pStart->GetImpl();
    if (pParent)
        pParentImpl = pParent->GetImpl();

    ListItemBase* p = m_pListCtrlBaseImpl->FindChildItemByText(szText, pParentImpl, pStartImpl);
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::GetFirstItem()     
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetFirstItem(); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::GetLastItem()      
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetLastItem(); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::GetFirstDrawItem() 
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetFirstDrawItem(); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::GetLastDrawItem()  
{
    ListItemBase* p = m_pListCtrlBaseImpl->GetLastDrawItem(); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::FindVisibleItemFrom(IListItemBase* pFindFrom) 
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->FindVisibleItemFrom(pFindFrom ? pFindFrom->GetImpl() : NULL); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase* IListCtrlBase::EnumItemByProc(ListItemEnumProc pProc, IListItemBase* pEnumFrom, WPARAM w, LPARAM l)
{
    ListItemBase* pEnumFrom2 = NULL;
    if (pEnumFrom)
        pEnumFrom2 = pEnumFrom->GetImpl();

    ListItemBase* pRet = m_pListCtrlBaseImpl->EnumItemByProc(pProc, pEnumFrom2, w, l);
    if (pRet)
        return pRet->GetIListItemBase();

    return NULL;
}

IListItemBase*  IListCtrlBase::GetFirstSelectItem() 
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetFirstSelectItem(); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}
IListItemBase*  IListCtrlBase::GetLastSelectItem()  
{ 
    ListItemBase* p = m_pListCtrlBaseImpl->GetLastSelectItem(); 
    if (p)
        return p->GetIListItemBase();
    return NULL;
}

IRenderBase*  IListCtrlBase::GetFocusRender()                               { return m_pListCtrlBaseImpl->GetFocusRender(); }
void  IListCtrlBase::SetFocusRender(IRenderBase* p)                         { m_pListCtrlBaseImpl->SetFocusRender(p); }
void  IListCtrlBase::SelectItem(IListItemBase* pItem, bool bUpdate) 
{ m_pListCtrlBaseImpl->SelectItem(pItem ? pItem->GetImpl():NULL, bUpdate); }

void  IListCtrlBase::UpdateItemRect()                                       { m_pListCtrlBaseImpl->UpdateItemRect(); }
void  IListCtrlBase::UpdateItemRect(IListItemBase* pStart, bool bRedraw)       
{ m_pListCtrlBaseImpl->UpdateItemRect(pStart?pStart->GetImpl():NULL, bRedraw); }
void  IListCtrlBase::UpdateItemIndex(IListItemBase* pStart)
{ m_pListCtrlBaseImpl->UpdateItemIndex(pStart?pStart->GetImpl():NULL); }
void  IListCtrlBase::SetLayout(IListCtrlLayout* pLayout)                    { m_pListCtrlBaseImpl->SetLayout(pLayout); }
IListCtrlLayout*  IListCtrlBase::GetLayout()                                { return m_pListCtrlBaseImpl->GetLayout(); }
void  IListCtrlBase::SetLayoutDefaultV()                                    { m_pListCtrlBaseImpl->SetLayoutDefaultV(); }
void  IListCtrlBase::SetLayoutDefaultHV()                                   { m_pListCtrlBaseImpl->SetLayoutDefaultHV(); }

void  IListCtrlBase::InvalidateItem(IListItemBase* pItem) 
{
    if (NULL == pItem)
        return;
    m_pListCtrlBaseImpl->InvalidateItem(pItem->GetImpl());
}
void  IListCtrlBase::SetInvalidateAllItems()                                { m_pListCtrlBaseImpl->SetInvalidateAllItems(); }
void  IListCtrlBase::ClearInvalidateItems()                                 { m_pListCtrlBaseImpl->ClearInvalidateItems(); }
int   IListCtrlBase::GetInvalidateItemCount()                               { return m_pListCtrlBaseImpl->GetInvalidateItemCount(); }
void  IListCtrlBase::Refresh() { m_pListCtrlBaseImpl->Refresh(); }
void  IListCtrlBase::RedrawItem(IListItemBase** ppItemArray, int nCount) 
{
    if (nCount < 0)
        return;

    ListItemBase** pp = new ListItemBase*[nCount];
    for (int i = 0; i < nCount; i++)
    {
        if (ppItemArray[i])
        {
            pp[i] = ppItemArray[i]->GetImpl();
        }
    }
    m_pListCtrlBaseImpl->RedrawItem(pp, nCount);
    SAFE_ARRAY_DELETE(pp);
}
void  IListCtrlBase::RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, IListItemBase* pItem) 
{ m_pListCtrlBaseImpl->RedrawItemByInnerCtrl(pRenderTarget, pItem?pItem->GetImpl():NULL); }
void  IListCtrlBase::MakeItemVisible(IListItemBase* pItem, bool* pbNeedUpdate) 
{ m_pListCtrlBaseImpl->MakeItemVisible(pItem?pItem->GetImpl():NULL, pbNeedUpdate); }

IListItemTypeShareData*  IListCtrlBase::GetItemTypeShareData(int lType)     { return m_pListCtrlBaseImpl->GetItemTypeShareData(lType); }
void  IListCtrlBase::SetItemTypeShareData(int lType, IListItemTypeShareData* pData) { m_pListCtrlBaseImpl->SetItemTypeShareData(lType, pData); }
void  IListCtrlBase::RemoveItemTypeShareData(int lType)                   { m_pListCtrlBaseImpl->RemoveItemTypeShareData(lType); }

SIZE  IListCtrlBase::GetAdaptWidthHeight(int nWidth, int nHeight)           { return m_pListCtrlBaseImpl->GetAdaptWidthHeight(nWidth, nHeight); }
short  IListCtrlBase::GetVertSpacing()                                      {  return m_pListCtrlBaseImpl->GetVertSpacing(); }
short  IListCtrlBase::GetHorzSpacing()                                      { return m_pListCtrlBaseImpl->GetHorzSpacing(); }

int   IListCtrlBase::GetChildNodeIndent()                                   { return m_pListCtrlBaseImpl->GetChildNodeIndent(); }
void  IListCtrlBase::SetChildNodeIndent(int n)                              { m_pListCtrlBaseImpl->SetChildNodeIndent(n); }

void  IListCtrlBase::SetMinWidth(int n)                                     { m_pListCtrlBaseImpl->SetMinWidth(n); }
void  IListCtrlBase::SetMinHeight(int n)                                    { m_pListCtrlBaseImpl->SetMinHeight(n); }
void  IListCtrlBase::SetMaxWidth(int n)                                     { m_pListCtrlBaseImpl->SetMaxWidth(n); }
void  IListCtrlBase::SetMaxHeight(int n)                                    { m_pListCtrlBaseImpl->SetMaxHeight(n); }
int   IListCtrlBase::GetMinWidth()                                          { return m_pListCtrlBaseImpl->GetMinWidth(); } 
int   IListCtrlBase::GetMinHeight()                                         { return m_pListCtrlBaseImpl->GetMinHeight(); }
int   IListCtrlBase::GetMaxWidth()                                          { return m_pListCtrlBaseImpl->GetMaxWidth(); } 
int   IListCtrlBase::GetMaxHeight()                                         { return m_pListCtrlBaseImpl->GetMaxHeight(); }

int  IListCtrlBase::GetItemHeight()                                         { return m_pListCtrlBaseImpl->GetItemHeight(); }
void  IListCtrlBase::SetItemHeight(int nHeight, bool bUpdate)               { m_pListCtrlBaseImpl->SetItemHeight(nHeight, bUpdate); }

void  IListCtrlBase::Scroll2Y(int nY, bool bUpdate)                         { m_pListCtrlBaseImpl->Scroll2Y(nY, bUpdate); }
void  IListCtrlBase::Scroll2X(int nX, bool bUpdate)                         { m_pListCtrlBaseImpl->Scroll2X(nX, bUpdate); }
void  IListCtrlBase::ScrollY(int nY, bool bUpdate)                          { m_pListCtrlBaseImpl->ScrollY(nY, bUpdate); }
void  IListCtrlBase::ScrollX(int nX, bool bUpdate)                          { m_pListCtrlBaseImpl->ScrollX(nX, bUpdate); }
void  IListCtrlBase::SetScrollPos(int nX, int nY, bool bUpdate)             { m_pListCtrlBaseImpl->SetScrollPos(nX, nY, bUpdate); }

IListCtrlBase*  IListItemTypeShareData::GetListCtrlBase()                   { return m_pListItemTypeShareDataImpl->GetListCtrlBase(); }
}