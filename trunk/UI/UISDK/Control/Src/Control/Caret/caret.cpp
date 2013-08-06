#include "StdAfx.h"
#include "caret.h"
#include "api\apicaret.h"
#include "ownerdraw\ownerdrawcaret.h"
#include "window\windowcaret.h"
#include "controldraw\controldrawcaret.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{

CCaret::CCaret()
{ 
    m_pCaret = NULL;
    m_ptLast.x = m_ptLast.y = 0;
}
CCaret::~CCaret()
{
    SAFE_DELETE(m_pCaret);
}

BOOL CCaret::CreateCaret(IObject* pObj, HBITMAP hbmp, int nWidth, int nHeight, CARET_TYPE eType)
{
    if (NULL == pObj)
        return FALSE;

    HWND hWnd = pObj->GetHWND();
#if 1
    eType = CARET_TYPE_CONTROLDRAW;
    // TODO:
    // api和window都存在剪裁区的问题
    // api还涉及到同一个窗口上面两个EDIT相互切换的问题
    // 以后再解决这些问题了，先用WINDOW吧
#elif 1
    if (CARET_TYPE_UNKNOWN == eType)
    {
        if (Util::IsLayeredWindow(hWnd))
            eType = CARET_TYPE_WINDOW;
    }
#endif
    SAFE_DELETE(m_pCaret);
    switch (eType)
    {
//     case CARET_TYPE_API:
//         {
//             m_pCaret = new ApiCaret;
//         }
//         break;

    case CARET_TYPE_WINDOW:
        {
            m_pCaret = new CaretWindow;
        }
        break;

//     case CARET_TYPE_OWNERDRAW:
//         {
//             m_pCaret = new OwnerDrawCaret;
//         }
//         break;

    case CARET_TYPE_CONTROLDRAW:
        {
            m_pCaret = new ControlDrawCaret;
        }
        break;

    default:
        break;
    }

    if (m_pCaret)
    {
        return m_pCaret->Create(pObj, hWnd, hbmp, nWidth, nHeight);
    }

    return TRUE;
}
void CCaret::DestroyCaret()
{
    if (m_pCaret)
    {
        m_pCaret->Destroy();
        SAFE_DELETE(m_pCaret);
    }
}

// 2013.6.17 将SetCaretPos的参数修改为相对于控件，而不是窗口
// 这样才好解决Edit滚动的问题
void CCaret::SetCaretPos(int x, int y, bool bRedraw)
{
    if (m_pCaret)
        m_pCaret->SetPos(x, y, bRedraw);

    m_ptLast.x = x;
    m_ptLast.y = y;
}
void CCaret::ShowCaret(bool bRedraw)
{
    if (m_pCaret)
        m_pCaret->Show(bRedraw);
}
void CCaret::HideCaret(bool bRedraw)
{
    if (m_pCaret)
        m_pCaret->Hide(bRedraw);
}
void  CCaret::OnControlPaint(IRenderTarget* p)
{
    if (m_pCaret)
        m_pCaret->OnControlPaint(p);
}

CARET_TYPE  CCaret::GetCaretType()
{
    if (m_pCaret)
        return m_pCaret->GetType();
    
    return CARET_TYPE_UNKNOWN;
}

}