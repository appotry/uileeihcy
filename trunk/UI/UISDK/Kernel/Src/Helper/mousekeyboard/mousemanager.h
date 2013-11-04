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
    WindowBase*  m_pWindow;         // Ҫ����Ĵ��ڶ���

    Object*  m_pObjHover;           // �Ƿ��ж������������
    Object*  m_pObjPress;           // �Ƿ��ж����ѱ�����
    Object*  m_pObjRPress;          // �Ҽ�������������ж��Ƿ���Ҫ��˭����RButtonUp��Ϣ������RDown��RUp�д��������߼��в��ܣ�Խ��Խ�ã� 
    
    Object*  m_pOldFocusObject;     // ��ʱ�������ڼ�¼���㷢���仯�����еľɵĽ������
    Object*  m_pFocusObject;        // ��ǰ���ý���Ķ���

    Object*  m_pObjDefault;         // ��ǰdefault button
    Object*  m_pObjOriginDefault;   // ԭʼ��default button��û��defbtnʱ��ϵͳ����default�������õ��ö������档

    IMessage*  m_pObjKeyboardCapture; // ��ʱ��Ҫ���ؼ�����Ϣ�Ķ���
    IMessage*  m_pObjMouseCapture;    // ��ʱ��Ҫ���������Ϣ�Ķ���
    int  m_nKeyboardCaptureNotifyMsgId;
    int  m_nMouseCaptureNotifyMsgId;

    HIMC  m_hIMC;                   // ���ڵ����뷨����������һ���ؼ��Ƿ���Ҫ�������뷨�����һ���ؼ����������뷨����ʹ�л����������뷨��Ҳ������ʾ���뷨��״̬���ġ�
    BOOL  m_bMouseTrack;            // �Ƿ���Ҫ�������뿪m_pWnd��
    BOOL  m_bMouseMoveReady;        // ���ڽ����������ֵĳ�������ûʹ��MOUSEMOVE��ȡhover obj�����յ���һ��BUTTONDOWN����Ϣ��
                                    // ������Ϣ��Ӧʧ�ܡ������ֶ�����һ��MOUSEMOVE���߰���ALT�����µ�MouseLeave
};

}