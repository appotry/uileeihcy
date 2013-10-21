#pragma once

namespace UI
{

enum CARET_TYPE
{
    CARET_TYPE_UNKNOWN = 0,
    CARET_TYPE_API = 1,
    CARET_TYPE_OWNERDRAW = 2,
    CARET_TYPE_WINDOW = 3,
    CARET_TYPE_CONTROLDRAW = 4,
};

interface ICaret
{
    virtual ~ICaret() = 0{};
    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight) = 0;
    virtual void  Destroy(bool bRedraw) = 0;
    virtual void  Show(bool bRedraw) = 0;
    virtual void  Hide(bool bRedraw) = 0;
    virtual void  SetPos(int x, int y, bool bRedraw) = 0;
    virtual void  OnControlPaint(IRenderTarget* p) = 0;

    virtual CARET_TYPE  GetType() = 0;
    virtual IObject*  GetObject() = 0;
};

}