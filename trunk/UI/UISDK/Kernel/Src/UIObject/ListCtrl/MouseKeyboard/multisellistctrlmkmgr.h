#pragma once
#include "listctrlmkmgr.h"

namespace UI
{


class ISelectRegionRender
{
public:
    ISelectRegionRender()
    {
        m_pObject = NULL;
        m_rcSelectRegion.SetRectEmpty();
    }
    virtual ~ ISelectRegionRender() = 0 {};

public:
    virtual bool Init(Object*  pObj, POINT pt)
    {
        m_pObject = pObj;
        return true;
    }
    virtual void MouseMove(POINT pt) = 0;
    virtual void LButtonUp() = 0;
    // virtual void PrePaint() = 0;
    // virtual void PostPaint() = 0;
    virtual void GetSelectRegion(RECT* lprc) 
    {
        if (lprc)
            ::CopyRect(lprc, &m_rcSelectRegion);
    }

    RECT    NormalizeRect(RECT* prc)
    {
        RECT rc;
        CopyRect(&rc, prc);
        if (rc.left > rc.right)
        {
            rc.left = rc.right;
            rc.right = prc->left;
        }
        if (rc.top > rc.bottom)
        {
            rc.top = rc.bottom;
            rc.bottom = prc->top;
        }
        return rc;
    }
protected:
    Object*  m_pObject;
    CRect    m_rcSelectRegion;
};

class MultiSelListCtrlMKMgr : public ListCtrlMKMgrBase
{
public:
    MultiSelListCtrlMKMgr();
    ~MultiSelListCtrlMKMgr();

protected:
    virtual BOOL  DoProcessMessage(UIMSG* pMsg);

protected:
    void  OnMouseMove(UIMSG* pMsg);
    void  OnLButtonDown(UIMSG* pMsg);
    void  OnLButtonUp(UIMSG* pMsg);
    void  OnMouseLeave(UIMSG* pMsg);

    void  OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg);
    void  OnKeyDown_down(UIMSG* pMsg);
    void  OnKeyDown_up(UIMSG* pMsg);

    void  OnMouseMove_DragSelectRegion(UIMSG* pMsg);

protected:
    ISelectRegionRender*  m_pSelectRegionRender;
};




// XP�µ����߿�ѡ��
// ����ʽ�������ڷֲ㴰�ڡ���Ϊ����ʽ��Ҫֱ���ύ���������棬�����ڲ���Ҫˢ�£���Ч������ߵġ�
//
// 2013.1.7 ע
//
//   ��������ʵ������˸������ѡ�񣬷�����DrawFocusRect�ǻᵼ����˸�ģ�����Rectangle/Polyline��˸�����ء�
//   ��������һЩ�������������ˡ�����������polylineֻ����һ�����Ӻ���Ҫ���������ߣ������ö������ߵ㣬
//   ����CLIP RGN��������Щ���ӣ�����WM_REDRAW�в�ͨ������...)
//   �Ժ��л����ٿ��ǰɡ�
//
class DotLine_SelectRegionRender : public ISelectRegionRender//, ICommitWindowBufferListener
{
public:
    DotLine_SelectRegionRender();
    ~DotLine_SelectRegionRender();

    virtual bool Init(Object* pObj, POINT pt);
    virtual void MouseMove(POINT point);
    virtual void LButtonUp();
//     virtual void PrePaint();
//     //	virtual void PostPaint();
// 
//     virtual void  PreCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount);
//     virtual void  PostCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount);
//     virtual bool  OnlyListenOnce() { return true; } 

    void  Draw(RECT* prc);

protected:
    HWND    m_hWnd;
    HDC     m_hWndDC;

    CRect   m_rcClip;
    HRGN    m_hClipRgn;

    CRect   m_rcView;
};
class AlphaRect_SelectRegionRender : public ISelectRegionRender
{
public:
};


}