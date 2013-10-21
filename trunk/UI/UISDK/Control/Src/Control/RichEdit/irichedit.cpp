#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Control\Src\Control\RichEdit\richedit.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IRichEdit, RichEdit, IControl)

bool  IRichEdit::SetText(const TCHAR* szText) 
{ 
    return m_pRichEditImpl->GetRichEdit().SetText(szText);
}

int   IRichEdit::GetText(TCHAR* szBuf, int nLen)
{
    return m_pRichEditImpl->GetRichEdit().GetText(szBuf, nLen);
}

bool  IRichEdit::AppendText(const TCHAR* szText, int nSize)
{
    return m_pRichEditImpl->GetRichEdit().AppendText(szText, nSize);
}

bool  IRichEdit::GetEncodeTextW(IBuffer** ppBuffer)
{
    return m_pRichEditImpl->GetRichEdit().GetEncodeTextW(ppBuffer);
}

bool  IRichEdit::AppendEncodeTextW(const TCHAR* szText, int nSize)
{
    return m_pRichEditImpl->GetRichEdit().AppendEncodeTextW(szText, nSize);
}

LONG  IRichEdit::GetSelBarWidth() 
{ 
    return m_pRichEditImpl->GetRichEdit().GetSelBarWidth(); 
}
LONG  IRichEdit::SetSelBarWidth(LONG l_SelBarWidth) 
{ 
    return m_pRichEditImpl->GetRichEdit().SetSelBarWidth(l_SelBarWidth); 
}

bool  IRichEdit::GetRichTextFlag()
{ 
    return m_pRichEditImpl->GetRichEdit().GetRichTextFlag(); 
}
void  IRichEdit::SetRichTextFlag(bool b)
{
    m_pRichEditImpl->GetRichEdit().SetRichTextFlag(b); 
}
void  IRichEdit::SetMultiLine(bool b)
{
    m_pRichEditImpl->GetRichEdit().SetMultiLine(b);
}
bool  IRichEdit::IsReadOnly()
{
    return m_pRichEditImpl->GetRichEdit().IsReadOnly();
}
void  IRichEdit::SetReadOnly(bool fReadOnly)
{
    m_pRichEditImpl->GetRichEdit().SetReadOnly(fReadOnly);
}
void  IRichEdit::SetAutoResize(bool b, int nMaxSize)
{
    m_pRichEditImpl->GetRichEdit().SetAutoResize(b, nMaxSize);
}
bool  IRichEdit::SetCharFormatByLogFont(LOGFONT* plf)
{
    return m_pRichEditImpl->GetRichEdit().SetCharFormatByLogFont(plf);
}
bool  IRichEdit::SetCharFormat(CHARFORMAT2* pcf)
{
    return m_pRichEditImpl->GetRichEdit().SetCharFormat(pcf);
}
void  IRichEdit::GetCharFormat(CHARFORMAT2* pcf)
{
    return m_pRichEditImpl->GetRichEdit().GetCharFormat(pcf);
}
bool  IRichEdit::InsertGif(const TCHAR* szGifFile)
{
    return m_pRichEditImpl->GetRichEdit().InsertGif(szGifFile);
}
bool  IRichEdit::InsertSkinGif(const TCHAR* szGifId)
{
    return m_pRichEditImpl->GetRichEdit().InsertSkinGif(szGifId);
}
}

