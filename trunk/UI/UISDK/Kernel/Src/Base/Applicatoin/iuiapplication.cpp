#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iuiapplication.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\Helper\timer\timermanager.h"

namespace UI
{

IUIApplication::IUIApplication()
{
    m_pImpl = new UIApplication(this);
}
IUIApplication::~IUIApplication()
{
    SAFE_DELETE(m_pImpl);
}
UIApplication*  IUIApplication::GetImpl()  { return m_pImpl; }

HRESULT  IUIApplication::SetSkinDirection(const TCHAR* szDir) { return m_pImpl->SetSkinDirection(szDir); }
HRESULT  IUIApplication::LoadSkin(const TCHAR* szSkinName) { return m_pImpl->LoadSkin(szSkinName); }
HRESULT  IUIApplication::SetLog(ILog* pLog, const TCHAR* szLogXmlPath) { return m_pImpl->SetLog(pLog, szLogXmlPath); }
HRESULT  IUIApplication::GetLog(ILog** ppLog) { return m_pImpl->GetLog(ppLog); }

HRESULT  IUIApplication::SetDesignMode(VARIANT_BOOL b, IUIEditor* pUIBuilder) { return m_pImpl->SetDesignMode(b, pUIBuilder); }
bool     IUIApplication::IsDesignMode() { return m_pImpl->IsDesignMode(); }
IUIEditor*  IUIApplication::GetUIEditorPtr() { return m_pImpl->GetUIEditorPtr(); }

ISkinManager*       IUIApplication::GetSkinMgr()      { return m_pImpl->GetSkinMgr(); }
ITopWindowManager*  IUIApplication::GetTopWindowMgr() { return m_pImpl->GetTopWindowMgr(); }
IAnimateManager*    IUIApplication::GetAnimateMgr()   { return m_pImpl->GetAnimateMgr(); }

HRESULT  IUIApplication::GetWaitForHandlesMgr(IWaitForHandlesMgr** ppMgr) { return m_pImpl->GetWaitForHandlesMgr(ppMgr); }
HRESULT  IUIApplication::GetMessageFilterMgr(IMessageFilterMgr** ppMgr) { return m_pImpl->GetMessageFilterMgr(ppMgr); }

ISkinRes*       IUIApplication::GetActiveSkinRes() { return m_pImpl->GetActiveSkinRes(); }
IImageManager*  IUIApplication::GetActiveSkinImageMgr() { return m_pImpl->GetActiveSkinImageMgr(); }
IImageRes*      IUIApplication::GetActiveSkinImageRes() { return m_pImpl->GetActiveSkinImageRes(); }
ICursorRes*     IUIApplication::GetActiveSkinCursorRes() { return m_pImpl->GetActiveSkinCursorRes(); }
IGifRes*        IUIApplication::GetActiveSkinGifRes() { return m_pImpl->GetActiveSkinGifRes(); }
IFontManager*   IUIApplication::GetActiveSkinFontMgr() { return m_pImpl->GetActiveSkinFontMgr(); }
IFontRes*       IUIApplication::GetActiveSkinFontRes() { return m_pImpl->GetActiveSkinFontRes(); }
IColorManager*  IUIApplication::GetActiveSkinColorMgr() { return m_pImpl->GetActiveSkinColorMgr(); }
IColorRes*      IUIApplication::GetActiveSkinColorRes() { return m_pImpl->GetActiveSkinColorRes(); }
IStyleManager*  IUIApplication::GetActiveSkinStyleMgr() { return m_pImpl->GetActiveSkinStyleMgr(); }
IStyleRes*      IUIApplication::GetActiveSkinStyleRes() { return m_pImpl->GetActiveSkinStyleRes(); }
ILayoutManager* IUIApplication::GetActiveSkinLayoutMgr() { return m_pImpl->GetActiveSkinLayoutMgr(); }
ILayoutRes*     IUIApplication::GetActiveSkinLayoutRes() { return m_pImpl->GetActiveSkinLayoutRes(); }

void     IUIApplication::RestoreRegisterUIObject()       { m_pImpl->RestoreRegisterUIObject(); }
HRESULT  IUIApplication::RegisterLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement func){ return m_pImpl->RegisterLayoutTagParseFunc(szTag, func); }
HRESULT  IUIApplication::GetSkinTagParseFunc(const TCHAR* szTag, funcUIParseSkinElement* pFunc) { return m_pImpl->GetSkinTagParseFunc(szTag, pFunc); }
HRESULT  IUIApplication::GetLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement* pFunc) { return m_pImpl->GetLayoutTagParseFunc(szTag, pFunc); }

HRESULT  IUIApplication::CreateInstanceByName(const TCHAR* szName, IObject** ppOut) { return m_pImpl->CreateInstanceByName(szName, ppOut); }
HRESULT  IUIApplication::CreateInstanceByClsid(REFCLSID clsid, void** ppOut) { return m_pImpl->CreateInstanceByClsid(clsid, ppOut); }
HRESULT  IUIApplication::RegisterUIObjectCreateData(const TCHAR* szName, const TCHAR* szCategory, UINT nObjType, REFCLSID guid, funcUICreateInstancePtr pfun) { return m_pImpl->RegisterUIObjectCreateData(szName, szCategory, nObjType, guid, pfun); }
void     IUIApplication::LoadUIObjectListToToolBox()          { m_pImpl->LoadUIObjectListToToolBox(); }

HRESULT  IUIApplication::RegisterUIRenderBaseCreateData(const TCHAR* szName, int nType, int nControlType, int nControlSubType, funcUICreateRenderBasePtr pfunc) { return m_pImpl->RegisterUIRenderBaseCreateData(szName, nType, nControlType, nControlSubType, pfunc); }
HRESULT  IUIApplication::CreateRenderBaseByName(const TCHAR* szName, IObject* pObject, IRenderBase** ppOut) { return m_pImpl->CreateRenderBaseByName(szName, pObject, ppOut); }
HRESULT  IUIApplication::CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut) { return m_pImpl->CreateRenderBase(nType, pObject, ppOut); }

HRESULT  IUIApplication::RegisterUITextRenderBaseCreateData(const TCHAR* szName, int nType, int nControlType, int nControlSubType, funcUICreateTextRenderBasePtr pfunc) { return m_pImpl->RegisterUITextRenderBaseCreateData(szName, nType, nControlType, nControlSubType, pfunc); }
HRESULT  IUIApplication::CreateTextRenderBaseByName(const TCHAR* szName, IObject* pObject, ITextRenderBase** ppOut) { return m_pImpl->CreateTextRenderBaseByName(szName, pObject, ppOut); }
HRESULT  IUIApplication::CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut) { return m_pImpl->CreateTextRenderBase(nType, pObject, ppOut); }

HRESULT  IUIApplication::RegisterLayoutCreateData(const TCHAR* szName, int nType, funcUICreateLayoutPtr pfuc)
{
    return m_pImpl->RegisterLayoutCreateData(szName, nType, pfuc);
}
HRESULT  IUIApplication::CreateLayoutByName(const TCHAR* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut) 
{ 
    return m_pImpl->CreateLayoutByName(szName, pObject, bCreateDefault, ppOut); 
}
HRESULT  IUIApplication::CreateLayout(int nType, IObject* pObject, ILayout** ppOut)
{
    return m_pImpl->CreateLayout(nType, pObject, ppOut);
}

BOOL     IUIApplication::IsDialogMessage(MSG* pMsg) { return m_pImpl->IsDialogMessage(pMsg); }
HRESULT  IUIApplication::MsgHandleLoop(bool* pbQuitLoopRef) { return m_pImpl->MsgHandleLoop(pbQuitLoopRef); }
HRESULT  IUIApplication::RedrawTopWindows() { return m_pImpl->RedrawTopWindows(); }

HRESULT  IUIApplication::SetTooltipsUI(IToolTipUI* pUI) { return m_pImpl->SetTooltipsUI(pUI); }
HRESULT  IUIApplication::UseInnerTooltipsUI(const TCHAR* szWndID) { return m_pImpl->UseInnerTooltipsUI(szWndID); }
HRESULT  IUIApplication::ShowToolTip(TOOLTIPITEM* pItem) { return m_pImpl->ShowToolTip(pItem); }
HRESULT  IUIApplication::HideToolTip() { return m_pImpl->HideToolTip(); }

UINT_PTR  IUIApplication::SetTimer(int nElapse, IMessage* pNotify)
{
    return TimerHelper::GetInstance()->SetTimer(nElapse, pNotify);
}
UINT_PTR  IUIApplication::SetTimer(int nElapse, TimerItem* pTimerItem)
{
    return TimerHelper::GetInstance()->SetTimer(nElapse, pTimerItem);
}
UINT_PTR  IUIApplication::SetTimerById(int nElapse, int nId, IMessage* pNotify)
{
    return  TimerHelper::GetInstance()->SetTimerById(nElapse, nId, pNotify);
}
void  IUIApplication::KillTimer(UINT_PTR& nTimerID)
{
    TimerHelper::GetInstance()->KillTimer(nTimerID);
}
void  IUIApplication::KillTimerById(int nId, IMessage* pNotify)
{
    TimerHelper::GetInstance()->KillTimerById(nId, pNotify);
}

HDC  IUIApplication::GetCacheDC()
{
    return Image::GetCacheDC();
}
void  IUIApplication::ReleaseCacheDC(HDC hDC)
{
    return Image::ReleaseCacheDC(hDC);
}  
bool     IUIApplication::IsUnderXpOS() { return m_pImpl->IsUnderXpOS(); }
bool     IUIApplication::IsVistaOrWin7etc() { return m_pImpl->IsVistaOrWin7etc(); }

bool     IUIApplication::IsUIObjectAvailable(IMessage* p) { return m_pImpl->IsUIObjectAvailable(p); }
HRESULT  IUIApplication::AddUIObject(IMessage* p) { return m_pImpl->AddUIObject(p); }
HRESULT  IUIApplication::RemoveUIObject(IMessage* p) { return m_pImpl->RemoveUIObject(p); }

HWND     IUIApplication::GetForwardPostMessageWnd() { return m_pImpl->GetForwardPostMessageWnd(); }

}