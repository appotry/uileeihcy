#include "stdafx.h"
#include "apicaret.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{
ApiCaret::ApiCaret()
{
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
}
ApiCaret::~ApiCaret()
{

}

BOOL  ApiCaret::Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
    m_pObject = pObj;
    m_hWnd = hWndParent;

    m_sizeCaret.cx = nWidth;
    m_sizeCaret.cy = nHeight;
    return ::CreateCaret(hWndParent, hbmp, nWidth, nHeight);
}
void  ApiCaret::Destroy()
{
    ::DestroyCaret();
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
}
void  ApiCaret::Show()
{
    ::ShowCaret(m_hWnd);
}
void  ApiCaret::Hide()
{
    ::HideCaret(m_hWnd);
}
void  ApiCaret::SetPos(int x, int y)
{
    m_ptLast.x = x;
    m_ptLast.y = y;

    if (m_pObject)
    {
        // ƫ��
        POINT ptCtrl = m_pObject->GetRealPosInWindow(); 

        x += ptCtrl.x;
        y += ptCtrl.y;

        // �ü� -- ̫�����ˣ���ownderdrawcaretʵ�ְ�.
//         CRect  rcVisible;
//         m_pObject->GetObjectVisibleRect(&rcVisible, true);
// 
//         CRect  rcCaret = {x, y, x+m_sizeCaret.cx, y+m_sizeCaret.cy};
//         // ��ȫ���ص����
//         if (rcCaret.left > rcVisible.right ||
//             rcCaret.right < rcVisible.left ||
//             rcCaret.bottom < rcVisible.top ||
//             rcCaret.top > rcVisible.bottom)
//         {
//         }
    }

    ::SetCaretPos(x, y);
}


// �޷��жϵ�ǰ�������ʾ�Ļ������صģ�����PostCommitWindowBuffer����ǿ�ƽ������ʾ������
// Ҳ���ǵ���дһ���Լ���caret�źý��������
// void  ApiCaret::PreCommitWindowBuffer(HDC hMemDC)
// {
//     Hide();  
// }
// void  ApiCaret::PostCommitWindowBuffer(HDC hMemDC)
// {
//     SetPos(m_ptLast.x, m_ptLast.y);
//     Show();
// }
}