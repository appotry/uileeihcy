#ifndef IRICHEDIT_H_7B01BC76_7BCB_4fe3_AE3B_5404D421D2D1
#define IRICHEDIT_H_7B01BC76_7BCB_4fe3_AE3B_5404D421D2D1
#include "UISDK\Kernel\Inc\Interface\icontrol.h"
#include <richedit.h>
namespace UI
{
    
// 向外部转发通知消息
// message: UI_WM_RICHEDIT_TXNOTIFY   
// wParam:  DWORD iNotify
// lParam:  void* pv
// return:  HRESULT
#define UI_WM_RICHEDIT_TXNOTIFY   137181715

class RichEdit;
interface UICTRLAPI IRichEdit : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IRichEdit, RichEdit);

    bool  SetText(const TCHAR* szText);
    int   GetText(TCHAR* szBuf, int nLen);
    bool  AppendText(const TCHAR* szText, int nSize);
    bool  GetEncodeTextW(IBuffer** ppBuffer);
    bool  AppendEncodeTextW(const TCHAR* szText, int nSize);

    LONG  GetSelBarWidth();
    LONG  SetSelBarWidth(LONG l_SelBarWidth);
    bool  GetRichTextFlag();
    void  SetRichTextFlag(bool b);
    void  SetMultiLine(bool b);
    bool  IsReadOnly();
    void  SetReadOnly(bool fReadOnly);
    void  SetAutoResize(bool b, int nMaxSize);
    bool  SetCharFormatByLogFont(LOGFONT* plf);
    bool  SetCharFormat(CHARFORMAT2* pcf);
    void  GetCharFormat(CHARFORMAT2* pcf);
    
    bool  InsertGif(const TCHAR* szGifFile);
    bool  InsertSkinGif(const TCHAR* szGifId);

    
};

}

#endif // IRICHEDIT_H_7B01BC76_7BCB_4fe3_AE3B_5404D421D2D1