#pragma once
#include <richedit.h>
#include <textserv.h>
#include "CaretWindow.h"
#pragma comment(lib, "Riched20.lib")
class   RichEditBase;

//
//
//	Q1. �޴���RE����ô�����ģ�
//		a. ����CreateTextSevices()
//      b. ??
//
//  Q2. �޴���RE����ô���Ƶ���������ģ�
//		a. �õ�һ���ڴ�DC?
//		b. ����itextservices->txdraw����������ṩһ��clientrect�����������ڸô�����������ʾһ����ɫ������
//		c. 
//
//	Q3. �޴���RE����ô�����Լ��ڴ����ϵķ�Χ�ģ�
//		a. Draw��ʱ�������clientrect������ʾ�ķ�Χ
//		b. Ȼ��������ʱ���жϵ�ǰ����Ƿ�λ�ڴ����ڻ��߷���TxGetClientRect
//	
//	Q4. ��ô�������֣�
//		a. ��WM_CHAR��Ϣ���ݸ�service text���ɡ���Ҫע�����ģ̬�Ի������ղ���WM_CHAR��Ϣ�ġ�
//
//	Q5. �������ֺ�����ôˢ����ʾ�ģ�
//		a. �ڲ�������TxInvalidateRect�ӿڣ��ڸýӿ��У������Լ�ȥʵ��Ϊ::InvalidateRect
//
//	Q6. ��ô����������ʽ��������б༭���ܣ�
//		a. TxGetPropertyBits ���ص�
//
//	Q7. ����ѡ������ʱ�����ֹ���λ����ʾ��̫׼ȷ����ô��������ѡ������ʱ����ʾ��ꣿ
//		a. ��ʵ���ʱ��TxCreateCaret(HBITMAP hbmp ���hbmp�ǲ�Ϊ�յģ�ʹ����������caret�Ͳ�����ʾ��why?
//
//  Q8. Ϊʲô����CCaretWindowʵ�ֹ��ʱ������������������ʱ��ϵͳ������أ�
//		
//
//	Q9. �������ô���ٵģ�
//		a. ��text service����kill focus��Ϣ�����һ��ô���set focus��Ϣ�������ٴεõ�����ʱ��겻��ʾ��
//
//  Q10.Ϊʲô����textservice����SETFOCUS��Ϣ��ȴ������ӦTxCreateCaret��Ϣ
//		a. ��Ϊû�е���OnTxInPlaceActivate
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
//	virtual HRESULT		TxGetClientRect(LPRECT prc);
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
	SIZE    m_sizeExtent;   // text service ����ʵ�����ŵĲ�����Each HIMETRIC unit corresponds to 0.01 millimeter.
	int     m_nxPerInch;
	int     m_nyPerInch;    
	DWORD   m_dwStyle;      // �༭����ʽ

	HWND    m_hParentWnd;
	CCaret  m_caret;
};

interface ITextEditControl
{
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;
};

// �����ж�
#define PRE_HANDLE_MSG() \
	{ \
		SetMsgHandled(TRUE); \
		lResult = OnPreHandleMsg(hWnd,uMsg,wParam,lParam); \
		if(IsMsgHandled()) \
			return FALSE; \
	}

// Ĭ�ϴ���
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
	WindowlessRichEdit(RichEditBase*);
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// �����Ӹ�������ת����������Ϣ

	BEGIN_MSG_MAP_EX(WindowlessRichEdit)
		PRE_HANDLE_MSG()
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_MOVING(OnMoving)

		// ����Ҫ������������Ϣ
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
	bool    Create(HWND hWndParent);
	void    Draw(HDC);
	bool    HitTest(POINT pt);

public:
	// IUnknown  Interface
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

	// ITextHost Interface
	HRESULT TxGetClientRect(LPRECT prc);

protected:
	CComPtr<ITextServices>  m_spTextServices;
	RichEditBase*           m_pRichEditBase;
};