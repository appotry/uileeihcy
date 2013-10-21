#pragma once
#include "UISDK\Control\Src\Control\Caret\icaret.h"
#include "UISDK\Kernel\Inc\Util\layeredwindow.h"

namespace UI
{

//
// ����������4������⣺ʹ�ô�����Ϊ�����ڱ༭�����������֣����᲻�ϵ���˸!
//
// 1. ֻҪ��setwindowpos move or size/show hide window���ͻᵼ�µ�ǰ����µĴ��ڽ��ܵ�һ��NCHITTEST/SETCURSOR/MOUSEMOVE����Ϣ��
//    �������Ƿ���һ���ֲ㴰��
// 2. ͨ�����ý�CCaretWindow�ֲ��ο�ʵ�ֵ����أ����Դﵽ��������������Ϣ������setcaretposҪ��ô�������ķ�����ʵ�֣�
// 3. Mouse Vanish  -- û�ã��������޸Ĳ��ˣ������ڿ������--��������ø�ѡ��Ҳ��������
// 4. Ӧ�ò���ʹ��showcursorʵ�ֵ����ع�꣬��������ƶ�ʱ�Ͳ�����ʾ��
// 5. �ѵ���setcursor(null)? ��ô�ƽ⣿
//
//    --> ������Ϊ�ڲ�ֱ�ӵ��õ�SetCursor(NULL)���µ�
//
//
//  ע: 20130701ΪʲôҪʹ�÷ֲ㴰����Ϊ��꣺
//      ����Combobox�ؼ�����ʹ����ͨ������Ϊ���ʱ�������Edit�в�ͣ������/��ʾ������ϵͳ
//      �᲻ͣ�ĸ���ǰ����µĴ��ڷ���WM_MOUSEMOVE��Ϣ���Ӷ�����listbox��ǰ��hover�
//
class CaretWindow : public ICaret,   // ע����ICaret����CWindowImpl<CCaretWindow>���棬�ᵼ�¶����ͷ�ʱ������ԭ��δ֪
                     public CWindowImpl<CaretWindow>
{
public:
    CaretWindow(void);
    ~CaretWindow(void);

     DECLARE_WND_CLASS(_T("CaretWindow"))	
 
    BEGIN_MSG_MAP_EX(CaretWindow)
         MSG_WM_CREATE(OnCreate)
//         MSG_WM_MOVE(OnMove)
         MSG_WM_DESTROY(OnDestroy)
         MSG_WM_PAINT(OnPaint)
         MSG_WM_ERASEBKGND(OnEraseBkgnd)
         MSG_WM_TIMER(OnTimer)
         MESSAGE_HANDLER(UI_WM_SYNC_WINDOWPOSCHANING, OnSyncWindowPosChanging);
    END_MSG_MAP()

protected:
     int   OnCreate(LPCREATESTRUCT lpCreateStruct);
     void  OnDestroy();
     void  OnPaint(HDC hDC);
     BOOL  OnEraseBkgnd(HDC hDC);
     void  OnTimer(UINT_PTR nIDEvent);
     void  OnMove(CPoint ptPos);
     virtual void  OnControlPaint(IRenderTarget* p){};
     LRESULT OnSyncWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
    virtual CARET_TYPE  GetType() { return CARET_TYPE_WINDOW; }
    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
    virtual void  Destroy(bool bRedraw);
    virtual void  SetPos(int x, int y, bool bRedraw);
    virtual void  Show(bool bRedraw);
    virtual void  Hide(bool bRedraw);
    virtual IObject*  GetObject() { return m_pObject; }

    void  OnWindowMove();
    void  OnControlMove();
    void  DrawCaretBitmap(HBITMAP hbmp, HDC hDstDC, HBITMAP hDstBmp, int nWidht, int nHeight);

protected:
    void  _SetPos(int x, int y);
public:
    HWND   m_hCtrlWnd;
    IObject*  m_pObject;
    POINT  m_ptLast;   // �������������ƶ���ʱ��ά�ֹ����EDIT�е�λ��

    bool   m_bShow;    // �Ƿ���ʾ
    bool   m_bVisible; // ��˸״̬
    bool   m_bTestEditVisibleEveryTime;   // ���ڽ��Edit������������
private:
    CLayeredWindow*  m_pLayeredWnd;
};

}