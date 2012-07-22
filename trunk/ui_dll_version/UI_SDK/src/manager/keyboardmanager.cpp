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
//	�ú���ֻ���������ط�����:
//		a. �����ѡ��һ��UI�ؼ�
//		b. ʹ��TAB�л��ؼ�
//
//	��˵������ѡ��һ��ϵͳ�ؼ�ʱ����Ҫ��HWNDHOST�е���һ��SetFocusObjectDirect
//
void KeyboardManager::SetFocusObject( Object* pObj )
{
	 if( m_pFocusObject == pObj )
		 return;

	 if( NULL != pObj )
	 {
		 if( OBJ_HWNDHOST == pObj->GetObjectType() )
		 {
			 if( ::GetFocus() != ((HwndHost*)pObj)->m_hWnd )
			 {
				 m_pOldFocusObject = m_pFocusObject;   // ��HwndHost::WndProc WM_SETFOCUS��ʹ��
				 m_pFocusObject = pObj;
				 ::SetFocus( ((HwndHost*)pObj)->m_hWnd );  
				 return;
			 }
		 }
		 else
		 {
			 if( ::GetFocus() != m_pWindow->m_hWnd )
			 {
				 m_pOldFocusObject = m_pFocusObject;   // ��KeyboardManager::SetFocus��ʹ��
				 m_pFocusObject = pObj;
				 ::SetFocus( m_pWindow->m_hWnd );
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
//                               ��Ϣ����                               //
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
	this->m_pFocusObject = NULL;	// ������SetFocusObject����Ϊ���漰ˢ����
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
		::UISendMessage( m_pFocusObject, WM_SETFOCUS, (WPARAM)GetOldFocusObject(), (LPARAM)0 );  // �����GetOldFocusObject��Ӧ��
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
//	��һ����������ʱ�����͸�֪ͨ��keyboardmgr�л��µĽ���
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