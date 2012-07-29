#include "stdafx.h"


KeyboardManager::KeyboardManager()
{
	m_pWindow     = NULL;
	m_pFocusObject= NULL;
	m_pOldFocusObject = NULL;
}

KeyboardManager::~KeyboardManager(void)
{
	m_pWindow     = NULL;
}


void KeyboardManager::SetWindow( WindowBase* pWnd )
{
	this->m_pWindow = pWnd;
}

Object*  KeyboardManager::GetFocusObject( )
{
	return m_pFocusObject;
}
Object*  KeyboardManager::GetOldFocusObject(  bool bRemove )
{
	if( bRemove )
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

void KeyboardManager::SetFocusObjectDirect( Object* pObj )
{
	m_pFocusObject = pObj;
}

//
//	该函数只会在两个地方触发:
//		a. 鼠标点击选中一个UI控件
//		b. 使用TAB切换控件
//
//	因此当鼠标点击选中一个系统控件时，需要在HWNDHOST中调用一次SetFocusObjectDirect
//
void KeyboardManager::SetFocusObject( Object* pObj )
{
#ifdef _DEBUG
	return;
#endif
	 if( m_pFocusObject == pObj )
		 return;

	 if( NULL != pObj )
	 {
		 if( OBJ_HWNDHOST == pObj->GetObjectType() )
		 {
			 if( ::GetFocus() != ((HwndHost*)pObj)->m_hWnd )
			 {
				 m_pOldFocusObject = m_pFocusObject;   // 在HwndHost::WndProc WM_SETFOCUS中使用
				 m_pFocusObject = pObj;

				 HWND hWnd = ((HwndHost*)pObj)->m_hWnd;
				 if (GetWindowLong(hWnd, GWL_EXSTYLE)&WS_EX_NOACTIVATE)
					{}
				 else
					::SetFocus(hWnd);  
				 return;
			 }
		 }
		 else
		 {
			 if( ::GetFocus() != m_pWindow->m_hWnd )
			 {
				 m_pOldFocusObject = m_pFocusObject;   // 在KeyboardManager::SetFocus中使用
				 m_pFocusObject = pObj;

				 HWND hWnd = m_pWindow->m_hWnd;
				 if (GetWindowLong(hWnd, GWL_EXSTYLE)&WS_EX_NOACTIVATE)
					{int a = 0;}
				 else
					 ::SetFocus(hWnd);  

				 return;
			 }
		 }
	 }

	 if( NULL != m_pFocusObject )
	 {
		 int nOldStateBits = m_pFocusObject->GetStateBit();
		 m_pFocusObject->SetFocus(false);
		 ::UISendMessage(m_pFocusObject, WM_KILLFOCUS, (WPARAM)pObj, NULL );
		 ::UISendMessage(m_pFocusObject,UI_WM_STATECHANGED, nOldStateBits, m_pFocusObject->GetStateBit());
	 }
	 if( NULL != pObj )
	 {
		 int nOldStateBits = pObj->GetStateBit();
		 pObj->SetFocus(true);
		 ::UISendMessage(pObj, WM_SETFOCUS, (WPARAM)m_pFocusObject, NULL );
		 ::UISendMessage(pObj,UI_WM_STATECHANGED, nOldStateBits, pObj->GetStateBit());
	 }
	 m_pFocusObject = pObj;
}



//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息处理                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////



LRESULT KeyboardManager::HandleMessage( UINT msg, WPARAM w, LPARAM l )
{
	switch( msg )
	{
	case WM_KILLFOCUS:
		this->KillFocus();
		break;

	case WM_SETFOCUS:
		this->SetFocus();
		break;

	case WM_CHAR:
		this->OnChar( w, l );
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		this->KeyDown( w,l );
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		this->KeyUp( w, l );
		break;

	case WM_NCDESTROY:
		this->NcDestroy();
		break;

	case WM_MOUSEWHEEL:
		this->MouseWheel(w,l);
	}
	return 0L;
}

void KeyboardManager::NcDestroy()
{
	this->m_pFocusObject = NULL;	// 不调用SetFocusObject，因为不涉及刷新了
	this->m_pOldFocusObject = NULL;
}

void KeyboardManager::KillFocus()
{
	Object* p = this->GetOldFocusObject();
	if( NULL == p )
		p = m_pFocusObject;

	if( NULL != p )
	{
		int nOldStateBits = m_pFocusObject->GetStateBit();
		p->SetFocus(false);
		::UISendMessage( p, WM_KILLFOCUS, (WPARAM)NULL, (LPARAM)0 );
		::UISendMessage( p,UI_WM_STATECHANGED, nOldStateBits, m_pFocusObject->GetStateBit() );
	}
}
 
void KeyboardManager::SetFocus()
{
	if( NULL != m_pFocusObject )
	{
		int nOldStateBits = m_pFocusObject->GetStateBit();
		m_pFocusObject->SetFocus(true);
		::UISendMessage( m_pFocusObject, WM_SETFOCUS, (WPARAM)GetOldFocusObject(), (LPARAM)0 );  // 这里的GetOldFocusObject对应于
		::UISendMessage( m_pFocusObject,UI_WM_STATECHANGED, nOldStateBits, m_pFocusObject->GetStateBit() );
	}
	else
	{
		this->Tab_2_NextControl();
	}
}

void KeyboardManager::OnChar( WPARAM w, LPARAM l )
{	
	if( this->m_pFocusObject != NULL )
	{
		UISendMessage( m_pFocusObject, WM_CHAR, w, l );
	}
}

void KeyboardManager::KeyDown( WPARAM w,LPARAM l )
{
	if( this->m_pFocusObject != NULL )
	{
		UISendMessage( m_pFocusObject, WM_KEYDOWN, w, l );
	}
}
void KeyboardManager::KeyUp( WPARAM w,LPARAM l )
{
	if( this->m_pFocusObject != NULL )
	{
		UISendMessage( m_pFocusObject, WM_KEYUP, w, l );
	}
}

void KeyboardManager::MouseWheel( WPARAM w, LPARAM l )
{
	if( this->m_pFocusObject != NULL )
	{
		UISendMessage( m_pFocusObject, WM_MOUSEWHEEL, w, l );
	}
}

void KeyboardManager::Tab_2_NextControl()
{
	Object* p = m_pFocusObject;
	if( NULL == p )
	{
		p = this->m_pWindow;
	}
	p = p->GetNextTabObject();

	if( NULL != p )
	{
		SetFocusObject(p);
	}
}
void KeyboardManager::Tab_2_PrevControl()
{
	Object* p = m_pFocusObject;
	if( NULL == p )
	{
		p = this->m_pWindow;
	}
	p = p->GetPrevTabObject();

	if( NULL != p )
	{
		SetFocusObject(p);
	}
}

 
//
//	当一个对象隐藏时，发送该通知给keyboardmgr切换新的焦点
//
void KeyboardManager::OnObjectHideInd(Object* pObj)
{
	if( NULL == pObj )
		return;

	if( pObj == m_pFocusObject )
	{
		m_pFocusObject = NULL;
		this->Tab_2_NextControl();
	}
}