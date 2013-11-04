#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(IWindowBase, WindowBase, WindowRenderLayer)
UI_IMPLEMENT_Ixxx_INTERFACE(IWindow, Window, WindowBase)

HWND  IWindowBase::GetHWND()                                               { return m_pWindowBaseImpl->GetHWND(); }
BOOL  IWindowBase::IsChildWindow()                                         { return m_pWindowBaseImpl->IsChildWindow(); }
void  IWindowBase::ShowWindow()                                            { m_pWindowBaseImpl->ShowWindow(); }
void  IWindowBase::HideWindow()                                            { m_pWindowBaseImpl->HideWindow(); }
void  IWindowBase::CenterWindow(HWND hWndCenter)                           { m_pWindowBaseImpl->CenterWindow(hWndCenter); }
void  IWindowBase::UpdateDesktopLayout()                                   { m_pWindowBaseImpl->UpdateDesktopLayout(); }
HDC   IWindowBase::GetRenderChainMemDC()                                   { return m_pWindowBaseImpl->GetRenderChainMemDC(); }
bool  IWindowBase::IsDoModal()                                             { return m_pWindowBaseImpl->IsDoModal(); }
IRenderChain*  IWindowBase::GetIRenderChain()                              { return m_pWindowBaseImpl->GetIRenderChain(); }

void    IWindowBase::OnObjectDeleteInd(IObject* p) 
{ 
    if (p)
        m_pWindowBaseImpl->OnObjectDeleteInd(p->GetImpl()); 
}
void  IWindowBase::OnObjectHideInd(IObject* p)   
{ 
    if (p)
        m_pWindowBaseImpl->OnObjectHideInd(p->GetImpl()); 
}

void   IWindowBase::SetFocusObject(IObject* pObj)
{
    Object*  p = NULL;
    if (pObj)
        p = pObj->GetImpl();

    m_pWindowBaseImpl->SetFocusObject(p);
}

IObject*  IWindowBase::GetHoverObject()
{
    Object* p = m_pWindowBaseImpl->GetHoverObject();
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IWindowBase::GetPressObject()
{
    Object* p = m_pWindowBaseImpl->GetPressObject();
    if (p)
        return p->GetIObject();
    return NULL;
}
IObject*  IWindowBase::GetObjectByPos(IObject* pObjParent, POINT* pt, bool bSkinBuilderInvoke)
{ 
    if (NULL == pObjParent)
        return NULL;

    Object* pRet = m_pWindowBaseImpl->GetObjectByPos(pObjParent->GetImpl(), pt, bSkinBuilderInvoke); 
    if (pRet)
        return pRet->GetIObject();

    return NULL;
}
bool  IWindowBase::Create(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndParent) {return m_pWindowBaseImpl->Create(pUIApp, szID, hWndParent);}
void  IWindowBase::Attach(IUIApplication* pUIApp, HWND hWnd, const TCHAR* szID) { m_pWindowBaseImpl->Attach(pUIApp, hWnd, szID); }
void  IWindowBase::Detach() { m_pWindowBaseImpl->Detach(); }
long  IWindowBase::DoModal(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndParent ) { return m_pWindowBaseImpl->DoModal(pUIApp, szID, hWndParent); }
long  IWindowBase::DoModal(IUIApplication* pUIApp, HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent) { return m_pWindowBaseImpl->DoModal(pUIApp, hResInst, nResID, szID, hWndParent); }
HWND  IWindowBase::DoModeless(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndParent ) { return m_pWindowBaseImpl->DoModeless(pUIApp, szID, hWndParent); }
HWND  IWindowBase::DoModeless(IUIApplication* pUIApp, HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent) { return m_pWindowBaseImpl->DoModeless(pUIApp, hResInst, nResID, szID, hWndParent); }
void  IWindowBase::EndDialog(INT_PTR n) { return m_pWindowBaseImpl->EndDialog(n); }

void  IWindowBase::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount) { m_pWindowBaseImpl->CommitDoubleBuffet2Window(hDCWnd, prcCommit, nRectCount); }
void  IWindowBase::CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow ) { m_pWindowBaseImpl->CalcWindowSizeByClientSize(sizeClient, pSizeWindow); }
void  IWindowBase::CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient ) { m_pWindowBaseImpl->CalcClientRectByWindowRect(rcWindow, rcClient); }
void  IWindowBase::SaveMemBitmap(TCHAR* szFile) { m_pWindowBaseImpl->SaveMemBitmap(szFile); }
void  IWindowBase::DrawMemBitmap(HDC hDC, RECT* prc) { m_pWindowBaseImpl->DrawMemBitmap(hDC, prc); }
void  IWindowBase::AddCommitWindowBufferListener(ICommitWindowBufferListener* p) { m_pWindowBaseImpl->AddCommitWindowBufferListener(p); }
void  IWindowBase::RemoveCommitWindowBufferListener(ICommitWindowBufferListener* p) { m_pWindowBaseImpl->RemoveCommitWindowBufferListener(p); }

bool  IWindowBase::AddAnchorItem(const SyncWindowData& data){ return m_pWindowBaseImpl->AddAnchorItem(data); }
void  IWindowBase::HideAllAnchorItem() { m_pWindowBaseImpl->HideAllAnchorItem(); }

HRESULT  IWindowBase::SetCanDrop(bool b) { return m_pWindowBaseImpl->SetCanDrop(b); }
}