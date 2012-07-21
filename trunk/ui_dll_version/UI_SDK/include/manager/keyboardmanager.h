#pragma once

namespace UI
{

//
//	1. 如果要设置默认第一个控件的焦点，必须让OnInitDialog返回FALSE；
//
class KeyboardManager
{
public:
	KeyboardManager( );
	~KeyboardManager(void);

private:
	WindowBase*         m_pWindow;             // 要处理的窗口对象
	Object*             m_pFocusObject;        // 当前有用焦点的对象
	Object*             m_pOldFocusObject;     // 临时对象，用于记录焦点发生变化过程中的旧的焦点对象

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