#pragma once
#include "UISDK\Control\Src\Control\Caret\icaret.h"
#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{

// �ɿؼ��Լ����û��ƣ�ÿ��timerʱ����ˢ�������ؼ���
// �������͵Ĺ��һ��ֻ������Ҫ������Edit/RichEdit���棬
// ��������Ʋü������⡣���ÿ����˸����һ���Ŀ���

class ControlDrawCaret : public ICaret
{
public:
    ControlDrawCaret();
    ~ControlDrawCaret();

    virtual CARET_TYPE  GetType() { return CARET_TYPE_CONTROLDRAW; }
    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
    virtual void  Destroy(bool bRedraw);
    virtual void  SetPos(int x, int y, bool bRedraw);
    virtual void  Show(bool bRedraw);
    virtual void  Hide(bool bRedraw);
    virtual void  OnControlPaint(IRenderTarget* p);
    virtual IObject*  GetObject() { return m_pObject; }

    void  OnTimer();
protected:
    void  CreateTimer();
    void  DestroyTimer();

private:
    HWND   m_hWnd;
    HBITMAP  m_hBitmap;
    UI::IRenderBitmap*  m_pRenderBitmap;

    IObject*  m_pObject;
    POINT  m_ptLast;
    SIZE   m_sizeCaret;

    CRect  m_rcDraw;
    UINT_PTR  m_nTimerId;
    bool   m_bOn;  // ��ǰ�Ƿ������ShowCaret
    bool   m_bVisible; // ��¼������˸״̬
};

}