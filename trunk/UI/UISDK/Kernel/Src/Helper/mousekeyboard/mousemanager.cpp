#include "stdafx.h"
#include "mousemanager.h"
#include "UISDK\Kernel\Src\Helper\tooltip\tooltipmanager.h"
#include "UISDK\Kernel\Src\Helper\MouseKeyboard\keyboardmanager.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Src\UIObject\HwndHost\HwndHost.h"
#include "mkmgr.h"

#pragma comment(lib, "Imm32.lib")

namespace UI
{



WindowMouseMgr::WindowMouseMgr()
{
    m_pUIApplication       = NULL;
    m_pWindow              = NULL;
	m_pObjPress            = NULL;
	m_pObjHover            = NULL;
    m_pObjRPress           = NULL;
    m_pObjDefault          = NULL;
    m_pObjOriginDefault    = NULL;
    m_pFocusObject         = NULL;
    m_pWindow              = NULL;
    m_pFocusObject         = NULL;
    m_pOldFocusObject      = NULL;
    m_pObjKeyboardCapture  = NULL;
    m_pObjMouseCapture     = NULL;
    m_bMouseTrack          = TRUE;      // 默认需要进行鼠标监视
    m_nKeyboardCaptureNotifyMsgId = 0;
    m_nMouseCaptureNotifyMsgId = 0;
    m_hIMC                 = ::ImmCreateContext();
    this->m_bMouseMoveReady = FALSE;
}
WindowMouseMgr::~WindowMouseMgr()
{
	m_pObjPress = NULL;
	m_pObjHover = NULL;
    m_bMouseTrack = TRUE;
    m_pWindow = NULL;

    ::ImmDestroyContext(m_hIMC);
    m_hIMC = NULL;
}

Object*  WindowMouseMgr::GetHoverObject()
{
	return m_pObjHover;
}
Object*  WindowMouseMgr::GetPressObject()
{
	return m_pObjPress;
}

void WindowMouseMgr::SetHoverObject(Object* pNewHoverObj)
{
    _SetHoverObject(pNewHoverObj, this);
}
void WindowMouseMgr::SetPressObject(Object* pNewPressObj)
{
	_SetPressObject(pNewPressObj, this);
}

// 这两个Direct仅仅是用于防止崩溃的产物。应该是架构不优的问题
// 在listctrl中的内部子控件销毁时，没法通知到listctrl的mousemgr，而是去通知window的mousemgr
// 导致最后在listctrl::removeall中SetHoverObject(NULL)中崩溃
void  WindowMouseMgr::SetHoverObjectDirectNULL()
{
    m_pObjHover = NULL;
}
void  WindowMouseMgr::SetPressObjectDirectNULL()
{
    m_pObjPress = NULL;
}
void WindowMouseMgr::ObjectHideInd(Object* pObj)
{
    if (NULL == pObj)
        return;

    if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
    {
        SetHoverObject(NULL);
    }
    if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
    {
        SetPressObject(NULL);
    }

    if (m_pObjDefault == pObj)
    {
        SetDefaultObject(NULL, true);
    }
    if (m_pObjOriginDefault == pObj)
    {
        m_pObjOriginDefault = NULL;
    }

    // 切换新的焦点
    if (m_pFocusObject && 
        (pObj == m_pFocusObject || pObj->IsMyChild(m_pFocusObject, true)))
    {
        m_pFocusObject->SetFocus(false);
        ::UISendMessage(m_pFocusObject, WM_KILLFOCUS, (WPARAM)pObj, NULL );
        m_pFocusObject = NULL;

        this->Tab_2_NextControl();
    }
}

void WindowMouseMgr::ObjectDeleteInd(Object* pObj)
{
	if (NULL == pObj)
		return;

	if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
	{
		m_pObjHover = NULL;
	}
	if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
	{
		m_pObjPress = NULL;
	}

    if (m_pObjDefault == pObj)
    {
        m_pObjDefault = NULL;
    }
    if (m_pObjOriginDefault == pObj)
    {
        m_pObjOriginDefault = NULL;
    }

    if (pObj == m_pFocusObject)
    {
        m_pFocusObject = NULL;
    }
    if (pObj == m_pOldFocusObject)
    {
        m_pOldFocusObject = NULL;
    }

    if (pObj->GetIMessage() == m_pObjKeyboardCapture)
    {
        m_pObjKeyboardCapture = NULL;
        m_nKeyboardCaptureNotifyMsgId = 0;
    }
    if (pObj->GetIMessage() == m_pObjMouseCapture)
    {
        m_pObjMouseCapture = NULL;
        m_nMouseCaptureNotifyMsgId = 0;
    }
}


//
//	[递归+遍历] [public] [static] 
//
//	获取当前鼠标下的最小单位控件. （ 不可见，不可用，不接受鼠标消息的控件、Panel 这些类型的对象不返回）
//
//	Parameter
//
//		pObjParent
//			[in]
//				递归遍历时，使用的parent object.调用该函数时赋值为窗口对象(Window/)pObj即可。
//		pt
//			[in]
//				在父对象中的鼠标位置，初始值为在窗口中的位置
// 
//      bUIEditorInvoke
//          [in]
//              在UIBuilder中只需要一个完整的CONTROL例如不查找SliderCtrl中的button，
//              另外UIBuilder还需要查找不接受鼠标消息类型的控件，如Label, Picture
//
//	Return
//		该位置下的对象，没有则返回空
//
Object* WindowMouseMgr::GetObjectByPos(Object* pObjParent, POINT* pt)
{
	Object* pHitObj = NULL;
	POINT   ptHit = *pt;

	Object* pChild = NULL;
	while (pChild = pObjParent->EnumNcChildObject(pChild))
	{
		if (!pChild->IsVisible())
			continue;
		if (!pChild->IsEnable())
			continue;
		if (pChild->IsRejectMouseMsgAll())
			continue;

		UINT nHitTest = UISendMessage(pChild, UI_WM_HITTEST, ptHit.x, ptHit.y);
		if (HTNOWHERE != nHitTest)
		{
			pHitObj = pChild;
			break;
		}
	}

	if (NULL == pHitObj)  // 继续搜索客户区的子对象
	{
		ptHit.x -= pObjParent->GetNonClientL();
		ptHit.y -= pObjParent->GetNonClientT();

		POINT ptOffset = {0,0};
		if (pObjParent->GetScrollOffset((int*)&ptOffset.x, (int*)&ptOffset.y))
		{
			ptHit.x += ptOffset.x;
			ptHit.y += ptOffset.y;
		}

		pChild = NULL;
		while (pChild = pObjParent->REnumChildObject(pChild))  // 反过来搜索。因为越后面的才显示在上面
		{
			if (!pChild->IsVisible())
				continue;
			if (!pChild->IsEnable())
				continue;
			if (pChild->IsRejectMouseMsgAll())
				continue;

			UINT nHitTest = UISendMessage(pChild, UI_WM_HITTEST, ptHit.x, ptHit.y);
			if (HTNOWHERE != nHitTest)
			{
				pHitObj = pChild;
				break;
			}
		}
	}

	if (NULL == pHitObj)
		return NULL;

	// 将坐标转换为相对于pHitObj的坐标
	ptHit.x -= pHitObj->GetParentRectL();
	ptHit.y -= pHitObj->GetParentRectT();

	Object* pRetObj = GetObjectByPos(pHitObj, &ptHit);

	// Panel不接收鼠标消息（但Panel中的子对象需要接收）
	if (pRetObj && (pRetObj->IsRejectMouseMsgSelf() || pRetObj->IsRejectMouseMsgAll()))
		pRetObj = NULL;

	// 若子控件中没有处于该位置的，直接返回本对象
	if (pRetObj)
		return pRetObj;

    if (pHitObj && (pHitObj->IsRejectMouseMsgSelf() || pHitObj->IsRejectMouseMsgAll()))
        pHitObj = NULL;

	return pHitObj;
}

//  将这个不放在GetObjectByPos中，避免影响GetObjectByPos的效率
Object* WindowMouseMgr::GetObjectByPos_UIEditor(Object* pObjParent, POINT* pt)
{
    Object* pHitObj = NULL;
    POINT   ptHit = *pt;

    Object* pChild = NULL;
    while (pChild = pObjParent->EnumNcChildObject(pChild))
    {
        if (!pChild->IsVisible())
            continue;

        UINT nHitTest = UISendMessage(pChild, UI_WM_HITTEST, ptHit.x, ptHit.y);
        if (HTNOWHERE != nHitTest)
        {
            pHitObj = pChild;
            break;
        }
    }

    if (NULL == pHitObj)  // 继续搜索客户区的子对象
    {
        ptHit.x -= pObjParent->GetNonClientL();
        ptHit.y -= pObjParent->GetNonClientT();

        POINT ptOffset = {0,0};
        if (pObjParent->GetScrollOffset((int*)&ptOffset.x, (int*)&ptOffset.y))
        {
            ptHit.x += ptOffset.x;
            ptHit.y += ptOffset.y;
        }

        pChild = NULL;
        while (pChild = pObjParent->REnumChildObject(pChild))  // 反过来搜索。因为越后面的才显示在上面
        {
            if (!pChild->IsVisible())
                continue;

            UINT nHitTest = UISendMessage(pChild, UI_WM_HITTEST, ptHit.x, ptHit.y);
            if (HTNOWHERE != nHitTest)
            {
                pHitObj = pChild;
                break;
            }
        }
    }

    if (NULL == pHitObj)
        return NULL;

    // 将坐标转换为相对于pHitObj的坐标
    ptHit.x -= pHitObj->GetParentRectL();
    ptHit.y -= pHitObj->GetParentRectT();

    Object* pRetObj = NULL;
    if (pHitObj->GetObjectType() != OBJ_CONTROL)
    {
        pRetObj = GetObjectByPos_UIEditor(pHitObj, &ptHit);
    }

    if (pRetObj)
        return pRetObj;
    return pHitObj;
}


void  WindowMouseMgr::SetUIApplication(IUIApplication* p, WindowBase* pWindow)
{
    this->m_pWindow = pWindow;
    m_pUIApplication = p;
}


//////////////////////////////////////////////////////////////////////////

IUIApplication*  WindowMouseMgr::GetUIApplication()
{
	if (NULL == m_pWindow)
		return NULL;

	return m_pWindow->GetUIApplication();
}

WindowBase* WindowMouseMgr::GetWindowObject() 
{ 
    return m_pWindow; 
}


LRESULT  WindowMouseMgr::HandleMessage(UINT msg, WPARAM w, LPARAM l, BOOL* pbHandled)
{
    switch (msg)
    {
    case WM_KILLFOCUS:
        this->OnKillFocus((HWND)w);
        break;

    case WM_SETFOCUS:
        this->OnSetFocus();
        break;

    case WM_NCDESTROY:
        this->OnNcDestroy();
        break;

    case WM_SETCURSOR:
        return this->OnSetCursor(w, l);
        break;
    }
    return 0L;
}
//
//The low-order word specifies the x-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area. 
//The high-order word specifies the y-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area. 
//
LRESULT WindowMouseMgr::HandleMouseMessage(UINT msg, WPARAM w, LPARAM l, BOOL* pbHandled)
{
    if (this->m_pObjMouseCapture)
    {
        BOOL bHandled = FALSE;
        LRESULT lRet = UISendMessage(m_pObjMouseCapture, msg, w, l, 0,0, m_nMouseCaptureNotifyMsgId, &bHandled);
        if (bHandled)
        {
            if (pbHandled)
                *pbHandled = TRUE;

            return lRet;
        }
    }

	int vkFlag = (int)w;
	int xPos = GET_X_LPARAM ( l );
	int yPos = GET_Y_LPARAM ( l );

	switch( msg )
	{
	case WM_MOUSEMOVE:
		{
			if (m_bMouseTrack)    //若允许追踪，则。
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof( TRACKMOUSEEVENT );
				tme.dwFlags = TME_LEAVE;
				tme.dwHoverTime = HOVER_DEFAULT;
				tme.hwndTrack = this->m_pWindow->GetHWND();
				::TrackMouseEvent( &tme );
				m_bMouseTrack = FALSE;
			}

			Object* pOldHover = m_pObjHover;
			Object* pOldPress = m_pObjPress;
			LRESULT lRet = this->OnMouseMove( vkFlag, xPos, yPos );

			if (pOldPress==m_pObjPress && pOldHover!=m_pObjHover  // 鼠标下的对象发生变化，需要重置鼠标样式
				&& GetCapture() != m_pWindow->GetHWND())             // 鼠标拖拽时不变，例如从EDIT中拖拽出来
			{
				::PostMessage(m_pWindow->GetHWND(), WM_SETCURSOR, NULL, MAKELPARAM(HTCLIENT,1));  // hiword 0表示弹出菜单
			}
			return lRet;
		}
		break;

	case WM_MOUSELEAVE:
		return this->OnMouseLeave( vkFlag, xPos, yPos );
		break;

	case WM_CANCELMODE:
		return this->OnCancelMode(w,l);

	case WM_LBUTTONDOWN:
		return this->OnLButtonDown(w, l, pbHandled);
		break;

	case WM_LBUTTONUP:
		return this->OnLButtonUp(w, l);
		break;

	case WM_RBUTTONDOWN:
		return this->OnRButtonDown(w,l);
		break;

	case WM_RBUTTONUP:
		return this->OnRButtonUp(w,l);
		break;

	case WM_LBUTTONDBLCLK:
		//this->OnLButtonDown( vkFlag, xPos, yPos );
		return this->OnLButtonDBClick(w,l, pbHandled);
		break;

    case WM_MOUSEWHEEL:
        return this->OnMouseWheel(w,l);  // 该消息可能需要上抛给主窗口处理
	}
	return 0L;
}

LRESULT  WindowMouseMgr::HandleKeyboardMessage(UINT msg, WPARAM w, LPARAM l, BOOL* pbHandled)
{
    if (this->m_pObjKeyboardCapture)
    {
        BOOL bHandled = FALSE;
        LRESULT lRet = UISendMessage(m_pObjKeyboardCapture, msg, w, l, 0,0, m_nKeyboardCaptureNotifyMsgId, &bHandled);
        if (bHandled)
        {
            if (pbHandled)
                *pbHandled = TRUE;

            return lRet;
        }
    }

    switch( msg )
    {
    case WM_CHAR:
        return this->OnChar(w, l);
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        return this->OnKeyDown(msg, w,l );
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        return this->OnKeyUp(w, l);
        break;

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_ENDCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_SETCONTEXT:
    case WM_IME_NOTIFY:
    case WM_IME_CONTROL:
    case WM_IME_COMPOSITIONFULL:
    case WM_IME_SELECT:
    case WM_IME_CHAR:
    case WM_IME_REQUEST:
    case WM_IME_KEYDOWN:
    case WM_IME_KEYUP:
        {
            return this->OnImeMsg(msg, w, l, pbHandled);
        }
    }
    return 0L;
}

//
//	Parameter
//		vkFlag
//			[in]
//
//		xPos, yPos
//			[in]	当前鼠标位置，以窗口左上角为(0,0)起点
//
LRESULT WindowMouseMgr::OnMouseMove( int vkFlag, int xPos, int yPos )
{	
    this->m_bMouseMoveReady = TRUE;

	// 1. 判断当前鼠标位置
	POINT pt = { xPos, yPos };

	Object* pObj = this->GetObjectByPos(m_pWindow, &pt);

	_OnMouseMove(pObj, vkFlag, MAKELPARAM(pt.x, pt.y), this);
	return 0L;
}


LRESULT WindowMouseMgr::OnMouseLeave( int vkFlag, int xPos, int yPos )
{
	_OnMouseLeave(this);

	this->m_bMouseTrack = TRUE;// 继续开启TRACKMOUSEEVENT
    this->m_bMouseMoveReady = FALSE;

	if (GetCapture() == m_pWindow->GetHWND())  // 有可能是其它对象直接发送过来WM_MOUSELEAVE，例如COMBOBOX.button.onlbuttondown
	{
		::ReleaseCapture();
	}
	return TRUE;
}

LRESULT WindowMouseMgr::OnCancelMode(WPARAM w, LPARAM l)
{
	if (m_pObjPress != NULL)
	{
        UIMSG msg;
		msg.pMsgTo = m_pObjPress->GetIMessage();
		msg.message = WM_CANCELMODE;
		msg.wParam  = w;
		msg.lParam  = l;
		::UISendMessage(&msg);
	}

	return 0;
}


LRESULT WindowMouseMgr::OnLButtonDown( WPARAM w, LPARAM l, BOOL* pbHandled)
{
    if (FALSE == this->m_bMouseMoveReady)
    {
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, NULL);  // 注：要取消wParam中的鼠标按下标记
    }

	if (m_pObjHover)
		::SetCapture(this->m_pWindow->GetHWND());

	_OnLButtonDown(w, l, pbHandled, this);
	return 0L;
}

LRESULT WindowMouseMgr::OnLButtonUp( WPARAM w, LPARAM l)
{
    Object*  pSaveObjPress = m_pObjPress;
    Object*  pSaveObjHover = m_pObjHover;

	if (m_pObjPress)
	{
        _OnLButtonUp(w, l, this);

        // 如果这个时候鼠标离开了控件，发送MOUSELEAVE消息. 这个时候鼠标位置可能已经重新变化了，需要重新获取
        POINT ptCursorNow;
        GetCursorPos(&ptCursorNow);
        ::MapWindowPoints(NULL, m_pWindow->GetHWND(), &ptCursorNow, 1);

        Object* pNowHover = this->GetObjectByPos(m_pWindow, &ptCursorNow);
        this->SetHoverObject(pNowHover);
        if (pNowHover != pSaveObjPress && NULL != pSaveObjPress)
            ::UISendMessage(pSaveObjPress, WM_MOUSELEAVE);
        if (pNowHover != pSaveObjHover && NULL != pNowHover)
            ::UISendMessage(pNowHover, WM_MOUSEMOVE, w, l );

		if (GetCapture() == m_pWindow->GetHWND()) 
			::ReleaseCapture();
	}
	 
	return 0L;
}

LRESULT WindowMouseMgr::OnLButtonDBClick( WPARAM w,LPARAM l, BOOL* pbHandled )
{
    if (FALSE == this->m_bMouseMoveReady)
    {
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, NULL);  // 注：要取消wParam中的鼠标按下标记
    }

	if (m_pObjPress || NULL != m_pObjHover)
	{
		::SetCapture(this->m_pWindow->GetHWND());
	}
	
    _OnLButtonDBClick(w, l, pbHandled, this);
	return 0;
}

LRESULT WindowMouseMgr::OnRButtonDown( WPARAM w,LPARAM l )
{
    if (FALSE == this->m_bMouseMoveReady)
    {
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, NULL);
    }

//     Object* p = m_pObjPress;
//     if (NULL == p)
//         p = m_pObjHover;
    Object* p = m_pObjHover;
    if (NULL == p)
        return 0;

    m_pObjRPress = p;
    if (p && p->CanTabstop())
    {
        SetFocusObject(p);
    }

	if (p)
	{
		m_pWindow->GetUIApplication()->HideToolTip();
		return ::UISendMessage(p, WM_RBUTTONDOWN, w, l);
	}
	
	return 0;
}
LRESULT WindowMouseMgr::OnRButtonUp( WPARAM w,LPARAM l )
{
	if (m_pObjRPress)
	{
		LRESULT lRet = ::UISendMessage(m_pObjRPress, WM_RBUTTONUP, w, l);
        m_pObjRPress = NULL;
        return lRet;
	}
// 	else if (m_pObjHover)  // 仅hover时不发送up消息，因为有可能是鼠标在别的地方点击，然后移到hover上释放
// 	{
// 		return ::UISendMessage(m_pObjHover, WM_RBUTTONUP, w, l);
// 	}
	return 0;
}

// 该函数同时还支撑着WM_CANCELMODE的作用
void WindowMouseMgr::OnKillFocus(HWND hFocusWnd)
{
	if (this->m_pObjHover != NULL)
	{
		::UISendMessage(m_pObjHover, WM_MOUSELEAVE);
	}
	if (this->m_pObjPress != NULL)
	{
		if (GetCapture() == m_pWindow->GetHWND()) 
		{
			::ReleaseCapture();
		}

		if (this->m_pObjPress != this->m_pObjHover)  // 避免将WM_MOUSELEAVE发送两次
			::UISendMessage(m_pObjPress, WM_MOUSELEAVE);
	}

	this->SetHoverObject(NULL);
	this->SetPressObject(NULL);
    this->m_bMouseMoveReady = FALSE;  // 失焦点后直接触发鼠标按下会导致状态不正确

    m_pObjKeyboardCapture = NULL;
    m_nKeyboardCaptureNotifyMsgId = 0;
    m_pObjMouseCapture = NULL;
    m_nMouseCaptureNotifyMsgId = 0;

    // TODO: KEYBOARD
    Object* p = this->GetOldFocusObject();
    if (NULL == p)
        p = m_pFocusObject;

    if (p)  // 给Focus对象发送焦点消息，但不重置m_pFocusObject。例如鼠标点到桌面上导致的失焦
    {
        int nOldStateBits = p->GetStateBit();
        p->SetFocus(false);
        ::UISendMessage(p, WM_KILLFOCUS, (WPARAM)NULL, (LPARAM)0);
    }

    // 当鼠标是点在了本窗口上面的一个HOSTWND里的子窗口时，重置m_pFocusObject，否则鼠标再点回m_pFocusObject时将没反应
    if (m_pWindow)
    {
        HWND hWnd = m_pWindow->GetHWND();
        if (IsChild(hWnd, hFocusWnd))
            this->SetFocusObjectDirect(NULL);
    }
}

void WindowMouseMgr::OnSetFocus()
{
    if (m_pFocusObject && !m_pFocusObject->IsFocus())
    {
        int nOldStateBits = m_pFocusObject->GetStateBit();
        m_pFocusObject->SetFocus(true);
        ::UISendMessage(m_pFocusObject, WM_SETFOCUS, (WPARAM)GetOldFocusObject(), (LPARAM)0);  // 这里的GetOldFocusObject对应于
    }
    else
    {
        this->Tab_2_NextControl();
    }
}

BOOL WindowMouseMgr::OnSetCursor( WPARAM w,LPARAM l )
{
    Object* pObj = m_pObjPress;
    if (NULL == pObj)
        pObj = m_pObjHover;

    BOOL bHandled = FALSE;
	if (pObj)
	{
		::UISendMessage(pObj->GetIMessage(), WM_SETCURSOR, w, l, 0, 0, 0, &bHandled);
	}
	return bHandled;
}

void WindowMouseMgr::OnNcDestroy()
{
	m_pObjHover = m_pObjPress = NULL;  // 不调用SetHoverObject/SetPressObject
    m_pFocusObject = NULL;	// 不调用SetFocusObject，因为不涉及刷新了
    m_pOldFocusObject = NULL;
    m_pObjKeyboardCapture = NULL;
}

/*
Code                   Meaning
------------------------------------------------------------------------

DLGC_BUTTON            Control is a button (of any kind).
DLGC_DEFPUSHBUTTON     Control is a default push button.
DLGC_HASSETSEL         Windows will send an EM_SETSEL message to the control to select its contents.
DLGC_RADIOBUTTON       Control is an option (radio) button.
DLGC_STATIC            Control is a static control.
DLGC_UNDEFPUSHBUTTON   Control is a push button but not the default push button.(是一个按钮，但不是default)
DLGC_WANTALLKEYS       Control processes all keyboard input.
DLGC_WANTARROWS        Control processes arrow keys.
DLGC_WANTCHARS         Control processes WM_CHAR messages.
DLGC_WANTMESSAGE       Control processes the message in the MSG structure that lParam points to.
DLGC_WANTTAB           Control processes the TAB key.
*/

void WindowMouseMgr::CheckDefPushButton(Object* pNewObj)
{
    if (NULL == pNewObj)
        return;

    UINT codeOldFocus = 0;
    UINT codeNewFocus = UISendMessage(pNewObj->GetIObject(), WM_GETDLGCODE);

    if (pNewObj == m_pFocusObject)
    {
        if (codeNewFocus & DLGC_UNDEFPUSHBUTTON)
        {
            SetDefaultObject(pNewObj, true);
        }
        return;
    }

    if (codeNewFocus & DLGC_DEFPUSHBUTTON)
        return;

    Object* pLastDefaultObj = m_pObjDefault; // 用于计算最终defbtn

    /*
     * If the focus is changing to or from a pushbutton, then remove the
     * default style from the current default button
     */
    if ((m_pFocusObject != NULL && (UISendMessage(m_pFocusObject, WM_GETDLGCODE) & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))) ||
        (pNewObj != NULL && (codeNewFocus & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))))
    {
        pLastDefaultObj = NULL;
    }

    /*
     * If moving to a button, make that button the default.
     */
    if (codeNewFocus & DLGC_UNDEFPUSHBUTTON)
    {
        pLastDefaultObj = pNewObj;
    }
    else
    {
        /*
         * Otherwise, make sure the original default button is default
         * and no others.
         */
        pLastDefaultObj = m_pObjOriginDefault;
    }

    SetDefaultObject(pLastDefaultObj, true);
}

BOOL WindowMouseMgr::IsDialogMessage(MSG* pMsg)
{
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        CheckDefPushButton(m_pObjHover);
    }

    if (pMsg->message == WM_KEYDOWN)
    {
        if (VK_TAB == pMsg->wParam)
        {
            LRESULT lRet = SendMessage(m_pWindow->m_hWnd, WM_QUERYUISTATE, 0, 0);
            if (lRet & UISF_HIDEFOCUS)
            {
                lRet &= ~UISF_HIDEFOCUS;
                SendMessage(m_pWindow->m_hWnd, WM_UPDATEUISTATE, lRet|UIS_SET, 0);
            }
            Object* pFocusObj = GetFocusObject();
            if (pFocusObj)
            {
                LRESULT lr = UISendMessage(pFocusObj->GetIMessage(), WM_GETDLGCODE, pMsg->wParam, (LPARAM)pMsg);
                if (lr & DLGC_WANTTAB)
                    return FALSE;
            }

            // 导航
            if (Util::IsKeyDown(VK_SHIFT))
            {
                Tab_2_PrevControl();
            }
            else
            {
                Tab_2_NextControl();
            }
            return TRUE;
        }
        else if (VK_LEFT == pMsg->wParam)
        {

        }
        else if (VK_RIGHT == pMsg->wParam)
        {

        }
        else if (VK_UP == pMsg->wParam)
        {

        }
        else if (VK_DOWN == pMsg->wParam)
        {

        }
        else if (VK_RETURN == pMsg->wParam || VK_EXECUTE == pMsg->wParam)
        {
            if (m_pFocusObject)
            {
                // 例如在多行编辑框中回车
                UINT codeFocus = UISendMessage(m_pFocusObject, WM_GETDLGCODE, 0, (LPARAM)pMsg);
                if (codeFocus & (DLGC_WANTMESSAGE|DLGC_WANTALLKEYS))
                    return FALSE;
            }
            
            // 查找default button，发送给defpushbutton
            if (m_pObjDefault && m_pObjDefault->IsEnable())
            {
                UIMSG   msg;
                msg.message = UI_WM_NOTIFY;
                msg.nCode   = UI_BN_CLICKED;
                msg.pMsgFrom = m_pObjDefault->GetIMessage();
                msg.pMsgTo = m_pObjDefault->GetIMessage();
                UISendMessage(&msg);
                return TRUE;
            }
        }
        else if (VK_ESCAPE == pMsg->wParam || VK_CANCEL == pMsg->wParam)
        {
            // 查找取消按钮，发送消息
        }
    }
    else if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_MENU)
    {
        LRESULT lRet = SendMessage(m_pWindow->m_hWnd, WM_QUERYUISTATE, 0, 0);
        if (lRet & UISF_HIDEFOCUS)
        {
            lRet &= ~UISF_HIDEFOCUS;
            SendMessage(m_pWindow->m_hWnd, WM_UPDATEUISTATE, lRet|UIS_SET, 0);

            if (m_pFocusObject)
            {
                m_pFocusObject->UpdateObject();
            }
        }
    }
    
    return FALSE;
}


void WindowMouseMgr::SetKeyboardCapture(IMessage* pObj, int nNotifyMsgId)
{
    m_pObjKeyboardCapture = pObj;
    m_nKeyboardCaptureNotifyMsgId = nNotifyMsgId;
}
void WindowMouseMgr::ReleaseKeyboardCapture(IMessage* pObj)
{
    if (pObj == m_pObjKeyboardCapture)
    {
        m_pObjKeyboardCapture = NULL;
        m_nKeyboardCaptureNotifyMsgId = 0;
    }
}
void  WindowMouseMgr::SetMouseCapture(IMessage* pObj, int nNotifyMsgId)
{
    m_pObjMouseCapture = pObj;
    m_nMouseCaptureNotifyMsgId = nNotifyMsgId;
}
void  WindowMouseMgr::ReleaseMouseCapture(IMessage* pObj)
{
    if (pObj == m_pObjMouseCapture)
    {
        m_pObjMouseCapture = NULL;
        m_nMouseCaptureNotifyMsgId = 0;
    }
}

void WindowMouseMgr::Tab_2_NextControl()
{
    Object* p = m_pFocusObject;
    if (!p)
        p = this->m_pWindow;

    if (!p)
        return;

    p = p->GetNextTabObject();

    if (p)
    {
        CheckDefPushButton(p);
        SetFocusObject(p);
    }
}
void WindowMouseMgr::Tab_2_PrevControl()
{
    Object* p = m_pFocusObject;
    if (NULL == p)
    {
        p = this->m_pWindow;
    }
    p = p->GetPrevTabObject();

    if (p)
    {
        CheckDefPushButton(p);
        SetFocusObject(p);
    }
}


Object*  WindowMouseMgr::GetOldFocusObject(bool bRemove)
{
    if (bRemove)
    {
        Object* p = m_pOldFocusObject;
        m_pOldFocusObject = NULL;
        return p;
    }
    else
    {
        return m_pOldFocusObject;
    }
}

void WindowMouseMgr::SetFocusObjectDirect(Object* pObj )
{
    m_pFocusObject = pObj;
}

Object*  WindowMouseMgr::GetFocusObject()
{
    return m_pFocusObject; 
}

void  WindowMouseMgr::SetDefaultObject(Object* pObj, bool bUpdate)
{
    if (pObj == m_pObjDefault)
        return;

    if (m_pObjDefault)
        m_pObjDefault->SetDefault(false, bUpdate);

    m_pObjDefault = pObj;

    if (m_pObjDefault)
        m_pObjDefault->SetDefault(true, bUpdate);
}


Object*  WindowMouseMgr::GetOriginDefaultObject()
{
    return m_pObjOriginDefault;
}
void  WindowMouseMgr::SetOriginDefaultObject(Object* pObj)
{
    m_pObjOriginDefault = pObj;
}
Object*  WindowMouseMgr::GetDefaultObject()
{
    return m_pObjDefault;
}


//
//	该函数只会在两个地方触发:
//		a. 鼠标点击选中一个UI控件
//		b. 使用TAB切换控件
//
//	因此当鼠标点击选中一个系统控件时，需要在HWNDHOST中调用一次SetFocusObjectDirect
//
void WindowMouseMgr::SetFocusObject(Object* pObj)
{
    // TODO: 这个条件会导致焦点从系统的combobox.edit点击到ui combobox.edit时没反应
    //       但屏蔽这个条件会导致UI COMBOBOX中的EDIT无法拖拽选中（因为一直在KILLFOCUS/SETFOCUS）
    if (m_pFocusObject == pObj)  
        return;

    if (pObj)
    {
        if (OBJ_HWNDHOST == pObj->GetObjectType())
        {
            HWND hWndFocus = ::GetFocus();
            HWND hWndHost = ((HwndHost*)pObj)->m_hWnd;
            if (hWndFocus != hWndHost && FALSE == ::IsChild(hWndHost, hWndFocus))  // 有可能点击了combobox中的edit,edit不是一个HOSTWND，但是HOSTWND的子窗口
            {
                m_pOldFocusObject = m_pFocusObject;   // 在HwndHost::WndProc WM_SETFOCUS中使用
                m_pFocusObject = pObj;

                if (GetWindowLong(hWndHost, GWL_EXSTYLE)&WS_EX_NOACTIVATE)
                {}
                else
                    ::SetFocus(hWndHost);  
                return;
            }
        }
        else
        {
            if (m_pWindow && ::GetFocus() != m_pWindow->GetHWND())
            {
                m_pOldFocusObject = m_pFocusObject;   // 在KeyboardManager::SetFocus中使用
                m_pFocusObject = pObj;

                HWND hWnd = m_pWindow->GetHWND();
                if (!IsWindowVisible(hWnd) || GetWindowLong(hWnd, GWL_EXSTYLE)&WS_EX_NOACTIVATE)
                {

                }
                else
                {
                    ::SetFocus(hWnd);  
                }

                return;
            }
        }
    }

    if (m_pFocusObject)
    {
        int nOldStateBits = m_pFocusObject->GetStateBit();
        m_pFocusObject->SetFocus(false);
        ::UISendMessage(m_pFocusObject, WM_KILLFOCUS, (WPARAM)pObj, NULL );
    }
    if (pObj)
    {
        int nOldStateBits = pObj->GetStateBit();
        pObj->SetFocus(true);
        ::UISendMessage(pObj, WM_SETFOCUS, (WPARAM)m_pFocusObject, NULL );
    }
    m_pFocusObject = pObj;

    // 根据焦点对象切换输入法状态
    if (m_pFocusObject && m_pFocusObject->TestStyle(OBJECT_STYLE_ENABLE_IME))
    {
        ImmAssociateContext(m_pWindow->m_hWnd, m_hIMC);
    }
    else
    {
        ImmAssociateContext(m_pWindow->m_hWnd, NULL);
    }
}


BOOL WindowMouseMgr::OnChar(WPARAM w, LPARAM l)
{	
    if (this->m_pFocusObject)
    {
        UISendMessage(m_pFocusObject, WM_CHAR, w, l);
        return TRUE;
    }
    return FALSE;
}

BOOL WindowMouseMgr::OnKeyDown(UINT nMsg, WPARAM w,LPARAM l)
{
    if (this->m_pFocusObject)
    {
        UISendMessage(m_pFocusObject, WM_KEYDOWN, w, l );
        return TRUE;
    }

    return FALSE;
}
BOOL WindowMouseMgr::OnKeyUp(WPARAM w,LPARAM l)
{
    if (this->m_pFocusObject)
    {
        UISendMessage(m_pFocusObject, WM_KEYUP, w, l);
        return TRUE;
    }

    return FALSE;
}

long WindowMouseMgr::OnMouseWheel(WPARAM w, LPARAM l)
{
    // 如果焦点对象没处理，则抛给焦点对象的父对象。
    // 例如一个位于ScrollPanel中的Edit，在Edit
    // 中进行MouseWheel的话，则应该滚动ScrollPanel

    Object*  pObj = m_pFocusObject;
    while (pObj)
    {
        BOOL  bHandled = FALSE;
        UISendMessage(pObj->GetIMessage(), WM_MOUSEWHEEL, w, l, 0, 0, 0, &bHandled);
        if (bHandled)
            break;

        pObj = pObj->GetParentObject();
    }
    return 0;
}

LRESULT  WindowMouseMgr::OnImeMsg(UINT uMsg, WPARAM w, LPARAM l, BOOL* pbHandled)
{
    if (this->m_pFocusObject)
    {
        return UISendMessage(m_pFocusObject->GetIMessage(), uMsg, w, l, 0, NULL, 0, pbHandled);
    }
    return 0;
}



}