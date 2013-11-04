#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "listitembase.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IListItemBase, ListItemBase, Message);

void  IListItemBase::SetIListCtrlBase(IListCtrlBase* p)           { m_pListItemBaseImpl->SetIListCtrlBase(p); }
IListCtrlBase*  IListItemBase::GetIListCtrlBase()                 { return m_pListItemBaseImpl->GetIListCtrlBase(); }

IListItemBase*  IListItemBase::GetNextItem() 
{ 
    ListItemBase* p = m_pListItemBaseImpl->GetNextItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}

IListItemBase*  IListItemBase::GetPrevItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetPrevItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListItemBase::GetParentItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetParentItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListItemBase::GetChildItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetChildItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListItemBase::GetPrevSelection()
{
    ListItemBase* p = m_pListItemBaseImpl->GetPrevSelection();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListItemBase::GetNextSelection()
{
    ListItemBase* p = m_pListItemBaseImpl->GetNextSelection();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}

IListItemBase*  IListItemBase::GetNextVisibleItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetNextVisibleItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListItemBase::GetPrevVisibleItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetPrevVisibleItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListItemBase::GetNextTreeItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetNextTreeItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
IListItemBase*  IListItemBase::GetPrevTreeItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetPrevTreeItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}

IListItemBase*  IListItemBase::GetLastChildItem()
{
    ListItemBase* p = m_pListItemBaseImpl->GetLastChildItem();
    if (p)
        return p->GetIListItemBase();

    return NULL;
}
bool  IListItemBase::IsMyChildItem(IListItemBase* pChild, bool bTestGrandChildren)
{
    if (NULL == pChild)
        return false;

    return m_pListItemBaseImpl->IsMyChildItem(pChild->GetImpl(), bTestGrandChildren);
}

bool  IListItemBase::HasChild() { return m_pListItemBaseImpl->HasChild(); }

void  IListItemBase::SetNextItem(IListItemBase* p)
{
    ListItemBase* pListItem = NULL;
    if (p)
        pListItem = p->GetImpl();

    m_pListItemBaseImpl->SetNextItem(pListItem);

}
void  IListItemBase::SetPrevItem(IListItemBase* p)
{
    ListItemBase* pListItem = NULL;
    if (p)
        pListItem = p->GetImpl();

    m_pListItemBaseImpl->SetPrevItem(pListItem);
}
void  IListItemBase::SetParentItem(IListItemBase* p)
{
    ListItemBase* pListItem = NULL;
    if (p)
        pListItem = p->GetImpl();

    m_pListItemBaseImpl->SetParentItem(pListItem);
}
void  IListItemBase::SetChildItem(IListItemBase* p)
{
    ListItemBase* pListItem = NULL;
    if (p)
        pListItem = p->GetImpl();

    m_pListItemBaseImpl->SetChildItem(pListItem);
}
void  IListItemBase::AddChild(IListItemBase* p)
{
	ListItemBase* pListItem = NULL;
	if (p)
		pListItem = p->GetImpl();

	m_pListItemBaseImpl->AddChild(pListItem);
}
void  IListItemBase::AddChildFront(IListItemBase* p)
{
	ListItemBase* pListItem = NULL;
	if (p)
		pListItem = p->GetImpl();

	m_pListItemBaseImpl->AddChildFront(pListItem);
}
void  IListItemBase::SetNextSelection(IListItemBase* p)
{
    ListItemBase* pListItem = NULL;
    if (p)
        pListItem = p->GetImpl();

    m_pListItemBaseImpl->SetNextSelection(pListItem);
}
void  IListItemBase::SetPrevSelection(IListItemBase* p)
{
    ListItemBase* pListItem = NULL;
    if (p)
        pListItem = p->GetImpl();

    m_pListItemBaseImpl->SetPrevSelection(pListItem);
}

const CRect*  IListItemBase::GetParentRect()                                  { return m_pListItemBaseImpl->GetParentRect(); }
void  IListItemBase::GetParentRect(CRect* prc)                                { m_pListItemBaseImpl->GetParentRect(prc); }
void  IListItemBase::SetParentRect(CRect* prc)                                { m_pListItemBaseImpl->SetParentRect(prc); }

int   IListItemBase::GetItemType()                                            { return m_pListItemBaseImpl->GetItemType(); }
void  IListItemBase::SetItemType(long n)                                      { m_pListItemBaseImpl->SetItemType(n); }
long  IListItemBase::GetId()                                                  { return m_pListItemBaseImpl->GetId(); }
void  IListItemBase::SetId(long n)                                            { return m_pListItemBaseImpl->SetId(n); }
void  IListItemBase::SetText(const TCHAR* szText)                             { m_pListItemBaseImpl->SetText(szText); }
const TCHAR*  IListItemBase::GetText()                                        { return m_pListItemBaseImpl->GetText(); }
void  IListItemBase::SetToolTip(const TCHAR* szText)                          { return m_pListItemBaseImpl->SetToolTip(szText); }
UINT  IListItemBase::GetItemFlag()                                            { return m_pListItemBaseImpl->GetItemFlag(); }
void  IListItemBase::SetItemFlag(int n)                                       { m_pListItemBaseImpl->SetItemFlag(n); }
const TCHAR*  IListItemBase::GetToolTip()                                     { return m_pListItemBaseImpl->GetToolTip(); }
LPARAM  IListItemBase::GetData()                                              { return m_pListItemBaseImpl->GetData(); }
void  IListItemBase::SetData(LPARAM p)                                        { m_pListItemBaseImpl->SetData(p); }
LPARAM  IListItemBase::GetCtrlData()                                          { return m_pListItemBaseImpl->GetCtrlData(); }
void  IListItemBase::SetCtrlData(LPARAM p)                                    { m_pListItemBaseImpl->SetCtrlData(p); }
void  IListItemBase::GetDesiredSize(SIZE* pSize)                              { UISendMessage(this, UI_WM_GETDESIREDSIZE, (WPARAM)pSize); }
int   IListItemBase::GetLineIndex()                                           { return m_pListItemBaseImpl->GetLineIndex(); }
void  IListItemBase::SetLineIndex(int n)                                      { m_pListItemBaseImpl->SetLineIndex(n); }
int   IListItemBase::GetTreeIndex()                                           { return m_pListItemBaseImpl->GetTreeIndex(); }
int   IListItemBase::GetNeighbourIndex()                                      { return m_pListItemBaseImpl->GetNeighbourIndex(); }
int   IListItemBase::GetDepth()                                               { return m_pListItemBaseImpl->GetDepth(); }
void  IListItemBase::SetTreeIndex(int n)                                      { m_pListItemBaseImpl->SetTreeIndex(n); }
void  IListItemBase::SetNeighbourIndex(int n)                                 { m_pListItemBaseImpl->SetNeighbourIndex(n); }
void  IListItemBase::SetDepth(int n)                                          { m_pListItemBaseImpl->SetDepth(n); }
void  IListItemBase::SetConfigWidth(int n)                                    { m_pListItemBaseImpl->SetConfigWidth(n); }
void  IListItemBase::SetConfigHeight(int n)                                   { m_pListItemBaseImpl->SetConfigHeight(n); }

IRenderBase*  IListItemBase::GetIconRender()                                  { return m_pListItemBaseImpl->GetIconRender(); }
void  IListItemBase::SetIconRender(IRenderBase* p)                            { m_pListItemBaseImpl->SetIconRender(p); }
void  IListItemBase::SetIconRenderType(RENDER_TYPE eType)                     { m_pListItemBaseImpl->SetIconRenderType(eType); }
void  IListItemBase::SetIconFromFile(const TCHAR* szIconPath)                 { m_pListItemBaseImpl->SetIconFromFile(szIconPath); }

UINT  IListItemBase::GetItemState()                                           { return m_pListItemBaseImpl->GetItemState(); }
bool  IListItemBase::IsMySelfVisible()                                        { return m_pListItemBaseImpl->IsMySelfVisible(); }
bool  IListItemBase::IsVisible()                                              { return m_pListItemBaseImpl->IsVisible(); }
bool  IListItemBase::IsDisable()                                              { return m_pListItemBaseImpl->IsDisable(); }
bool  IListItemBase::IsHover()                                                { return m_pListItemBaseImpl->IsHover(); }
bool  IListItemBase::IsPress()                                                { return m_pListItemBaseImpl->IsPress(); }
bool  IListItemBase::IsFocus()                                                { return m_pListItemBaseImpl->IsFocus(); }
bool  IListItemBase::IsCollapsed()                                            { return m_pListItemBaseImpl->IsCollapsed(); }
bool  IListItemBase::IsExpand()                                               { return m_pListItemBaseImpl->IsExpand(); }
bool  IListItemBase::IsSelected()                                             { return m_pListItemBaseImpl->IsSelected(); }
bool  IListItemBase::IsChecked()                                              { return m_pListItemBaseImpl->IsChecked(); }
bool  IListItemBase::IsRadioChecked()                                         { return m_pListItemBaseImpl->IsRadioChecked(); }
void  IListItemBase::SetPress(bool b, bool bNotify)                           { m_pListItemBaseImpl->SetPress(b, bNotify); }
void  IListItemBase::SetHover(bool b, bool bNotify)                           { m_pListItemBaseImpl->SetHover(b, bNotify); }
void  IListItemBase::SetExpand(bool b, bool bNotify)                          { m_pListItemBaseImpl->SetExpand(b, bNotify); }
void  IListItemBase::SetFocus(bool b, bool bNotify)                           { m_pListItemBaseImpl->SetFocus(b, bNotify); }
void  IListItemBase::SetSelected(bool b, bool bNotify)                        { m_pListItemBaseImpl->SetSelected(b, bNotify); }
void  IListItemBase::SetDisable(bool b, bool bNotify)                         { m_pListItemBaseImpl->SetDisable(b, bNotify); }
void  IListItemBase::SetChecked(bool b, bool bNotify)                         { m_pListItemBaseImpl->SetChecked(b, bNotify); }
void  IListItemBase::SetRadioChecked(bool b, bool bNotify)                    { m_pListItemBaseImpl->SetRadioChecked(b, bNotify); }

IPanel*  IListItemBase::GetRootPanel()                                        { return m_pListItemBaseImpl->GetRootPanel(); }
bool  IListItemBase::CreateRootPanel()                                        { return m_pListItemBaseImpl->CreateRootPanel(); }
void  IListItemBase::DrawItemInnerControl(IRenderTarget* pRenderTarget, RenderContext* prenderContext)       { m_pListItemBaseImpl->DrawItemInnerControl(pRenderTarget, prenderContext); }
int   IListItemBase::GetItemRenderState()                                     { return m_pListItemBaseImpl->GetItemRenderState(); }
UINT  IListItemBase::GetItemDelayOp()                                         { return m_pListItemBaseImpl->GetItemDelayOp(); }
void  IListItemBase::AddItemDelayOp(int n)                                    {  m_pListItemBaseImpl->AddItemDelayOp(n); }
void  IListItemBase::RemoveDelayOp(int n)                                     {  m_pListItemBaseImpl->RemoveDelayOp(n); }
void  IListItemBase::ClearDelayOp()                                           {  m_pListItemBaseImpl->ClearDelayOp(); }
}