#pragma once

namespace UI
{

class MouseManager
{
public:
	MouseManager( );
	~MouseManager(void);

private:
	WindowBase*         m_pWindow;             // Ҫ����Ĵ��ڶ���
	KeyboardManager*    m_pKeyboardManager;    // ����m_pWindow��Ӧ��KeyboardManager

	Object*             m_pObjHover;           // �Ƿ��ж������������
	Object*             m_pObjPress;           // �Ƿ��ж����ѱ�����
	BOOL                m_bMouseTrack;         // �Ƿ���Ҫ�������뿪m_pWnd��

	HWND                m_hToolTip;            // �ô���ʹ�õ���ʾ�����ھ��
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