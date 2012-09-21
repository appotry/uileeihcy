#pragma once

namespace UI
{

class MouseManager
{
public:
	MouseManager( );
	~MouseManager(void);

private:
	WindowBase*         m_pWindow;             // 要处理的窗口对象
	KeyboardManager*    m_pKeyboardManager;    // 窗口m_pWindow对应的KeyboardManager

	Object*             m_pObjHover;           // 是否有对象处于鼠标下面
	Object*             m_pObjPress;           // 是否有对象已被按下
	BOOL                m_bMouseTrack;         // 是否需要监测鼠标离开m_pWnd了

	HWND                m_hToolTip;            // 该窗口使用的提示条窗口句柄
	TOOLINFO            m_toolinfo;

public:
	void     SetWindow( WindowBase* pWnd );
	void     SetKeyboardManager( KeyboardManager* pKeyboardManager );
	WindowBase* GetWindowObject() { return m_pWindow; }
	HWND     GetTooltipHWND()     { return m_hToolTip; }
	TOOLINFO*   GetToolInfoPtr()  { return &m_toolinfo; }

	LRESULT  HandleMessage( UINT msg, WPARAM w, LPARAM l );
	Object*  GetHoverObject();
	Object*  GetPressObject();

	void     OnObjectHideInd(Object* pObj);
	void     OnObjectDeleteInd(Object* pObj);
	
private:
	LRESULT  MouseMove ( int vkFlag, int xPos, int yPos );
	LRESULT  MouseLeave( int vkFlag, int xPos, int yPos );
	LRESULT  LButtonDown( int vkFlag, int xPos, int yPos );
	LRESULT  LButtonUp ( int vkFlag, int xPos, int yPos );
	LRESULT  RButtonDown(WPARAM,LPARAM);
	LRESULT  RButtonUp  (WPARAM,LPARAM);
	LRESULT  LButtonDBClick(WPARAM,LPARAM);
	void     KillFocus();
	void     SetFocus();
	BOOL     SetCursor( WPARAM,LPARAM );
	void     NcDestroy();

	void     SetHoverObject( Object* pNewHoverObj );
	void     SetPressObject( Object* pNewPressObj );

	void     CreateToolTip();
	void     DestroyToolTip();
	void     HideToolTip();
public:
	void     FixStringWordBreakUnderXP(const String& src, String& strOut);

public:
	static   Object*  GetObjectByPos( Object* pObjParent, POINT* pt );

};


}