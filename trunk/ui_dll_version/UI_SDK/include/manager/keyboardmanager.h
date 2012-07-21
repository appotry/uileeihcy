#pragma once

namespace UI
{

//
//	1. ���Ҫ����Ĭ�ϵ�һ���ؼ��Ľ��㣬������OnInitDialog����FALSE��
//
class KeyboardManager
{
public:
	KeyboardManager( );
	~KeyboardManager(void);

private:
	WindowBase*         m_pWindow;             // Ҫ����Ĵ��ڶ���
	Object*             m_pFocusObject;        // ��ǰ���ý���Ķ���
	Object*             m_pOldFocusObject;     // ��ʱ�������ڼ�¼���㷢���仯�����еľɵĽ������

public:
	void     SetWindow( WindowBase* pWnd );
	LRESULT  HandleMessage( UINT msg, WPARAM w, LPARAM l );
	
	void     SetFocusObject( Object* pObj );
	void     SetFocusObjectDirect( Object* pObj );
	Object*  GetFocusObject( );
	Object*  GetOldFocusObject( bool bRemove = true );
	
	void     OnObjectHideInd(Object* pObj);

private:
	void     KillFocus();
	void     SetFocus();
	void     OnChar( WPARAM,LPARAM );
	void     KeyDown( WPARAM,LPARAM );
	void     KeyUp( WPARAM,LPARAM );
	void     NcDestroy();
	void     MouseWheel( WPARAM, LPARAM );

public:
	void     Tab_2_NextControl();
	void     Tab_2_PrevControl();
};

}