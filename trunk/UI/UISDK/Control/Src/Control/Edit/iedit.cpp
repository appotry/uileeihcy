#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Src\Control\Edit\edit.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IEdit, Edit, IControl)

const TCHAR*  IEdit::GetText() { return m_pEditImpl->GetText(); }
void  IEdit::SetText(const TCHAR* szText) { m_pEditImpl->SetText(szText); }
void  IEdit::SetSel(int nStartChar, int nEndChar) { m_pEditImpl->SetSel(nStartChar, nEndChar); }
void  IEdit::GetSel(int& nStartChar,int& nEndChar) const { m_pEditImpl->GetSel(nStartChar, nEndChar); }

}