#pragma once

namespace UI
{
class KeyboardMgrBase;
class WindowKeyboardMgr;
class Object;
class WindowBase;

class WindowMouseMgr
{
public:
    WindowMouseMgr( );
    ~WindowMouseMgr(void);

    static Object*  GetObjectByPos(Object* pObjParent, POINT* pt);
    static Object*  GetObjectByPos_UIEditor(Object* pObjParent, POINT* pt);

public:
    IUIApplication*  GetUIApplication();
    WindowBase*  GetWindowObject();
    void  SetUIApplication(IUIApplication* p, WindowBase* pWindow);

    LRESULT  HandleMessage(UINT msg, WPARAM w, LPARAM l, BOOL* pbHandled);
    LRESULT  HandleMouseMessage(UINT msg, WPARAM w, LPARAM l, BOOL* pbHandled);
    LRESULT  HandleKeyboardMessage(UINT msg, WPARAM w, LPARAM l, BOOL* pbHandled);
    BOOL  IsDialogMessage(MSG* pMsg);

    void  CheckDefPushButton(Object* pNewObj);
    void  Tab_2_NextControl();
    void  Tab_2_PrevControl();
    void  SetKeyboardCapture(IMessage* pObj, int nNotifyMsgId);
    void  ReleaseKeyboardCapture(IMessage* pObj);
    void  SetMouseCapture(IMessage* pObj, int nNotifyMsgId);
    void  ReleaseMouseCapture(IMessage* pObj);

	Object*  GetHoverObject();
	Object*  GetPressObject();
    Object*  GetFocusObject();
    Object*  GetDefaultObject();
    Object*  GetOldFocusObject(bool bRemove = true);
    Object*  GetOriginDefaultObject();

    void  SetFocusObject(Object* pObj);
    void  SetDefaultObject(Object* pObj, bool bUpdate);
	void  SetHoverObject(Object* pNewHoverObj);
	void  SetPressObject(Object* pNewPressObj);
    void  SetHoverObjectDirectNULL();
    void  SetPressObjectDirectNULL();
    void  SetFocusObjectDirect(Object* pObj);
    void  SetOriginDefaultObject(Object* pObj);
   
    void  ObjectHideInd(Object* pObj);
    void  ObjectDeleteInd(Object* pObj);

protected:
    LRESULT  OnMouseMove (int vkFlag, int xPos, int yPos);
    LRESULT  OnMouseLeave(int vkFlag, int xPos, int yPos);
    LRESULT  OnCancelMode(WPARAM, LPARAM );
    LRESULT  OnLButtonDown(WPARAM, LPARAM, BOOL* pbHandled);
    LRESULT  OnLButtonUp (WPARAM, LPARAM);
    LRESULT  OnRButtonDown(WPARAM,LPARAM);
    LRESULT  OnRButtonUp  (WPARAM,LPARAM);
    LRESULT  OnLButtonDBClick(WPARAM,LPARAM, BOOL* pbHandled);
    LRESULT  OnImeMsg(UINT, WPARAM, LPARAM, BOOL* pbHandled);
    void  OnKillFocus(HWND hWndFocus);
    void  OnSetFocus();
    BOOL  OnSetCursor(WPARAM,LPARAM);
    void  OnNcDestroy();
    BOOL  OnChar(WPARAM,LPARAM);
    BOOL  OnKeyDown(UINT nMsg, WPARAM,LPARAM);
    BOOL  OnKeyUp(WPARAM,LPARAM);
    long  OnMouseWheel(WPARAM, LPARAM);

    
    template <class T>
    friend void _SetHoverObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _SetPressObject(Object* pNewHoverObj, T* pThis);
    template <class T>
    friend void _OnMouseMove(Object* pObj, WPARAM wParam, LPARAM lParam, T* pThis);
    template <class T>
    friend void _OnMouseLeave(T* pThis);
    template <class T>
    friend void _OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis);
    template <class T>
    friend void _OnLButtonUp(WPARAM w, LPARAM l, T* pThis);

protected:
    IUIApplication*  m_pUIApplication;
    WindowBase*  m_pWindow;         // 要处理的窗口对象

    Object*  m_pObjHover;           // 是否有对象处于鼠标下面
    Object*  m_pObjPress;           // 是否有对象已被按下
    Object*  m_pObjRPress;          // 右键点击对象，用于判断是否需要给谁发送RButtonUp消息（仅在RDown、RUp中处理，其它逻辑中不管，越简单越好） 
    
    Object*  m_pOldFocusObject;     // 临时对象，用于记录焦点发生变化过程中的旧的焦点对象
    Object*  m_pFocusObject;        // 当前有用焦点的对象

    Object*  m_pObjDefault;         // 当前default button
    Object*  m_pObjOriginDefault;   // 原始的default button，没有defbtn时，系统将把default属性设置到该对象上面。

    IMessage*  m_pObjKeyboardCapture; // 临时需要拦截键盘消息的对象
    IMessage*  m_pObjMouseCapture;    // 临时需要拦截鼠标消息的对象
    int  m_nKeyboardCaptureNotifyMsgId;
    int  m_nMouseCaptureNotifyMsgId;

    HIMC  m_hIMC;                   // 窗口的输入法，用于区分一个控件是否需要启用输入法。如果一个控件不启用输入法，即使切换到中文输入法，也不会显示输入法的状态栏的。
    BOOL  m_bMouseTrack;            // 是否需要监测鼠标离开m_pWnd了
    BOOL  m_bMouseMoveReady;        // 用于解决经常会出现的场景：还没使用MOUSEMOVE获取hover obj，就收到了一个BUTTONDOWN的消息，
                                    // 导致消息响应失败。例如手动发送一个MOUSEMOVE或者按下ALT键导致的MouseLeave
};

}