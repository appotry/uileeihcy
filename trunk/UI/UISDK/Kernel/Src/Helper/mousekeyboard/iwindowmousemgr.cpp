#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iwindowmousemgr.h"
#include "UISDK\Kernel\Src\Helper\mousekeyboard\mousemanager.h"
#include "UISDK\Kernel\Src\Helper\mousekeyboard\keyboardmanager.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{

#if 0
IMouseMgrBase::IMouseMgrBase()
{
    m_pMouseMgrBase = new MouseMgrBase;
}
IMouseMgrBase::~IMouseMgrBase()
{
    SAFE_DELETE(m_pMouseMgrBase);
}

IObject*  IMouseMgrBase::GetHoverObject()
{
    Object* p = m_pMouseMgrBase->GetHoverObject();
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
IObject*  IMouseMgrBase::GetPressObject()
{
    Object* p = m_pMouseMgrBase->GetPressObject();
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}

void  IMouseMgrBase::OnObjectHideInd(IObject* pObj)
{
    Object* p = NULL;
    if (pObj)
        p = pObj->GetImpl();

    m_pMouseMgrBase->OnObjectHideInd(p);
}
void  IMouseMgrBase::OnObjectDeleteInd(IObject* pObj)
{
    Object* p = NULL;
    if (pObj)
        p = pObj->GetImpl();

    m_pMouseMgrBase->OnObjectDeleteInd(p);
}

IObject*  IMouseMgrBase::GetObjectByPos(IObject* pObjParent, POINT* pt, bool bSkinBuilderInvoke)
{
    Object* pParent = NULL;
    if (pObjParent)
        pParent = pObjParent->GetImpl();

    Object* pRet = m_pMouseMgrBase->GetObjectByPos(pParent, pt, bSkinBuilderInvoke);
    if (NULL == pRet)
        return NULL;

    return pRet->GetIObject();
}

void  IMouseMgrBase::SetHoverObject(IObject* pNewHoverObj)
{
    Object* p = NULL;
    if (pNewHoverObj)
        p = pNewHoverObj->GetImpl();

    m_pMouseMgrBase->SetHoverObject(p);
}
void  IMouseMgrBase::SetPressObject(IObject* pNewPressObj)
{
    Object* p = NULL;
    if (pNewPressObj)
        p = pNewPressObj->GetImpl();

    m_pMouseMgrBase->SetPressObject(p);
}
void  IMouseMgrBase::SetHoverObjectDirectNULL() { m_pMouseMgrBase->SetHoverObjectDirectNULL(); }
void  IMouseMgrBase::SetPressObjectDirectNULL() { m_pMouseMgrBase->SetPressObjectDirectNULL(); }

void  IMouseMgrBase::OnMouseMoveBase(IObject* pNewHover, WPARAM wParam, LPARAM lParam)
{
    Object* p = NULL;
    if (pNewHover)
        p = pNewHover->GetImpl();

    m_pMouseMgrBase->OnMouseMoveBase(p, wParam, lParam);
}
void  IMouseMgrBase::OnMouseLeaveBase()
{
    m_pMouseMgrBase->OnMouseLeaveBase();
}
void  IMouseMgrBase::OnLButtonDownBase(WPARAM wParam, LPARAM lParam, BOOL* pbHandled)
{
    m_pMouseMgrBase->OnLButtonDownBase(wParam, lParam, pbHandled);
}
void  IMouseMgrBase::OnLButtonDBClickBase(WPARAM wParam, LPARAM lParam, BOOL* pbHandled)
{
    m_pMouseMgrBase->OnLButtonDBClickBase(wParam, lParam, pbHandled);
}

void  IMouseMgrBase::SetUIApplication(IUIApplication* p, IWindowBase* pWindow) 
{
    if (NULL == pWindow)
        m_pMouseMgrBase->SetUIApplication(p, NULL); 
    else
        m_pMouseMgrBase->SetUIApplication(p, pWindow->GetImpl()); 
}

//////////////////////////////////////////////////////////////////////////

IKeyboardMgrBase::IKeyboardMgrBase()
{
    m_pKeyboardMgrBase = new KeyboardMgrBase;
}   
IKeyboardMgrBase::~IKeyboardMgrBase()
{
    SAFE_DELETE(m_pKeyboardMgrBase);
}

// void  IKeyboardMgrBase::Tab_2_NextControl()
// {
//     m_pKeyboardMgrBase->Tab_2_NextControl();
// }
// void  IKeyboardMgrBase::Tab_2_PrevControl()
// {
//     m_pKeyboardMgrBase->Tab_2_PrevControl();
// }
// 
void  IKeyboardMgrBase::SetFocusObject(IObject*  pObj)
{
    Object* p = NULL;
    if (pObj)
        p = pObj->GetImpl();

    m_pKeyboardMgrBase->SetFocusObject(p);
}
IObject*  IKeyboardMgrBase::GetFocusObject()
{
    Object* p = m_pKeyboardMgrBase->GetFocusObject();
    if (NULL == p)
        return NULL;

    return p->GetIObject();
}
#endif

}