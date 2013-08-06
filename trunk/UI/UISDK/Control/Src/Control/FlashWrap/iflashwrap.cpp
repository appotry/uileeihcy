#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iflashwrap.h"
#include "UISDK\Control\Src\Control\FlashWrap\flashwrap.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IFlashWrap, FlashWrap, IControl);

void  IFlashWrap::Pause() { m_pFlashWrapImpl->Pause(); }
void  IFlashWrap::Play() { m_pFlashWrapImpl->Play(); }
void  IFlashWrap::SetFlashUri(BSTR bstr) { m_pFlashWrapImpl->SetFlashUri(bstr); }
HRESULT  IFlashWrap::CallFlashFunction(const TCHAR* szFuncName, const TCHAR* szArgs, BSTR* pbstrRet) 
{
    return m_pFlashWrapImpl->CallFlashFunction(szFuncName, szArgs, pbstrRet);
}

}