#pragma once
#include "LayeredWindow.h"
class  ITextHost;

//
// ����������4������⣺ʹ�ô�����Ϊ�����ڱ༭�����������֣����᲻�ϵ���˸!
//
// 1. ֻҪ��setwindowpos move or size/show hide window���ͻᵼ�µ�ǰ����µĴ��ڽ��ܵ�һ��NCHITTEST/SETCURSOR/MOUSEMOVE����Ϣ��
//    �������Ƿ���һ���ֲ㴰��
// 2. ͨ�����ý�CCaretWindow�ֲ��ο�ʵ�ֵ����أ����Դﵽ��������������Ϣ������setcaretposҪ��ô�������ķ�����ʵ�֣�
// 3. Mouse Vanish  -- û�ã��������޸Ĳ��ˣ������ڿ������--��������ø�ѡ��Ҳ��������
// 4. Ӧ�ò���ʹ��showcursorʵ�ֵ����ع�꣬��������ƶ�ʱ�Ͳ�����ʾ��
// 5. �ѵ���setcursor(null)? ��ô�ƽ⣿
//
//    --> ������Ϊ�ڲ�ֱ�ӵ��õ�SetCursor(NULL)���µ�
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
	POINT       m_ptOldPos;   // �������������ƶ���ʱ��ά�ֹ����EDIT�е�λ��
//	ITextHost*  m_pTextHost;
};


//////////////////////////////////////////////////////////////////////////
// ���ⲿ�ṩһ�׹��������������Ҫ�ڷֲ㴰��������ʾһ�����ʱ��
// ������һ��CCaretWindow������Ҫ����ͨ����������ʾһ�����ʱ����
// ����ϵͳ��API

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