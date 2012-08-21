#pragma once
#include <TextServ.h>
#include "CaretWindow.h"
#pragma comment(lib, "Riched20.lib")

//
//
//	Q1. 无窗口RE是怎么创建的？
//		a. 调用CreateTextSevices()
//      b. ??
//
//  Q2. 无窗口RE是怎么绘制到窗口上面的？
//		a. 拿到一个内存DC?
//		b. 调用itextservices->txdraw方法，随便提供一个clientrect，这样就能在该窗口区域上显示一个白色背景了
//		c. 
//
//	Q3. 无窗口RE是怎么决定自己在窗口上的范围的？
//		a. Draw的时候给出的clientrect就是显示的范围
//		b. 然后在其它时候判断当前鼠标是否位于窗口内或者返回TxGetClientRect
//	
//	Q4. 怎么输入文字？
//		a. 将WM_CHAR消息传递给service text即可。需要注意的是模态对话框是收不到WM_CHAR消息的。
//
//	Q5. 输入文字后是怎么刷新显示的？
//		a. 内部将调用TxInvalidateRect接口，在该接口中，我们自己去实现为::InvalidateRect
//
//	Q6. 怎么设置它的样式？例如多行编辑功能？
//		a. TxGetPropertyBits 返回的
//
//	Q7. 当有选中区域时，发现光标的位置显示不太准确，怎么才能在有选中区域时不显示光标？
//		a. 其实这个时候TxCreateCaret(HBITMAP hbmp 这个hbmp是不为空的，使用它创建的caret就不会显示。why?
//
//  Q8. 为什么我用CCaretWindow实现光标时，不能做到输入文字时将系统光标隐藏？
//		
//
//	Q9. 光标是怎么销毁的？
//		a. 向text service传递kill focus消息，而且还得传递set focus消息，否则再次得到焦点时光标不显示。
//

class ITextHostImpl : public ITextHost
{
public:
	ITextHostImpl();

	// ITextHost Interface
	virtual HDC 		TxGetDC();
	virtual INT			TxReleaseDC(HDC hdc);
	virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL 		TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	virtual BOOL 		TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
	virtual BOOL 		TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	virtual void		TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual void 		TxViewChange(BOOL fUpdate);
	virtual BOOL		TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
	virtual BOOL		TxShowCaret(BOOL fShow);
	virtual BOOL		TxSetCaretPos(INT x, INT y);
	virtual BOOL 		TxSetTimer(UINT idTimer, UINT uTimeout);
	virtual void 		TxKillTimer(UINT idTimer);
	virtual void		TxScrollWindowEx (INT dx,INT dy,LPCRECT lprcScroll,LPCRECT lprcClip,HRGN hrgnUpdate,LPRECT lprcUpdate,UINT fuScroll);
	virtual void		TxSetCapture(BOOL fCapture);
	virtual void		TxSetFocus();
	virtual void 	    TxSetCursor(HCURSOR hcur, BOOL fText);
	virtual BOOL 		TxScreenToClient (LPPOINT lppt);
	virtual BOOL		TxClientToScreen (LPPOINT lppt);
	virtual HRESULT		TxActivate( LONG * plOldState );
	virtual HRESULT		TxDeactivate( LONG lNewState );
	virtual HRESULT		TxGetClientRect(LPRECT prc);
	virtual HRESULT		TxGetViewInset(LPRECT prc);
	virtual HRESULT 	TxGetCharFormat(const CHARFORMATW **ppCF );
	virtual HRESULT		TxGetParaFormat(const PARAFORMAT **ppPF);
	virtual COLORREF	TxGetSysColor(int nIndex);
	virtual HRESULT		TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT		TxGetMaxLength(DWORD *plength);
	virtual HRESULT		TxGetScrollBars(DWORD *pdwScrollBar);
	virtual HRESULT		TxGetPasswordChar(TCHAR *pch);
	virtual HRESULT		TxGetAcceleratorPos(LONG *pcp);
	virtual HRESULT		TxGetExtent(LPSIZEL lpExtent);
	virtual HRESULT 	OnTxCharFormatChange (const CHARFORMATW * pcf);
	virtual HRESULT		OnTxParaFormatChange (const PARAFORMAT * ppf);
	virtual HRESULT		TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	virtual HRESULT		TxNotify(DWORD iNotify, void *pv);
	// Far East Methods for getting the Input Context
	//#ifdef WIN95_IME
	virtual HIMC		TxImmGetContext();
	virtual void		TxImmReleaseContext( HIMC himc );
	//#endif
	virtual HRESULT		TxGetSelectionBarWidth (LONG *lSelBarWidth);

protected:
	SIZE    m_sizeExtent;   // text service 用来实现缩放的参数。Each HIMETRIC unit corresponds to 0.01 millimeter.
	int     m_nxPerInch;
	int     m_nyPerInch;    
	DWORD   m_dwStyle;      // 编辑框样式

	CRect   m_rcBorder; 
	CRect   m_rectInWindow;

	HWND    m_hParentWnd;
	CCaret  m_caret;
};

interface ITextEditControl
{
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;
};

// 条件判断
#define PRE_HANDLE_MSG() \
	{ \
		SetMsgHandled(TRUE); \
		lResult = OnPreHandleMsg(hWnd,uMsg,wParam,lParam); \
		if(IsMsgHandled()) \
			return FALSE; \
	}

// 默认处理
#define POST_HANDLE_MSG() \
	{ \
		SetMsgHandled(TRUE); \
		lResult = OnPostHandleMsg(hWnd,uMsg,wParam,lParam); \
		if(IsMsgHandled()) \
			return TRUE; \
	}

class WindowlessRichEdit :	public ITextHostImpl, public ITextEditControl
{
public:
	WindowlessRichEdit(void);
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// 处理从父窗口中转发过来的消息

	BEGIN_MSG_MAP_EX(WindowlessRichEdit)
		PRE_HANDLE_MSG()
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_MOVING(OnMoving)

		// 必须要处理的其它消息
		MESSAGE_HANDLER_EX(WM_KEYDOWN, OnDefaultHandle)
		MESSAGE_HANDLER_EX(WM_CHAR, OnChar)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST, OnDefaultHandle)
		MESSAGE_HANDLER_EX(WM_SETFOCUS, OnDefaultHandle)
//		POST_HANDLE_MSG()		
	END_MSG_MAP()

protected:
	LRESULT OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	BOOL    OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	LRESULT OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnKillFocus(HWND wndFocus);
	void    OnMoving(UINT fwSide, LPRECT pRect);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	bool    Create(HWND hWndParent, CRect rcInWnd);
	void    Draw(HDC);
	bool    HitTest(POINT pt);

public:

	// IUnknown  Interface
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

protected:
	CComPtr<ITextServices>  m_spTextServices;
};
