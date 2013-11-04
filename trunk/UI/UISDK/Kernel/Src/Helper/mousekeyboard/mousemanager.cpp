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
    m_bMouseTrack          = TRUE;      // Ĭ����Ҫ����������
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

// ������Direct���������ڷ�ֹ�����Ĳ��Ӧ���Ǽܹ����ŵ�����
// ��listctrl�е��ڲ��ӿؼ�����ʱ��û��֪ͨ��listctrl��mousemgr������ȥ֪ͨwindow��mousemgr
// ���������listctrl::removeall��SetHoverObject(NULL)�б���
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

    // �л��µĽ���
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
//	[�ݹ�+����] [public] [static] 
//
//	��ȡ��ǰ����µ���С��λ�ؼ�. �� ���ɼ��������ã������������Ϣ�Ŀؼ���Panel ��Щ���͵Ķ��󲻷��أ�
//
//	Parameter
//
//		pObjParent
//			[in]
//				�ݹ����ʱ��ʹ�õ�parent object.���øú���ʱ��ֵΪ���ڶ���(Window/)pObj���ɡ�
//		pt
//			[in]
//				�ڸ������е����λ�ã���ʼֵΪ�ڴ����е�λ��
// 
//      bUIEditorInvoke
//          [in]
//              ��UIBuilder��ֻ��Ҫһ��������CONTROL���粻����SliderCtrl�е�button��
//              ����UIBuilder����Ҫ���Ҳ����������Ϣ���͵Ŀؼ�����Label, Picture
//
//	Return
//		��λ���µĶ���û���򷵻ؿ�
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

	if (NULL == pHitObj)  // ���������ͻ������Ӷ���
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
		while (pChild = pObjParent->REnumChildObject(pChild))  // ��������������ΪԽ����Ĳ���ʾ������
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

	// ������ת��Ϊ�����pHitObj������
	ptHit.x -= pHitObj->GetParentRectL();
	ptHit.y -= pHitObj->GetParentRectT();

	Object* pRetObj = GetObjectByPos(pHitObj, &ptHit);

	// Panel�����������Ϣ����Panel�е��Ӷ�����Ҫ���գ�
	if (pRetObj && (pRetObj->IsRejectMouseMsgSelf() || pRetObj->IsRejectMouseMsgAll()))
		pRetObj = NULL;

	// ���ӿؼ���û�д��ڸ�λ�õģ�ֱ�ӷ��ر�����
	if (pRetObj)
		return pRetObj;

    if (pHitObj && (pHitObj->IsRejectMouseMsgSelf() || pHitObj->IsRejectMouseMsgAll()))
        pHitObj = NULL;

	return pHitObj;
}

//  �����������GetObjectByPos�У�����Ӱ��GetObjectByPos��Ч��
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

    if (NULL == pHitObj)  // ���������ͻ������Ӷ���
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
        while (pChild = pObjParent->REnumChildObject(pChild))  // ��������������ΪԽ����Ĳ���ʾ������
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

    // ������ת��Ϊ�����pHitObj������
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
			if (m_bMouseTrack)    //������׷�٣���
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

			if (pOldPress==m_pObjPress && pOldHover!=m_pObjHover  // ����µĶ������仯����Ҫ���������ʽ
				&& GetCapture() != m_pWindow->GetHWND())             // �����קʱ���䣬�����EDIT����ק����
			{
				::PostMessage(m_pWindow->GetHWND(), WM_SETCURSOR, NULL, MAKELPARAM(HTCLIENT,1));  // hiword 0��ʾ�����˵�
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
        return this->OnMouseWheel(w,l);  // ����Ϣ������Ҫ���׸������ڴ���
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
//			[in]	��ǰ���λ�ã��Դ������Ͻ�Ϊ(0,0)���
//
LRESULT WindowMouseMgr::OnMouseMove( int vkFlag, int xPos, int yPos )
{	
    this->m_bMouseMoveReady = TRUE;

	// 1. �жϵ�ǰ���λ��
	POINT pt = { xPos, yPos };

	Object* pObj = this->GetObjectByPos(m_pWindow, &pt);

	_OnMouseMove(pObj, vkFlag, MAKELPARAM(pt.x, pt.y), this);
	return 0L;
}


LRESULT WindowMouseMgr::OnMouseLeave( int vkFlag, int xPos, int yPos )
{
	_OnMouseLeave(this);

	this->m_bMouseTrack = TRUE;// ��������TRACKMOUSEEVENT
    this->m_bMouseMoveReady = FALSE;

	if (GetCapture() == m_pWindow->GetHWND())  // �п�������������ֱ�ӷ��͹���WM_MOUSELEAVE������COMBOBOX.button.onlbuttondown
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
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, NULL);  // ע��Ҫȡ��wParam�е���갴�±��
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

        // ������ʱ������뿪�˿ؼ�������MOUSELEAVE��Ϣ. ���ʱ�����λ�ÿ����Ѿ����±仯�ˣ���Ҫ���»�ȡ
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
        HandleMouseMessage(WM_MOUSEMOVE, 0, l, NULL);  // ע��Ҫȡ��wParam�е���갴�±��
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
// 	else if (m_pObjHover)  // ��hoverʱ������up��Ϣ����Ϊ�п���������ڱ�ĵط������Ȼ���Ƶ�hover���ͷ�
// 	{
// 		return ::UISendMessage(m_pObjHover, WM_RBUTTONUP, w, l);
// 	}
	return 0;
}

// �ú���ͬʱ��֧����WM_CANCELMODE������
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

		if (this->m_pObjPress != this->m_pObjHover)  // ���⽫WM_MOUSELEAVE��������
			::UISendMessage(m_pObjPress, WM_MOUSELEAVE);
	}

	this->SetHoverObject(NULL);
	this->SetPressObject(NULL);
    this->m_bMouseMoveReady = FALSE;  // ʧ�����ֱ�Ӵ�����갴�»ᵼ��״̬����ȷ

    m_pObjKeyboardCapture = NULL;
    m_nKeyboardCaptureNotifyMsgId = 0;
    m_pObjMouseCapture = NULL;
    m_nMouseCaptureNotifyMsgId = 0;

    // TODO: KEYBOARD
    Object* p = this->GetOldFocusObject();
    if (NULL == p)
        p = m_pFocusObject;

    if (p)  // ��Focus�����ͽ�����Ϣ����������m_pFocusObject���������㵽�����ϵ��µ�ʧ��
    {
        int nOldStateBits = p->GetStateBit();
        p->SetFocus(false);
        ::UISendMessage(p, WM_KILLFOCUS, (WPARAM)NULL, (LPARAM)0);
    }

    // ������ǵ����˱����������һ��HOSTWND����Ӵ���ʱ������m_pFocusObject����������ٵ��m_pFocusObjectʱ��û��Ӧ
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
        ::UISendMessage(m_pFocusObject, WM_SETFOCUS, (WPARAM)GetOldFocusObject(), (LPARAM)0);  // �����GetOldFocusObject��Ӧ��
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
	m_pObjHover = m_pObjPress = NULL;  // ������SetHoverObject/SetPressObject
    m_pFocusObject = NULL;	// ������SetFocusObject����Ϊ���漰ˢ����
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
DLGC_UNDEFPUSHBUTTON   Control is a push button but not the default push button.(��һ����ť��������default)
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

    Object* pLastDefaultObj = m_pObjDefault; // ���ڼ�������defbtn

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

            // ����
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
                // �����ڶ��б༭���лس�
                UINT codeFocus = UISendMessage(m_pFocusObject, WM_GETDLGCODE, 0, (LPARAM)pMsg);
                if (codeFocus & (DLGC_WANTMESSAGE|DLGC_WANTALLKEYS))
                    return FALSE;
            }
            
            // ����default button�����͸�defpushbutton
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
            // ����ȡ����ť��������Ϣ
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
//	�ú���ֻ���������ط�����:
//		a. �����ѡ��һ��UI�ؼ�
//		b. ʹ��TAB�л��ؼ�
//
//	��˵������ѡ��һ��ϵͳ�ؼ�ʱ����Ҫ��HWNDHOST�е���һ��SetFocusObjectDirect
//
void WindowMouseMgr::SetFocusObject(Object* pObj)
{
    // TODO: ��������ᵼ�½����ϵͳ��combobox.edit�����ui combobox.editʱû��Ӧ
    //       ��������������ᵼ��UI COMBOBOX�е�EDIT�޷���קѡ�У���Ϊһֱ��KILLFOCUS/SETFOCUS��
    if (m_pFocusObject == pObj)  
        return;

    if (pObj)
    {
        if (OBJ_HWNDHOST == pObj->GetObjectType())
        {
            HWND hWndFocus = ::GetFocus();
            HWND hWndHost = ((HwndHost*)pObj)->m_hWnd;
            if (hWndFocus != hWndHost && FALSE == ::IsChild(hWndHost, hWndFocus))  // �п��ܵ����combobox�е�edit,edit����һ��HOSTWND������HOSTWND���Ӵ���
            {
                m_pOldFocusObject = m_pFocusObject;   // ��HwndHost::WndProc WM_SETFOCUS��ʹ��
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
                m_pOldFocusObject = m_pFocusObject;   // ��KeyboardManager::SetFocus��ʹ��
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

    // ���ݽ�������л����뷨״̬
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
    // ����������û�������׸��������ĸ�����
    // ����һ��λ��ScrollPanel�е�Edit����Edit
    // �н���MouseWheel�Ļ�����Ӧ�ù���ScrollPanel

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