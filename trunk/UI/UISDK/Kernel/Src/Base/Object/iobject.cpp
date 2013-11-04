#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(IObject, Object, Message)

//////////////////////////////////////////////////////////////////////////

TCHAR*  IObject::GetObjectName()                           { return m_pObjectImpl->GetObjectName(); }
OBJ_TYPE  IObject::GetObjectType()                         { return m_pObjectImpl->GetObjectType(); }
CONTROL_TYPE  IObject::GetObjectExtentType()               { return m_pObjectImpl->GetObjectExtentType(); }

const TCHAR*  IObject::GetId()                             { return m_pObjectImpl->GetId(); }
void  IObject::SetID(const TCHAR* szText)                  { m_pObjectImpl->SetId(szText); }

IWindowBase*  IObject::GetWindowObject() 
{ 
    WindowBase* p = m_pObjectImpl->GetWindowObject();
    if (p)
        return p->GetIWindowBase();
    else
        return NULL;
}
IRenderLayer*  IObject::GetRenderLayer()
{
    RenderLayer* p = m_pObjectImpl->GetRenderLayer();
    if (p)
        return p->GetIRenderLayer();
    else
        return NULL;
}
IRenderChain*  IObject::GetRenderChain()
{
    RenderChain*  p = m_pObjectImpl->GetRenderChain();
    if (p)
        return p->GetIRenderChain();
    else
        return NULL;
}
HWND  IObject::GetHWND()                                    { return m_pObjectImpl->GetHWND(); }
IUIApplication* IObject::GetUIApplication()                 { return m_pObjectImpl->GetUIApplication(); }
void  IObject::SetUIApplication(IUIApplication* p)          { m_pObjectImpl->SetUIApplication(p); }
void  IObject::SetOutRef(void** ppOutRef)                   { m_pObjectImpl->SetOutRef(ppOutRef); }

void  IObject::InitDefaultAttrib()                          { m_pObjectImpl->InitDefaultAttrib(); }
void  IObject::SetObjectPos(int x, int y, int cx, int cy, int nFlag) { m_pObjectImpl->SetObjectPos(x, y, cx, cy, nFlag); }
void  IObject::SetObjectPos(CRect* prc, int nFlag)          { m_pObjectImpl->SetObjectPos(prc, nFlag); }
void  IObject::SetUserData(LPVOID p)                        { m_pObjectImpl->SetUserData(p); }
LPVOID  IObject::GetUserData()                              { return m_pObjectImpl->GetUserData(); }
void  IObject::ModifyStyle(UINT add, UINT remove, bool bNotify) { m_pObjectImpl->ModifyStyle(add, remove, bNotify); }
bool  IObject::TestStyle(UINT nStyle)                       { return m_pObjectImpl->TestStyle(nStyle); }
void  IObject::ModifyStyleEx( UINT add, UINT remove, bool bNotify) { m_pObjectImpl->ModifyStyleEx(add, remove, bNotify); }
bool  IObject::TestStyleEx(UINT nStyleEx)                   { return m_pObjectImpl->TestStyleEx(nStyleEx); }
UINT  IObject::GetStyle()                                   { return m_pObjectImpl->GetStyle(); }
UINT  IObject::GetStyleEx()                                 { return m_pObjectImpl->GetStyleEx(); }
void  IObject::SetStyle(UINT n)                             { return m_pObjectImpl->SetStyle(n); }
void  IObject::SetStyleEx(UINT n)                           { return m_pObjectImpl->SetStyleEx(n); }
bool  IObject::testStateBit(UINT bit)                       { return m_pObjectImpl->testStateBit(bit); }
void  IObject::setStateBit(UINT bit)                        { m_pObjectImpl->setStateBit(bit); }
void  IObject::clearStateBit(UINT bit)                      { m_pObjectImpl->clearStateBit(bit); }
void  IObject::SetCanRedraw(bool bReDraw)                   { m_pObjectImpl->SetCanRedraw(bReDraw); }
bool  IObject::CanRedraw()                                  { return m_pObjectImpl->CanRedraw(); }
IRenderFont*  IObject::GetRenderFont()                      { return m_pObjectImpl->GetRenderFont(); }

int   IObject::GetStateBit()                                { return m_pObjectImpl->GetStateBit(); }
bool  IObject::IsFocus()                                    { return m_pObjectImpl->IsFocus(); }
//void  IObject::SetFocus(bool b)                             { m_pObjectImpl->SetFocus(b); }
bool  IObject::IsTabstop()                                  { return m_pObjectImpl->IsTabstop(); }
bool  IObject::CanTabstop()                                 { return m_pObjectImpl->CanTabstop(); }
bool  IObject::IsGroup()                                    { return m_pObjectImpl->IsGroup(); }
void  IObject::SetTabstop(bool b)                           { m_pObjectImpl->SetTabstop(b); }
void  IObject::SetGroup(bool b)                             { return m_pObjectImpl->SetGroup(b); }
bool  IObject::IsVisible()                                  { return m_pObjectImpl->IsVisible(); }
bool  IObject::IsCollapsed()                                { return m_pObjectImpl->IsCollapsed(); }
bool  IObject::IsMySelfVisible()                            { return m_pObjectImpl->IsMySelfVisible(); }
bool  IObject::IsEnable()                                   { return m_pObjectImpl->IsEnable(); }
void  IObject::SetVisible(bool b, bool bRedraw, bool bUpdateLayout) { m_pObjectImpl->SetVisible(b, bRedraw, bUpdateLayout); }
void  IObject::SetEnable(bool b, bool bUpdateNow) { m_pObjectImpl->SetEnable(b, bUpdateNow); }
bool  IObject::IsDefault()                                  { return m_pObjectImpl->IsDefault(); }
//bool  IObject::IsReadonly()                                 { return m_pObjectImpl->IsReadonly(); }
bool  IObject::IsHover()                                    { return m_pObjectImpl->IsHover(); }
bool  IObject::IsPress()                                    { return m_pObjectImpl->IsPress(); }
bool  IObject::IsForcePress()                               { return m_pObjectImpl->IsForcePress(); }
bool  IObject::IsForceHover()                               { return m_pObjectImpl->IsForceHover(); }
bool  IObject::IsSelected()                                 { return m_pObjectImpl->IsSelected(); }
void  IObject::SetDefault(bool b, bool bNotify)             { m_pObjectImpl->SetDefault(b, bNotify); }
//void  IObject::SetReadonly(bool b)                          { m_pObjectImpl->SetReadonly(b); }
void  IObject::SetForceHover(bool b, bool bNotify)          { m_pObjectImpl->SetForceHover(b, bNotify); }
void  IObject::SetForcePress(bool b, bool bNotify)          { m_pObjectImpl->SetForcePress(b, bNotify); }
void  IObject::SetHover(bool b, bool bNotify)               { m_pObjectImpl->SetHover(b, bNotify); }
void  IObject::SetPress(bool b, bool bNotify)               { m_pObjectImpl->SetPress(b, bNotify); }
void  IObject::SetSelected(bool b, bool bNotify)            { m_pObjectImpl->SetSelected(b, bNotify); }

bool  IObject::IsTransparent()                              { return m_pObjectImpl->IsTransparent(); }
void  IObject::SetTransparent(bool b)                       { m_pObjectImpl->SetTransparent(b); }
bool  IObject::IsNcObject()                                 { return m_pObjectImpl->IsNcObject(); }
void  IObject::SetAsNcObject(bool b)                        { m_pObjectImpl->SetAsNcObject(b); }
bool  IObject::IsRejectMouseMsgAll()                        { return m_pObjectImpl->IsRejectMouseMsgAll(); }
bool  IObject::IsRejectMouseMsgSelf()                       { return m_pObjectImpl->IsRejectMouseMsgSelf(); }

IObject*  IObject::FindChildObject(const TCHAR* szObjId) 
{
    Object* p = m_pObjectImpl->FindChildObject(szObjId); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetParentObject()  
{
    Object* p = m_pObjectImpl->GetParentObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetChildObject() 
{ 
    Object* p = m_pObjectImpl->GetChildObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetNcChildObject() 
{ 
    Object* p = m_pObjectImpl->GetNcChildObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetLastChildObject()
{ 
    Object* p = m_pObjectImpl->GetLastChildObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetNextObject()
{ 
    Object* p = m_pObjectImpl->GetNextObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetPrevObject() 
{
    Object* p = m_pObjectImpl->GetPrevObject();
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IObject::GetRootObject()
{ 
    Object* p = m_pObjectImpl->GetRootObject(); 
    if (p)
        return p->GetIObject();
    return NULL;
}

IObject*  IObject::EnumChildObject(IObject* p)  
{
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}
IObject*  IObject::REnumChildObject(IObject* p) 
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->REnumChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}
IObject*  IObject::EnumNcChildObject(IObject* p) 
{
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumNcChildObject(pObject); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}

IObject*  IObject::EnumAllChildObject(IObject* p) 
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumAllChildObject(pObject);
    if (NULL == pRet)
        return NULL;

    return pRet->GetIObject();
}
IObject*  IObject::EnumParentObject(IObject* p)
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->EnumParentObject(pObject); 
    if (NULL == pRet)
        return NULL;

    return pRet->GetIObject();
}
IObject*  IObject::REnumParentObject(IObject* p)
{ 
    Object* pObject = NULL;
    if (p)
        pObject = p->GetImpl();

    Object* pRet = m_pObjectImpl->REnumParentObject(pObject); 
    if (NULL == pRet)
        return NULL;

    return pRet->GetIObject();
}

IObject*  IObject::GetNextTreeItemObject()
{ 
    Object* p = m_pObjectImpl->GetNextTreeItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetPrevTreeItemObject()
{ 
    Object* p = m_pObjectImpl->GetPrevTreeItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetNextTreeTabstopItemObject() 
{ 
    Object* p = m_pObjectImpl->GetNextTreeTabstopItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetPrevTreeTabstopItemObject() 
{ 
    Object* p =  m_pObjectImpl->GetPrevTreeTabstopItemObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetNextTabObject()
{ 
    Object* p = m_pObjectImpl->GetNextTabObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IObject::GetPrevTabObject()
{
    Object* p = m_pObjectImpl->GetPrevTabObject(); 
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}

void  IObject::SetParentObjectDirect(IObject* p) 
{ 
    if (p)
        m_pObjectImpl->SetParentObjectDirect(p->GetImpl()); 
}
void  IObject::SetChildObjectDirect(IObject* p)
{
    if (p)
        m_pObjectImpl->SetChildObjectDirect(p->GetImpl()); 
}
void  IObject::SetNcChildObjectDirect(IObject* p)
{
    if (p)
        m_pObjectImpl->SetNcChildObjectDirect(p->GetImpl()); 
}
void  IObject::SetNextObjectDirect(IObject* p) 
{
    if (p)
        m_pObjectImpl->SetNextObjectDirect(p->GetImpl()); 
}
void  IObject::SetPrevObjectDirect(IObject* p) 
{
    if (p)
        m_pObjectImpl->SetPrevObjectDirect(p->GetImpl()); 
}

void  IObject::AddChild(IObject* p)
{ 
    if (p)
        m_pObjectImpl->AddChild(p->GetImpl()); 
}
void  IObject::AddNcChild(IObject*p) 
{
    if (p)
        m_pObjectImpl->AddNcChild(p->GetImpl()); 
}
void  IObject::InsertAfter(IObject* pInsertAfter)
{
    Object* p = NULL;

    if (pInsertAfter)
        p = pInsertAfter->GetImpl();

    m_pObjectImpl->InsertAfter(p);
}
void  IObject::InsertBefore(IObject* pInsertBefore)
{
    Object* p = NULL;

    if (pInsertBefore)
        p = pInsertBefore->GetImpl();

    m_pObjectImpl->InsertBefore(p);
}
bool  IObject::IsMyChild(IObject* pChild, bool bFindInGrand) 
{
    if (NULL == pChild)
        return false;

    return m_pObjectImpl->IsMyChild(pChild->GetImpl(), bFindInGrand); 
}
bool  IObject::RemoveChildInTree(IObject* pChild) 
{ 
    if (NULL == pChild)
        return false;

    return m_pObjectImpl->RemoveChildInTree(pChild->GetImpl()); 
}
void  IObject::RemoveMeInTheTree() 
{ 
    m_pObjectImpl->RemoveMeInTheTree(); 
}

void  IObject::UpdateObjectNonClientRegion()
{
    return m_pObjectImpl->UpdateObjectNonClientRegion();
}
void  IObject::Standalone() { m_pObjectImpl->ClearMyTreeRelationOnly(); }
void  IObject::MoveToAsFirstChild() { m_pObjectImpl->MoveToAsFirstChild(); }
void  IObject::MoveToAsLastChild() { m_pObjectImpl->MoveToAsLastChild(); }
bool  IObject::SwapObject(IObject* pObj1, IObject* pObj2)
{
    Object* p1 = NULL;
    Object* p2 = NULL;

    if (pObj1)
        p1 = pObj1->GetImpl();
    if (pObj2)
        p2 = pObj2->GetImpl();

    return m_pObjectImpl->SwapObject(p1, p2);
}

void  IObject::UpdateObject(bool bUpdateNow) { m_pObjectImpl->UpdateObject(bUpdateNow); }
void  IObject::UpdateObjectBkgnd(bool bUpdateNow) { m_pObjectImpl->UpdateObjectBkgnd(bUpdateNow); }
void  IObject::UpdateLayout(bool bUpdate) { m_pObjectImpl->UpdateLayout(bUpdate); }

void  IObject::GetNonClientRegion(CRegion4* prc) { m_pObjectImpl->GetNonClientRegion(prc); }
void  IObject::SetNonClientRegionExcludePaddingBorder(CRegion4* prc)  { m_pObjectImpl->SetNonClientRegionExcludePaddingBorder(prc); }
void  IObject::SetNonClientRegionExcludePaddingBorderL(int n) { m_pObjectImpl->SetNonClientRegionExcludePaddingBorderL(n); }
void  IObject::SetNonClientRegionExcludePaddingBorderT(int n) { m_pObjectImpl->SetNonClientRegionExcludePaddingBorderT(n); }
void  IObject::SetNonClientRegionExcludePaddingBorderR(int n) { m_pObjectImpl->SetNonClientRegionExcludePaddingBorderR(n); }
void  IObject::SetNonClientRegionExcludePaddingBorderB(int n) { m_pObjectImpl->SetNonClientRegionExcludePaddingBorderB(n); }
int  IObject::GetNonClientL(){ return m_pObjectImpl->GetNonClientL(); }
int  IObject::GetNonClientT(){ return m_pObjectImpl->GetNonClientT(); }
int  IObject::GetNonClientR(){ return m_pObjectImpl->GetNonClientR(); }
int  IObject::GetNonClientB(){ return m_pObjectImpl->GetNonClientB(); }
int  IObject::GetNonClientW(){ return m_pObjectImpl->GetNonClientW(); }
int  IObject::GetNonClientH(){ return m_pObjectImpl->GetNonClientH(); }
void  IObject::GetBorderRegion(CRegion4* prc){ m_pObjectImpl->GetBorderRegion(prc); }
int  IObject::GetBorderL() { return m_pObjectImpl->GetBorderL(); }
int  IObject::GetBorderT() { return m_pObjectImpl->GetBorderT(); }
int  IObject::GetBorderR() { return m_pObjectImpl->GetBorderR(); }
int  IObject::GetBorderB() { return m_pObjectImpl->GetBorderB(); }
void  IObject::SetBorderRegion(CRegion4* prc) { m_pObjectImpl->SetBorderRegion(prc); }
void  IObject::GetPaddingRegion(CRegion4* prc){ m_pObjectImpl->GetPaddingRegion(prc); }
void  IObject::SetPaddingRegion(CRegion4* prc){ m_pObjectImpl->SetPaddingRegion(prc); }
int  IObject::GetPaddingL() { return m_pObjectImpl->GetPaddingL(); }
int  IObject::GetPaddingT() { return m_pObjectImpl->GetPaddingT(); }
int  IObject::GetPaddingR() { return m_pObjectImpl->GetPaddingR(); }
int  IObject::GetPaddingB() { return m_pObjectImpl->GetPaddingB(); }
int  IObject::GetPaddingW() { return m_pObjectImpl->GetPaddingW(); }
int  IObject::GetPaddingH() { return m_pObjectImpl->GetPaddingH(); }
void  IObject::SetMarginRegion(CRegion4* prc) { m_pObjectImpl->SetMarginRegion(prc); }
void  IObject::GetMarginRegion(CRegion4* prc) { m_pObjectImpl->GetMarginRegion(prc); }
int  IObject::GetMarginL() { return m_pObjectImpl->GetMarginL(); }
int  IObject::GetMarginT() { return m_pObjectImpl->GetMarginT(); }
int  IObject::GetMarginR() { return m_pObjectImpl->GetMarginR(); }
int  IObject::GetMarginB() { return m_pObjectImpl->GetMarginB(); }
int  IObject::GetMarginW() { return m_pObjectImpl->GetMarginW(); }
int  IObject::GetMarginH() { return m_pObjectImpl->GetMarginH(); }
void  IObject::GetClientRect(CRect* prc)         { m_pObjectImpl->GetClientRect(prc); }
void  IObject::GetClientRectAsWin32(CRect* prc)  { m_pObjectImpl->GetClientRectAsWin32(prc); }
void  IObject::GetClientRectInWindow(CRect* prc) { m_pObjectImpl->GetClientRectInWindow(prc); }
void  IObject::GetParentRect(CRect* prc)         { m_pObjectImpl->GetParentRect(prc); }
int  IObject::GetParentRectL() { return m_pObjectImpl->GetParentRectL(); }
int  IObject::GetParentRectT() { return m_pObjectImpl->GetParentRectT(); }
int  IObject::GetParentRectR() { return m_pObjectImpl->GetParentRectR(); }
int  IObject::GetParentRectB() { return m_pObjectImpl->GetParentRectB(); }

POINT  IObject::GetRealPosInWindow()                                              { return m_pObjectImpl->GetRealPosInWindow(); }
void  IObject::GetWindowRect(CRect* prc)                                          { m_pObjectImpl->GetWindowRect(prc); }
void  IObject::WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj)       { m_pObjectImpl->WindowPoint2ObjectPoint(ptWindow, ptObj); }
void  IObject::WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj) { m_pObjectImpl->WindowPoint2ObjectClientPoint(ptWindow, ptObj); }
void  IObject::WindowPoint2ObjectClientPoint_CalcScroll(const POINT* ptWindow, POINT* ptObj) { m_pObjectImpl->WindowPoint2ObjectClientPoint_CalcScroll(ptWindow, ptObj); }
void  IObject::ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj) { m_pObjectImpl->ObjectPoint2ObjectClientPoint(ptWindow, ptObj); }
void  IObject::ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj)           { m_pObjectImpl->ClientRect2ObjectRect(rcClient, rcObj); }

bool  IObject::GetScrollOffset(int* pxOffset, int* pyOffset) { return m_pObjectImpl->GetScrollOffset(pxOffset, pyOffset); }
bool  IObject::GetScrollRange(int* pxRange, int* pyRange)    { return m_pObjectImpl->GetScrollRange(pxRange, pyRange); }
bool  IObject::GetObjectVisibleRect(RECT* prc, bool bInWindowOrLayer)       { return m_pObjectImpl->GetObjectVisibleRect(prc, bInWindowOrLayer); }
bool  IObject::GetObjectVisibleClientRect(RECT* prc, bool bInWindowOrLayer) { return m_pObjectImpl->GetObjectVisibleClientRect(prc, bInWindowOrLayer); }

int  IObject::GetWidth()             { return m_pObjectImpl->GetWidth(); }
int  IObject::GetHeight()            { return m_pObjectImpl->GetHeight(); }
int  IObject::GetWidthWithMargins()  { return m_pObjectImpl->GetWidthWithMargins(); }
int  IObject::GetHeightWithMargins() { return m_pObjectImpl->GetHeightWithMargins(); }

int   IObject::GetConfigWidth()        { return m_pObjectImpl->GetConfigWidth(); }
int   IObject::GetConfigHeight()       { return m_pObjectImpl->GetConfigHeight(); }
int   IObject::GetConfigLayoutFlags()  { return m_pObjectImpl->GetConfigLayoutFlags(); }
int   IObject::GetConfigLeft()         { return m_pObjectImpl->GetConfigLeft();  }
int   IObject::GetConfigRight()        { return m_pObjectImpl->GetConfigRight(); }
int   IObject::GetConfigTop()          { return m_pObjectImpl->GetConfigTop(); }
int   IObject::GetConfigBottom()       { return m_pObjectImpl->GetConfigBottom(); }
void  IObject::SetConfigWidth(int n)   { m_pObjectImpl->SetConfigWidth(n); }
void  IObject::SetConfigHeight(int n)  { m_pObjectImpl->SetConfigHeight(n); }
void  IObject::SetConfigLayoutFlags(int n) { m_pObjectImpl->SetConfigLayoutFlags(n); }
void  IObject::SetConfigLeft(int n)    { m_pObjectImpl->SetConfigLeft(n); }
void  IObject::SetConfigRight(int n)   { m_pObjectImpl->SetConfigRight(n); }
void  IObject::SetConfigTop(int n)     { m_pObjectImpl->SetConfigTop(n); }
void  IObject::SetConfigBottom(int n)  { m_pObjectImpl->SetConfigBottom(n); }

ITextRenderBase*  IObject::GetTextRender() { return m_pObjectImpl->GetTextRender(); }
IRenderBase*  IObject::GetBkRender()       { return m_pObjectImpl->GetBkRender(); }
IRenderBase*  IObject::GetForeRender()     { return m_pObjectImpl->GetForeRender(); }
void  IObject::SetBkgndRender(IRenderBase* p)    { m_pObjectImpl->SetBkgndRender(p); }
void  IObject::SetForegndRender(IRenderBase* p)  { m_pObjectImpl->SetForegndRender(p); }
void  IObject::SetTextRender(ITextRenderBase* p) { m_pObjectImpl->SetTextRender(p); }

void  IObject::SetAttributeByPrefix(const TCHAR* szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase) { m_pObjectImpl->SetAttributeByPrefix(szPrefix, pMatAttrib, bReload, bErase); }
void  IObject::ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload)   { m_pObjectImpl->ParseStyleAndSetAttribute(pMatAttrib, bReload); }
const TCHAR*  IObject::GetAttribute(const TCHAR* szKey, bool bErase) { return m_pObjectImpl->GetAttribute(szKey, bErase); }
void  IObject::AddAttribute(const TCHAR* szKey, const TCHAR*  szValue) { m_pObjectImpl->AddAttribute(szKey, szValue); }
void  IObject::GetMapAttribute(IMapAttribute** ppMapAttribute) { m_pObjectImpl->GetMapAttribute(ppMapAttribute); }
SIZE  IObject::GetDesiredSize() { return m_pObjectImpl->GetDesiredSize(); }

HBITMAP  IObject::TakeSnapshot() { return m_pObjectImpl->TakeSnapshot(); }
HBITMAP  IObject::TakeBkgndSnapshot() { return m_pObjectImpl->TakeBkgndSnapshot();  }
bool  IObject::SetCursor(const TCHAR* szCursorID) { return m_pObjectImpl->SetCursor(szCursorID); }
bool  IObject::SetMouseCapture(int nNotifyMsgId){ return m_pObjectImpl->SetMouseCapture(nNotifyMsgId); }
bool  IObject::ReleaseMouseCapture(){ return m_pObjectImpl->ReleaseMouseCapture(); }
bool  IObject::SetKeyboardCapture(int nNotifyMsgId){ return m_pObjectImpl->SetKeyboardCapture(nNotifyMsgId); }
bool  IObject::ReleaseKeyboardCapture() { return m_pObjectImpl->ReleaseKeyboardCapture(); }
}
