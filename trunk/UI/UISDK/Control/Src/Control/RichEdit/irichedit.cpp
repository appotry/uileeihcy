#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Control\Src\Control\RichEdit\richedit.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IRichEdit, RichEdit, IControl)

bool  IRichEdit::SetText(const TCHAR* szText) { return m_pRichEditImpl->SetText(szText); }

LONG  IRichEdit::GetSelBarWidth() { return m_pRichEditImpl->GetSelBarWidth(); }
LONG  IRichEdit::SetSelBarWidth(LONG l_SelBarWidth) { return m_pRichEditImpl->SetSelBarWidth(l_SelBarWidth); }

bool  IRichEdit::GetRichTextFlag() { return m_pRichEditImpl->GetRichTextFlag(); }
void  IRichEdit::SetRichTextFlag(bool b) { m_pRichEditImpl->SetRichTextFlag(b); }
void  IRichEdit::SetMultiLine(bool b)
{
    m_pRichEditImpl->GetRichEdit().SetMultiLine(b);
}
void  IRichEdit::SetAutoResize(bool b, int nMaxSize)
{
    m_pRichEditImpl->GetRichEdit().SetAutoResize(b, nMaxSize);
}
}