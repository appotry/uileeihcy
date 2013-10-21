#ifndef IOBJECT_H_E0069B93_FD58_46f0_9FCC_78B2F7D0A96D
#define IOBJECT_H_E0069B93_FD58_46f0_9FCC_78B2F7D0A96D

#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{
interface IWindowBase;
interface IPanel;
interface IRenderLayer;
interface IRenderChain;
class Object;

interface UISDKAPI IObject : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(IObject, Object)

    const TCHAR*  GetId();
    void  SetID(const TCHAR* szText);

    IWindowBase*  GetWindowObject();
    IRenderLayer*  GetRenderLayer();
    IRenderChain*  GetRenderChain();
    HWND  GetHWND();

    void  SetOutRef(void** ppOutRef);
    IUIApplication* GetUIApplication();
    void  SetUIApplication(IUIApplication* p);
    
    void     InitDefaultAttrib();
    void     SetObjectPos(int x, int y, int cx, int cy, int nFlag=0);
    void     SetObjectPos(CRect* prc, int nFlag=0);
    void     SetUserData(LPVOID p);
    LPVOID   GetUserData();
    void     ModifyStyle( UINT add, UINT remove, bool bNotify);
    bool     TestStyle(UINT nStyle);
    void     ModifyStyleEx( UINT add, UINT remove, bool bNotify);
    bool     TestStyleEx(UINT nStyleEx);
    void     SetCanRedraw(bool bReDraw);
    UINT     GetStyle();
    UINT     GetStyleEx();
    void     SetStyle(UINT n);
    void     SetStyleEx(UINT n);

    bool     testStateBit(UINT bit);
    void     setStateBit(UINT bit);
    void     clearStateBit(UINT bit);
    bool     CanRedraw();
    IRenderFont* GetRenderFont();

    int      GetStateBit();
    bool     IsFocus();
//    void     SetFocus(bool);  要设置一个对象的焦点，得使用windowbase->SetFocusObj
    bool     IsTabstop();
    bool     CanTabstop();
    bool     IsGroup();
    void     SetTabstop(bool);
    void     SetGroup(bool);
    bool     IsVisible();
    bool     IsCollapsed();
    bool     IsMySelfVisible();
    bool     IsEnable();
    void     SetVisible(bool b, bool bRedraw=true, bool bUpdateLayout=true);
    void     SetEnable(bool b, bool bUpdateNow=true);
    bool     IsDefault();
//    bool     IsReadonly();
    bool     IsHover();
    bool     IsSelected();
    bool     IsPress();
    bool     IsForcePress();
    bool     IsForceHover();
    void     SetDefault(bool, bool bNotify=true);
//    void     SetReadonly(bool);
    void     SetForceHover(bool, bool bNotify=true);
    void     SetForcePress(bool, bool bNotify=true);
    void     SetHover(bool, bool bNotify=true);
    void     SetPress(bool, bool bNotify=true);
    void     SetSelected(bool, bool bNotify=true);

    bool     IsTransparent();
    void     SetTransparent(bool b);
    bool     IsNcObject();
    void     SetAsNcObject(bool b);
    bool     IsRejectMouseMsgAll();
    bool     IsRejectMouseMsgSelf();

    IObject*  FindChildObject(const TCHAR* szObjId);
    IObject*  GetParentObject();
    IObject*  GetChildObject();
    IObject*  GetNcChildObject();
    IObject*  GetLastChildObject();
    IObject*  GetNextObject();
    IObject*  GetPrevObject();
    IObject*  GetRootObject();

    IObject*  EnumChildObject(IObject*);
    IObject*  REnumChildObject(IObject*);
    IObject*  EnumNcChildObject(IObject*);
    IObject*  EnumAllChildObject(IObject*);
    IObject*  EnumParentObject(IObject*);
    IObject*  REnumParentObject(IObject*);

    IObject*  GetNextTreeItemObject();
    IObject*  GetPrevTreeItemObject();
    IObject*  GetNextTreeTabstopItemObject();
    IObject*  GetPrevTreeTabstopItemObject();
    IObject*  GetNextTabObject();
    IObject*  GetPrevTabObject();

    void  SetParentObjectDirect(IObject* p);
    void  SetChildObjectDirect(IObject* p);
    void  SetNcChildObjectDirect(IObject* p);
    void  SetNextObjectDirect(IObject* p);
    void  SetPrevObjectDirect(IObject* p); 

    void  AddChild(IObject*);
    void  AddNcChild(IObject*);
    bool  IsMyChild(IObject* pChild, bool bFindInGrand);
    bool  RemoveChildInTree(IObject* pChild);
    void  RemoveMeInTheTree();
    void  UpdateObjectNonClientRegion();
    void  Standalone();
    void  MoveToAsFirstChild();
    void  MoveToAsLastChild();
    void  InsertAfter(IObject* pInsertAfter);
    void  InsertBefore(IObject* pInsertBefore);
    bool  SwapObject(IObject* pObj1, IObject* pObj2);

    void  UpdateObject(bool bUpdateNow=true);
    void  UpdateObjectBkgnd(bool bUpdateNow);
    void  UpdateLayout(bool bUpdate);    

    void  GetNonClientRegion(CRegion4* prc);
    void  SetNonClientRegionExcludePaddingBorder(CRegion4*);
    void  SetNonClientRegionExcludePaddingBorderL(int);
    void  SetNonClientRegionExcludePaddingBorderT(int);
    void  SetNonClientRegionExcludePaddingBorderR(int);
    void  SetNonClientRegionExcludePaddingBorderB(int);
    int   GetNonClientL();
    int   GetNonClientT();
    int   GetNonClientR();
    int   GetNonClientB();
    int   GetNonClientW();
    int   GetNonClientH();
    void  GetBorderRegion(CRegion4*);
    int   GetBorderL();
    int   GetBorderT();
    int   GetBorderR();
    int   GetBorderB();
    void  SetBorderRegion(CRegion4*);
    void  GetPaddingRegion(CRegion4*);
    void  SetPaddingRegion(CRegion4*);
    int   GetPaddingL();
    int   GetPaddingT();
    int   GetPaddingR();
    int   GetPaddingB();
    int   GetPaddingW();
    int   GetPaddingH();
    void  SetMarginRegion(CRegion4*);
    void  GetMarginRegion(CRegion4*);
    int   GetMarginL();
    int   GetMarginT();
    int   GetMarginR();
    int   GetMarginB();
    int   GetMarginW();
    int   GetMarginH();
    void  GetClientRect(CRect*);
    void  GetClientRectAsWin32(CRect*);
    void  GetClientRectInWindow(CRect*);
    void  GetParentRect(CRect*);
    int   GetParentRectL();
    int   GetParentRectT();
    int   GetParentRectR();
    int   GetParentRectB();

    POINT  GetRealPosInWindow();
    void  GetWindowRect(CRect*);
    void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj);
    void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
    void  WindowPoint2ObjectClientPoint_CalcScroll(const POINT* ptWindow, POINT* ptObj);
    void  ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
    void  ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj);
    bool  GetScrollOffset(int* pxOffset, int* pyOffset);
    bool  GetScrollRange(int* pxRange, int* pyRange);
    bool  GetObjectVisibleRect(RECT* prc, bool bInWindowOrLayer); 
    bool  GetObjectVisibleClientRect(RECT* prc, bool bInWindowOrLayer);

    int   GetWidth();
    int   GetHeight();
    int   GetWidthWithMargins();
    int   GetHeightWithMargins(); 

    int   GetConfigWidth();
    int   GetConfigHeight();
    int   GetConfigLayoutFlags();
    int   GetConfigLeft();
    int   GetConfigRight();
    int   GetConfigTop();
    int   GetConfigBottom();

    void  SetConfigWidth(int n);
    void  SetConfigHeight(int n);
    void  SetConfigLayoutFlags(int n);
    void  SetConfigLeft(int n);
    void  SetConfigRight(int n);
    void  SetConfigTop(int n);
    void  SetConfigBottom(int n);

    // 子类覆盖
    TCHAR*   GetObjectName();
    OBJ_TYPE  GetObjectType();
    CONTROL_TYPE  GetObjectExtentType();

    ITextRenderBase*  GetTextRender();
    IRenderBase*  GetBkRender();
    IRenderBase*  GetForeRender();
    void  SetBkgndRender(IRenderBase* p);
    void  SetForegndRender(IRenderBase* p);
    void  SetTextRender(ITextRenderBase* p);
    void  SetAttributeByPrefix(const TCHAR* szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase);
    void  ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload);
    const TCHAR*  GetAttribute(const TCHAR* szKey, bool bErase);
    void  AddAttribute(const TCHAR* szKey, const TCHAR*  szValue);
    void  GetMapAttribute(IMapAttribute** ppMapAttribute);
    SIZE  GetDesiredSize();
    HBITMAP  TakeSnapshot();
    HBITMAP  TakeBkgndSnapshot();
    
    bool  SetCursor(const TCHAR* szCursorID);
    bool  SetMouseCapture(int nNotifyMsgId);
    bool  ReleaseMouseCapture();
    bool  SetKeyboardCapture(int nNotifyMsgId);
    bool  ReleaseKeyboardCapture();
};

}

#endif  // IOBJECT_H_E0069B93_FD58_46f0_9FCC_78B2F7D0A96D