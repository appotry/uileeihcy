#include "stdafx.h"
#include "popupcontrolwindow.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Control\Src\Control\Menu\menu.h"

namespace UI
{

PopupControlWindow::PopupControlWindow()
{
	m_pObject = NULL;
	m_bExitLoop = false;
    m_bMouseIn = false;
	m_hWndClickFrom = NULL;
	m_rcClickFrom.SetRectEmpty();
}

//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
// ע������Լ��ĸ����ڲ������棬�򲻻���������������ʾͼ��
BOOL PopupControlWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    if (NULL == pcs->hwndParent)
    {
    	pcs->dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW;
    }
    else
    {
        pcs->dwExStyle |= WS_EX_NOACTIVATE;
    }
	pcs->lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;
	
    SetMsgHandled(FALSE);
    return TRUE;
}

void  PopupControlWindow::Create(IObject*  pBindObj, const TCHAR* szId, HWND hParentWnd)
{
    m_pObject = pBindObj;
    __super::Create(GetUIApplication(), _T(""), hParentWnd);  // �ڴ�����panel��layer֮���ٴ������ڣ������һ����Ӧ_OnSize��û��layout

    IMapAttribute*  pMapAttrib = NULL;
    pBindObj->GetMapAttribute(&pMapAttrib);
    if (pMapAttrib)
    {
        IMapAttribute*  pMapAttribCopy = NULL;
        pMapAttrib->CreateCopy(&pMapAttribCopy);

        if (szId)
            this->SetID(szId);

		SERIALIZEDATA data = {0};
		data.pMapAttrib = pMapAttrib;
		data.nFlag = SERIALIZEFLAG_LOAD;
		UISendMessage(this, UI_WM_SERIALIZE, (WPARAM)&data);
//        UISendMessage(this, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);

        SAFE_RELEASE(pMapAttribCopy);
        SAFE_RELEASE(pMapAttrib);
    }
}
 
void  PopupControlWindow::SetPopupFromInfo(HWND hWnd, RECT* prcClickInWnd)
{
	m_hWndClickFrom = hWnd;
	if (prcClickInWnd)
		m_rcClickFrom.CopyRect(prcClickInWnd);
	else
		m_rcClickFrom.SetRectEmpty();
}

void  PopupControlWindow::Show(POINT pt, BOOL bDoModal)
{
    if (NULL == m_pObject)
        return;

    m_bExitLoop = false;
    UISendMessage(m_pObject, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);

    SIZE size = m_pObject->GetDesiredSize();

    CRect  rcWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
    if (pt.x < rcWorkArea.left)
        pt.x = rcWorkArea.left;
    if (pt.x + size.cx > rcWorkArea.right)
        pt.x = rcWorkArea.right - size.cx;
    if (pt.y < rcWorkArea.top)
        pt.y = rcWorkArea.top;
    if (pt.y + size.cy > rcWorkArea.bottom)
        pt.y = rcWorkArea.bottom - size.cy;

    HWND hPopupWnd = GetHWND();
    ::SetWindowPos(hPopupWnd, NULL, pt.x, pt.y, size.cx, size.cy, SWP_NOZORDER|SWP_NOACTIVATE);
	if (!GetUIApplication()->IsDesignMode())
	{
		::ShowWindow(hPopupWnd, SW_SHOWNA); 
		::UpdateWindow(hPopupWnd);

		IMessageFilterMgr* pMgr = NULL;
		GetUIApplication()->GetMessageFilterMgr(&pMgr);
		pMgr->AddMessageFilter(static_cast<IPreTranslateMessage*>(this));

		if (TRUE == bDoModal)
			GetUIApplication()->MsgHandleLoop(&m_bExitLoop);
	}
}

// ������ⲿ���õĻ�������Ҫ�Ƴ�pretranslatemessage?/?
void  PopupControlWindow::Hide()
{
    m_bExitLoop = true;

    if (!IsWindowVisible(GetHWND()))  // �Ѿ�������
        return ;

    ::ShowWindow(GetHWND(), SW_HIDE);

    if (m_pObject)
    {
        UISendMessage(m_pObject, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);  // ������Ϣ�������﷢�ͣ���֤ÿһ���Ӳ˵���������ʱ�����յ�����Ϣ
    }

    // ֪ͨ���󴰿ڱ�����
    IMessageFilterMgr* pMgr = NULL;
    GetUIApplication()->GetMessageFilterMgr(&pMgr);
    pMgr->RemoveMessageFilter(static_cast<IPreTranslateMessage*>(this));
}

void  PopupControlWindow::Destroy()
{
    ::DestroyWindow(GetHWND());
}

void PopupControlWindow::OnInitialize()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    SetWindowResizeType(WRSB_NONE);

    if (m_pObject)
    {
	    m_pObject->AddHook(this, 0, 1);
	    this->AddChild(m_pObject);
    }
}

void PopupControlWindow::OnDestroy()
{
	SetMsgHandled(FALSE);

	// ���ⴰ����DestroyChildObject��delete child object
	// ͬʱ����������root menuʱ�����������Ӳ˵��Ĵ��ڣ���������ｫ�Ӳ˵���parentҲ�����
	if (m_pObject)
	{
		m_pObject->RemoveHook(this);
		this->Standalone();
		m_pObject->SetParentObjectDirect(NULL);
	}
}

// Popup�ؼ������뻻����Ŀǰ
void  PopupControlWindow::OnSkinChanging(BOOL* pbChange)
{
    *pbChange = FALSE;
}

// ������������ڵ�ʱ�򣬲�Ҫȥ��ռ�����ڵĽ���
// ��������ͬʱ�����������Ϣ�ͼ�����Ϣ��ȡ��������
// WM_NCHITTEST���յ�
int PopupControlWindow::OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}
// �����Ϣ��������������������/�������㴰�ڵ�ʱ��
// ���������ص�֪ͨ�����ڵ�����Լ��Ķ��㴰��ȴ������
void PopupControlWindow::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	SetMsgHandled(FALSE);
	if (FALSE == bActive)
	{
		this->Hide();
	}
}



// ע��
//	�����GetMessage��ʵ��ֻ������PostMessage���͹�������Ϣ��
//  ��������SendMessage����Ϣ�ǲ��ܵ�֪�ġ�
//

BOOL PopupControlWindow::PreTranslateMessage(MSG* pMsg)
{
	if ((WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message) || WM_MOUSEWHEEL == pMsg->message)
	{
		if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
		{
			this->Hide();
			return TRUE;
		}

		BOOL bHandle = FALSE;
		UISendMessage(this->m_pObject, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		//return bHandle;
        return TRUE;
	}
	
	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
		if (pMsg->hwnd != GetHWND())
		{
            if (m_bMouseIn)
            {
                ::PostMessage(GetHWND(), WM_MOUSELEAVE, 0, 0);
                m_bMouseIn = false;
            }
			return TRUE;  // �����������ƶ��¼�����
		}
		else
		{
            m_bMouseIn = true;
			return FALSE;
		}
	}
	else if (WM_LBUTTONDOWN == pMsg->message || WM_LBUTTONDBLCLK == pMsg->message)
	{
		// ������������������һ���жϡ��п��ܸô��ǵ����˵��ĵط���
		// ֱ�ӹرղ˵��ᵼ�²˵����رպ���������ʾ�����˵� 

		RECT rcWindow;
		::GetWindowRect(GetHWND(), &rcWindow);
		if (!PtInRect(&rcWindow, pMsg->pt))  
		{
			this->Hide();

			// ��ԭ���ڷ���һ������ƶ���Ϣ������hover����
			// ����ᵼ��popupwnd��ʧ��ԭ�������ֱ�ӵ���޷�Ӧ
			// ���ߵ��´��ڽ��յ�lbuttondown֮ǰ����û�и���hover����
			::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

			// ����ڵ��������������ˣ��رյ�ǰ���ڡ���������˵������İ�ť���棬��ֱ�ӹرգ�����ת����Ϣ
			if (m_hWndClickFrom && m_hWndClickFrom == pMsg->hwnd)
			{
				POINT ptWnd = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};
				if (PtInRect(&m_rcClickFrom, ptWnd))
					return TRUE;
			}

			return FALSE/*TRUE*/;
		}
	}
	else if (
		WM_RBUTTONDOWN   == pMsg->message ||
		WM_RBUTTONDBLCLK == pMsg->message ||
		WM_MBUTTONDOWN   == pMsg->message ||
		WM_MBUTTONDBLCLK == pMsg->message ||
		WM_XBUTTONDOWN   == pMsg->message ||
		WM_XBUTTONDBLCLK == pMsg->message ||
		WM_NCLBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDBLCLK == pMsg->message )
	{
		RECT rcWindow;
		::GetWindowRect(GetHWND(), &rcWindow);
		if (!PtInRect(&rcWindow, pMsg->pt))  // ����ڵ��������������ˣ��رյ�ǰ����
		{
			this->Hide();
			
			// ��ԭ���ڷ���һ������ƶ���Ϣ������hover����
			// ����ᵼ��popupwnd��ʧ��ԭ�������ֱ�ӵ���޷�Ӧ
			// ���ߵ��´��ڽ��յ�lbuttondown֮ǰ����û�и���hover����
			::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

            return FALSE/*TRUE*/;
		}
	}
	
	return FALSE;
}

BOOL PopupControlWindow::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	return TRUE;
}

LRESULT PopupControlWindow::OnGetGraphicsRenderType()
{
	if (m_pObject)
	{
		// ע������û��ȥ����GetGraphicsRenderType����ΪGetGraphicsRenderType��m_pObject�ֻᷴ����m_pWindow->GetGraphicsRenderLibraryType
		//     �п��ܵ�����Ƕ��

		GRAPHICS_RENDER_LIBRARY_TYPE e = (GRAPHICS_RENDER_LIBRARY_TYPE)UISendMessage( m_pObject, UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE );
		if (GRAPHICS_RENDER_LIBRARY_TYPE_AUTO == e)
		{
			e = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		}
		return e;
	}
	return  GRAPHICS_RENDER_LIBRARY_TYPE_AUTO;
}


PopupListBoxWindow::PopupListBoxWindow()
{
}

BOOL PopupListBoxWindow::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		if (pMsg->message != WM_KEYDOWN)
			return FALSE;

		UINT nChar = pMsg->wParam;
		if (VK_ESCAPE == nChar)
		{
			this->Hide();
			return TRUE;
		}

		// ֻ����һЩ��������������������Ϣ���Ǵ��ݸ���ǰFOCUS COMBOBOX'S EDIT
		if (Util::IsKeyDown(VK_SHIFT))  // SHIFT + xxx ��EDIT����
			return FALSE;

		if (nChar == VK_UP    ||
			nChar == VK_DOWN  ||
			nChar == VK_HOME  ||
			nChar == VK_END   ||
			nChar == VK_PRIOR ||
			nChar == VK_NEXT)
		{
			BOOL bHandle = FALSE;
			UISendMessage(this->m_pObject, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
			return TRUE;/*bHandle*/; // �п��ܴ�������ʵ�ؼ�
		}

		return FALSE;
	}

	return __super::PreTranslateMessage(pMsg);
}

void PopupListBoxWindow::OnInitialize()
{
	__super::OnInitialize();

	// ���ԭ���������hover��press����
	HWND hWnd = GetActiveWindow();
	::PostMessage(hWnd,WM_MOUSELEAVE,0,0);
}

void PopupListBoxWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetMsgHandled(FALSE);

	if (VK_DOWN == nChar || VK_UP == nChar) 
	{
		SetMsgHandled(TRUE);
		m_pObject->ProcessMessage(GetCurMsg(), 0);
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
PopupMenuWindow::PopupMenuWindow()
{
	m_pMenu = NULL;
	m_ptLastMousePos.x = m_ptLastMousePos.y = 0;
    m_pLastHoverMenu = NULL;
}
void PopupMenuWindow::OnInitialize()
{
	__super::OnInitialize();
    if (m_pObject)
    {
        IMenu* p = (IMenu*)m_pObject->QueryInterface(uiiidof(IMenu));
        if (p)
            m_pMenu = p->GetImpl();
    }

    if (m_pMenu)
    {
	    // ���ԭ���������hover��press����
	    if (m_pMenu->GetRootMenu() == m_pMenu) // �Ӳ˵�������ʱ,activeWindow�Ǹ��˵�����˲���Ҫ�ٷ���mouseleave��Ϣ
	    {
		    HWND hWnd = GetActiveWindow();
		    ::PostMessage(hWnd,WM_MOUSELEAVE,0,0);
	    }
    }
}


//
//	ע��ֻ�и��˵��Ż���MenuLoop���Ӳ˵�û��
//
BOOL PopupMenuWindow::PreTranslateMessage(MSG* pMsg)
{
    if (NULL == m_pMenu)
        return FALSE;

    HWND hWnd = GetHWND();
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		if (WM_SYSKEYDOWN == pMsg->message && Util::IsKeyDown(VK_MENU))
		{
			this->Hide();   // ����ALT��ʱ�����������˵���(ESCֻ����last menu)
			return TRUE;
		}

		BOOL bHandle = FALSE;
        Menu* pMenu = m_pMenu->GetLastMenu();
        if (pMenu)
    		UISendMessage(pMenu->GetIMenu(), pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);

		return bHandle;
	}

	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
        // ���β˵���������Ϣ����ϵͳ��,��������뿪�˵���ʱ�򣬵ò���һ��MOUSELEAVE�¼�...(TODO: �ǲ������ĸ����)
        Menu*  pMenuHover = m_pMenu->GetMenuByHWND(pMsg->hwnd);
		if (NULL == pMenuHover)
		{
            if (m_pLastHoverMenu)
            {
                ::PostMessage(m_pLastHoverMenu->GetIMenu()->GetHWND(), WM_MOUSELEAVE, 0, 0);
            }
            m_pLastHoverMenu = NULL;
			return TRUE;  // �����������ƶ��¼�����
		}
		else
		{
            m_pLastHoverMenu = pMenuHover;
			POINT ptNow = {LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)};
			if (ptNow.x == m_ptLastMousePos.x && ptNow.y == m_ptLastMousePos.y)  // �����������Ӳ˵�����ʾ��������MOUSEMOVE������
			{
				UI_LOG_DEBUG(_T("%s point repeat"),FUNC_NAME);
				return TRUE;
			}

			m_ptLastMousePos = ptNow;
			return FALSE;
		}
	}
	else if (WM_LBUTTONDOWN   == pMsg->message ||
		WM_LBUTTONDBLCLK == pMsg->message ||
		WM_RBUTTONDOWN   == pMsg->message ||
		WM_RBUTTONDBLCLK == pMsg->message ||
		WM_MBUTTONDOWN   == pMsg->message ||
		WM_MBUTTONDBLCLK == pMsg->message ||
		WM_XBUTTONDOWN   == pMsg->message ||
		WM_XBUTTONDBLCLK == pMsg->message ||
		WM_NCLBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDBLCLK == pMsg->message)
	{
		RECT rcWindow;
		::GetWindowRect(GetHWND(), &rcWindow);
		if (NULL == m_pMenu->GetMenuByPos(pMsg->pt))  // ����ڵ��������������ˣ��رյ�ǰ����
		{
			this->Hide();

			// ��ԭ���ڷ���һ������ƶ���Ϣ������hover����
			// ����ᵼ��popupwnd��ʧ��ԭ�������ֱ�ӵ���޷�Ӧ
			// ���ߵ��´��ڽ��յ�lbuttondown֮ǰ����û�и���hover����
			
			HWND hWnd = GetActiveWindow();
			::SendMessage(hWnd, WM_MOUSEMOVE, 0, pMsg->lParam);
            
            // ����ǰ������Ĵ��ڷ���һ�������Ϣ�����ڽ���ڵ����˵���
            // ֱ�ӵ����һ�������ϵİ�ť��û�з�Ӧ
            if (pMsg->hwnd != hWnd)
                ::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

            return TRUE;
		}
	}

	return FALSE;
}
}