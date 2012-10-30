#pragma once
#include "LayeredWindow.h"
class  ITextHost;

//
// 连续困扰了4天的问题：使用窗口作为光标后，在编辑框中输入文字，鼠标会不断的闪烁!
//
// 1. 只要有setwindowpos move or size/show hide window，就会导致当前鼠标下的窗口接受到一个NCHITTEST/SETCURSOR/MOUSEMOVE的消息，
//    无论这是否是一个分层窗口
// 2. 通过采用将CCaretWindow分部镂空实现的隐藏，可以达到不发送这样的消息。但是setcaretpos要怎么用这样的方法来实现？
// 3. Mouse Vanish  -- 没用，代码中修改不了，而且在控制面板--鼠标中设置该选项也不起作用
// 4. 应该不是使用showcursor实现的隐藏光标，否则鼠标移动时就不会显示了
// 5. 难道是setcursor(null)? 怎么破解？
//
//    --> 就是因为内部直接调用的SetCursor(NULL)导致的
//

class CCaretWindow : public CWindowImpl<CCaretWindow>, public CLayeredWindow<CCaretWindow> 
{
public:
	CCaretWindow(void);
	~CCaretWindow(void);

	DECLARE_WND_CLASS(_T("CaretWindow"))	

	BEGIN_MSG_MAP_EX(CCaretWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(UI_WM_SYNC_WINDOWPOSCHANING, OnSyncWindowPosChanging);
	END_MSG_MAP()

protected:
	int     OnCreate(LPCREATESTRUCT lpCreateStruct);
	void    OnDestroy();
	void    OnPaint(HDC hDC);
	void    OnTimer(UINT_PTR nIDEvent);
	LRESULT OnSyncWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
public:
	void   CreateCaret(HWND hWndParent, int nWidth, int nHeight);
	void   DestroyCaret();
	void   SetCaretPos(int x, int y);
	void   ShowCaret();
	void   HideCaret();

	void   OnWindowMove();
	void   OnControlMove();
//	void   SetTextHost(ITextHost* pTextHost){ m_pTextHost = pTextHost; }

private:
	POINT       m_ptOldPos;   // 用于在主窗口移动的时候维持光标在EDIT中的位置
//	ITextHost*  m_pTextHost;
};


//////////////////////////////////////////////////////////////////////////
// 对外部提供一套光标解决方案，当需要在分层窗口上面显示一个光标时，
// 将创建一个CCaretWindow，当需要在普通窗口上面显示一个光标时，则
// 调用系统的API

class CCaret
{
public:
	CCaret(){ m_bLayered = false; m_hWnd = NULL; }
	void   SetLayered(bool b);

	void   CreateCaret(HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
	void   DestroyCaret();
	void   SetCaretPos(int x, int y);
	void   ShowCaret();
	void   HideCaret();

	void   OnWindowMove();
	void   OnControlMove();

	void   SetTextHost(ITextHost* pTextHost);
	bool   IsLayered() { return m_bLayered; }

private:
	HWND          m_hWnd;
	bool          m_bLayered;
	CCaretWindow  m_caretwnd;
};