#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Kernel\Src\UIObject\ScrollBar\scrollbarmanager.h"

namespace UI
{
IScrollBarManager::IScrollBarManager()
{
    m_pImpl = new ScrollBarManager(this); 
}
IScrollBarManager::~IScrollBarManager()
{
    SAFE_DELETE(m_pImpl);
}

ScrollBarManager*  IScrollBarManager::GetImpl()                       { return m_pImpl; }

void  IScrollBarManager::SetBindObject(IObject* pBindObj)             { m_pImpl->SetBindObject(pBindObj); }
IObject*  IScrollBarManager::GetBindObject()                          { return m_pImpl->GetBindObject(); }
void  IScrollBarManager::SetHScrollBar(IMessage* p)                   { m_pImpl->SetHScrollBar(p); }
void  IScrollBarManager::SetVScrollBar(IMessage* p)                   { m_pImpl->SetVScrollBar(p); }
IMessage*  IScrollBarManager::GetHScrollBar()                         { return m_pImpl->GetHScrollBar(); }
IMessage*  IScrollBarManager::GetVScrollBar()                         { return m_pImpl->GetVScrollBar(); }

void  IScrollBarManager::SetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eDirType, SCROLLBAR_VISIBLE_TYPE eVisType)
{ m_pImpl->SetScrollBarVisibleType(eDirType, eVisType); }
SCROLLBAR_VISIBLE_TYPE  IScrollBarManager::GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType)
{ return m_pImpl->GetScrollBarVisibleType(eType); }
void  IScrollBarManager::FireScrollMessage(SCROLLBAR_DIRECTION_TYPE eType, int nCode, int nTrackPos) 
{ m_pImpl->FireScrollMessage(eType, nCode, nTrackPos); }

BOOL  IScrollBarManager::virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) { return m_pImpl->nvProcessMessage(pMsg, nMsgMapID, bDoHook); }
void  IScrollBarManager::ResetAttribute()                             { m_pImpl->ResetAttribute(); }
void  IScrollBarManager::SetAttribute(IMapAttribute* pMapAttrib, bool bReload) { m_pImpl->SetAttribute(pMapAttrib, bReload); }

void  IScrollBarManager::SetHScrollPos(int nPos)                      { m_pImpl->SetHScrollPos(nPos); }
void  IScrollBarManager::SetVScrollPos(int nPos)                      { m_pImpl->SetVScrollPos(nPos); }

void  IScrollBarManager::SetVScrollLine(int nLine)                    { m_pImpl->SetVScrollLine(nLine); }
void  IScrollBarManager::SetHScrollLine(int nLine)                    { m_pImpl->SetHScrollLine(nLine); }
void  IScrollBarManager::SetVScrollWheel(int nWheel)                  { m_pImpl->SetVScrollWheel(nWheel); }
void  IScrollBarManager::SetHScrollWheel(int nWheel)                  { m_pImpl->SetHScrollWheel(nWheel); }

void  IScrollBarManager::SetScrollRange(int nX, int nY)               { m_pImpl->SetScrollRange(nX, nY); }
void  IScrollBarManager::SetHScrollRange(int nX)                      { m_pImpl->SetHScrollRange(nX); }
void  IScrollBarManager::SetVScrollRange(int nY)                      { m_pImpl->SetVScrollRange(nY); }
void  IScrollBarManager::SetScrollPageAndRange(const SIZE* pPage, const SIZE* pRange) { m_pImpl->SetScrollPageAndRange(pPage, pRange); }

void  IScrollBarManager::GetScrollInfo(SCROLLBAR_DIRECTION_TYPE eDirType, UISCROLLINFO* pInfo)  { m_pImpl->GetScrollInfo(eDirType, pInfo); }

void  IScrollBarManager::GetScrollPos(int* pX, int* pY)               { m_pImpl->GetScrollPos(pX, pY);}
int   IScrollBarManager::GetScrollPos(SCROLLBAR_DIRECTION_TYPE eDirType) { return m_pImpl->GetScrollPos(eDirType); }
int   IScrollBarManager::GetHScrollPos()                              { return m_pImpl->GetHScrollPos(); }
int   IScrollBarManager::GetVScrollPos()                              { return m_pImpl->GetVScrollPos(); }

void  IScrollBarManager::GetScrollPage(int* pX, int* pY)              { m_pImpl->GetScrollPage(pX, pY); }
int   IScrollBarManager::GetScrollPage(SCROLLBAR_DIRECTION_TYPE eDirType) { return m_pImpl->GetScrollPage(eDirType); }
int   IScrollBarManager::GetHScrollPage()                             { return m_pImpl->GetHScrollPage(); }
int   IScrollBarManager::GetVScrollPage()                             { return m_pImpl->GetVScrollPage(); }

void  IScrollBarManager::GetScrollRange(int *pX, int* pY)             { m_pImpl->GetScrollRange(pX, pY); }
int   IScrollBarManager::GetScrollRange(SCROLLBAR_DIRECTION_TYPE eDirType) { return m_pImpl->GetScrollRange(eDirType); }
int   IScrollBarManager::GetHScrollRange()                            { return m_pImpl->GetHScrollRange(); }
int   IScrollBarManager::GetVScrollRange()                            { return m_pImpl->GetVScrollRange(); }
}