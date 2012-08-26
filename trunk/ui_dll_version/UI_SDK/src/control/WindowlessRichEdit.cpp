#include "StdAfx.h"
#include "control\WindowlessRichEdit.h"

// 备注：在CreateTextSerives之后，调用QueryInterface获取ITextServices接口时，总是返回E_NOINTERFACE
//       问题原来是从riched20.lib联系在一起的IID_ITextServices，是错误的。
const IID IID_ITextServices_Fix = { 0x8d33f740, 0xcf58, 0x11ce, {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5} };


// HIMETRIC units per inch (used for conversion)
#define HIMETRIC_PER_INCH 2540

// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
{
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
{
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}


WindowlessRichEdit::WindowlessRichEdit(RichEditBase* pRichEditBase)
{
	m_pRichEditBase = pRichEditBase;
}

WindowlessRichEdit::~WindowlessRichEdit(void)
{
	m_pRichEditBase = NULL;
}

bool WindowlessRichEdit::Create(HWND hWndParent)
{
	//////////////////////////////////////////////////////////////////////////
	// 创建Text Service

	//GdiSetBatchLimit(1);  ///??为什么都会加这么一句代码？

	IUnknown* pUnknown = NULL;
	HRESULT hr = ::CreateTextServices(NULL, static_cast<ITextHost*>(this), &pUnknown);
	if (FAILED(hr))
		return false;

	hr = pUnknown->QueryInterface(IID_ITextServices_Fix, (void**)&m_spTextServices);
 	pUnknown->Release();

	if (FAILED(hr) || NULL == m_spTextServices)
		return false;


	m_hParentWnd = hWndParent;
	if (GetWindowLong(m_hParentWnd, GWL_EXSTYLE) & WS_EX_LAYERED)
	{
		m_caret.SetLayered(true);
		m_caret.SetTextHost(this);
	}


	RECT  rcClient;
	this->TxGetClientRect(&rcClient);
	hr = m_spTextServices->OnTxInPlaceActivate(&rcClient);
	assert(SUCCEEDED(hr));
// 	hr = m_spTextServices->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
// 	assert(SUCCEEDED(hr));

	//////////////////////////////////////////////////////////////////////////
	// 参数初始化
	


	return true;
}

void WindowlessRichEdit::Draw(HDC hDC)
{
	if(NULL == m_spTextServices)
		return;

// 	HBRUSH hBrush = ::CreateSolidBrush(RGB(240,240,240));
	RECT  rcClient;
	this->TxGetClientRect(&rcClient);
// 	::FillRect(hDC, &rcClient, hBrush);
// 	::DeleteObject(hBrush);

	::OffsetRect(&rcClient, -rcClient.left, -rcClient.top);

	m_spTextServices->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hDC,
		NULL, (RECTL *)&rcClient, NULL, NULL, NULL, NULL, TXTVIEW_ACTIVE);
}

// handled表示richedit不处理这个消息
LRESULT WindowlessRichEdit::OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	SetMsgHandled(FALSE);
	if(NULL == m_spTextServices) 
	{
		SetMsgHandled(TRUE);
		return 0;
	}

	return 0;
}
LRESULT WindowlessRichEdit::OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lr = 0;
	HRESULT hr = m_spTextServices->TxSendMessage(Msg, wParam, lParam, &lr);

	if (hr == S_FALSE)
	{
		lr = ::DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return lr;
}

LRESULT WindowlessRichEdit::OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return OnPostHandleMsg(m_hParentWnd, uMsg, wParam, lParam);
}

BOOL WindowlessRichEdit::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	SetMsgHandled(FALSE);
	POINT pt;
	GetCursorPos(&pt);
	::ScreenToClient(m_hParentWnd, &pt);

	if (false == HitTest(pt))
		return FALSE;

	RECT  rcClient;
	this->TxGetClientRect(&rcClient);

	HDC hdc = GetDC(m_hParentWnd);
	HRESULT hr = m_spTextServices->OnTxSetCursor(
		DVASPECT_CONTENT,	
		-1,
		NULL,
		NULL,
		hdc,
		NULL,
		&rcClient,
		pt.x, 
		pt.y);
	ReleaseDC(m_hParentWnd, hdc);

	if (SUCCEEDED(hr))
	{
		SetMsgHandled(TRUE);
	}
	return TRUE;
}

void WindowlessRichEdit::OnKillFocus(HWND wndFocus)
{
	m_caret.DestroyCaret();
	SetMsgHandled(FALSE);
}

void WindowlessRichEdit::OnMoving(UINT fwSide, LPRECT pRect)
{
	SetMsgHandled(FALSE);
	m_caret.OnWindowMove();
}

bool WindowlessRichEdit::HitTest(POINT pt)
{
	RECT  rcClient;
	this->TxGetClientRect(&rcClient);

	if (::PtInRect(&rcClient, pt))
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE WindowlessRichEdit::QueryInterface(REFIID riid,void **ppvObject)
{
	if (NULL == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown,riid))
	{
		*ppvObject = static_cast<IUnknown*>(this);		
	}
	else if (::IsEqualIID(IID_ITextHost,riid))
	{
		*ppvObject = static_cast<ITextHost*>(this);
	}
	else
	{
		return E_NOINTERFACE;
	}
	return S_OK;
}
ULONG STDMETHODCALLTYPE WindowlessRichEdit::AddRef(void)
{
	return 1;
}
ULONG STDMETHODCALLTYPE WindowlessRichEdit::Release(void)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

ITextHostImpl::ITextHostImpl()
{
	m_nxPerInch = m_nyPerInch = 0;
	m_hParentWnd = NULL;
	m_dwStyle = ES_MULTILINE|ES_NOHIDESEL;
}


//@cmember Get the DC for the host
HDC ITextHostImpl::TxGetDC() 
{
	return ::GetDC(m_hParentWnd);
}

//@cmember Release the DC gotten from the host
INT ITextHostImpl::	TxReleaseDC(HDC hdc)
{
	return ::ReleaseDC (m_hParentWnd, hdc);
}

//@cmember Show the scroll bar
BOOL ITextHostImpl::TxShowScrollBar(INT fnBar, BOOL fShow) 
{
	return FALSE;
}

//@cmember Enable the scroll bar
BOOL ITextHostImpl::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
	return FALSE;
}

//@cmember Set the scroll range
BOOL ITextHostImpl::TxSetScrollRange(
				 INT fnBar,
				 LONG nMinPos,
				 INT nMaxPos,
				 BOOL fRedraw)
{
	return FALSE;
}

//@cmember Set the scroll position
BOOL ITextHostImpl::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw) 
{
	return FALSE;
}

//@cmember InvalidateRect
void ITextHostImpl::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	::InvalidateRect(m_hParentWnd, prc, fMode);
	::PostMessage(m_hParentWnd, 1111,0,0);
}

//@cmember Send a WM_PAINT to the window
void ITextHostImpl::TxViewChange(BOOL fUpdate)
{
	::UpdateWindow (m_hParentWnd);
}

//@cmember Create the caret
BOOL ITextHostImpl::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	//return ::CreateCaret (m_hParentWnd, hbmp, xWidth, yHeight);
	m_caret.CreateCaret(m_hParentWnd, hbmp, xWidth, yHeight);
	return TRUE;
}

//@cmember Show the caret
BOOL ITextHostImpl::TxShowCaret(BOOL fShow) 
{
// 	if(fShow)
// 		return ::ShowCaret(m_hParentWnd);
// 	else
// 		return ::HideCaret(m_hParentWnd);

	if (fShow)
		m_caret.ShowCaret();
	else
		m_caret.HideCaret();

	return TRUE;
}

//@cmember Set the caret position
BOOL ITextHostImpl::TxSetCaretPos(INT x, INT y)
{
//	return ::SetCaretPos(x, y);

	m_caret.SetCaretPos(x, y);

	return TRUE;
}

//@cmember Create a timer with the specified timeout
BOOL ITextHostImpl::TxSetTimer(UINT idTimer, UINT uTimeout)
{
//TODO:	fTimer = TRUE;
	return ::SetTimer(m_hParentWnd, idTimer, uTimeout, NULL);
}

//@cmember Destroy a timer
void ITextHostImpl::TxKillTimer(UINT idTimer)
{
	::KillTimer(m_hParentWnd, idTimer);
//TODO:	fTimer = FALSE;
}

//@cmember Scroll the content of the specified window's client area
void ITextHostImpl::TxScrollWindowEx (
	INT dx,
	INT dy,
	LPCRECT lprcScroll,
	LPCRECT lprcClip,
	HRGN hrgnUpdate,
	LPRECT lprcUpdate,
	UINT fuScroll)
{
	::ScrollWindowEx(m_hParentWnd, dx, dy, lprcScroll, lprcClip, hrgnUpdate, lprcUpdate, fuScroll);
}

//@cmember Get mouse capture
void ITextHostImpl::TxSetCapture(BOOL fCapture)
{
	if (fCapture)
		::SetCapture(m_hParentWnd);
	else
		::ReleaseCapture();
}

//@cmember Set the focus to the text window
void ITextHostImpl::TxSetFocus() 
{
	::SetFocus(m_hParentWnd);
}

//@cmember Establish a new cursor shape
void ITextHostImpl::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	if (fText)
	{
		::SetCursor(hcur);
	}
	else
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
}

//@cmember Converts screen coordinates of a specified point to the client coordinates
BOOL ITextHostImpl::TxScreenToClient (LPPOINT lppt)
{
	return ::ScreenToClient(m_hParentWnd, lppt);	
}

//@cmember Converts the client coordinates of a specified point to screen coordinates
BOOL ITextHostImpl::TxClientToScreen (LPPOINT lppt) 
{
	return ::ClientToScreen(m_hParentWnd, lppt);
}

//@cmember Request host to activate text services
HRESULT ITextHostImpl::TxActivate( LONG * plOldState )
{
	return S_OK;
}

//@cmember Request host to deactivate text services
HRESULT ITextHostImpl::TxDeactivate( LONG lNewState )
{
	return S_OK;
}

// 由派生类实现
//@cmember Retrieves the coordinates of a window's client area
// HRESULT ITextHostImpl::TxGetClientRect(LPRECT prc)
// {
// 	*prc = m_rectInWindow;
// 	return S_OK;
// }

//@cmember Get the view rectangle relative to the inset
HRESULT ITextHostImpl::TxGetViewInset(LPRECT prc)
{
	SetRectEmpty(prc);
	return S_OK;
}

//@cmember Get the default character format for the text
HRESULT ITextHostImpl::TxGetCharFormat(const CHARFORMATW **ppCF )
{
	// TODO:
	return S_OK;
}

//@cmember Get the default paragraph format for the text
HRESULT ITextHostImpl::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	// TODO:
	return S_OK;
}

//@cmember Get the background color for the window
COLORREF ITextHostImpl::TxGetSysColor(int nIndex) 
{
	return ::GetSysColor(nIndex);
}

//@cmember Get the background (either opaque or transparent)
// 注：设置TXTBACK_TRANSPARENT后，richedit将变成背景透明
//     不设置或设置其它值后，将会有白色背景
HRESULT ITextHostImpl::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	*pstyle = /*TXTBACK_OPAQUE*/TXTBACK_TRANSPARENT;
	return S_OK;
}

//@cmember Get the maximum length for the text
HRESULT ITextHostImpl::TxGetMaxLength(DWORD *plength)
{
	// TODO:
	return S_OK;
}

//@cmember Get the bits representing requested scroll bars for the window
HRESULT ITextHostImpl::TxGetScrollBars(DWORD *pdwScrollBar)
{
	// TODO:
	return S_OK;
}

//@cmember Get the character to display for password input
HRESULT ITextHostImpl::TxGetPasswordChar(TCHAR *pch)
{
	return S_OK;
}

//@cmember Get the accelerator character
HRESULT ITextHostImpl::TxGetAcceleratorPos(LONG *pcp) 
{
	// TODO:
	return S_OK;
}

//@cmember Get the native size
HRESULT ITextHostImpl::TxGetExtent(LPSIZEL lpExtent)
{
	CRect  rcClient;
	this->TxGetClientRect(&rcClient);

	HDC hDC = GetDC(m_hParentWnd);
	int xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
	int yPerInch =	GetDeviceCaps(hDC, LOGPIXELSY); 
	::ReleaseDC(m_hParentWnd, hDC);

	// The extent matches the full client rectangle in HIMETRIC
	lpExtent->cx = DXtoHimetricX(rcClient.Width(), xPerInch);
	lpExtent->cy = DYtoHimetricY(rcClient.Height(), yPerInch);

	// TODO:
	return S_OK;
}

//@cmember Notify host that default character format has changed
HRESULT ITextHostImpl::OnTxCharFormatChange (const CHARFORMATW * pcf)
{
	// TODO:
	return S_OK;
}

//@cmember Notify host that default paragraph format has changed
HRESULT ITextHostImpl::OnTxParaFormatChange (const PARAFORMAT * ppf)
{
	// TODO:
	return S_OK;
}

//@cmember Bulk access to bit properties
HRESULT ITextHostImpl::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
	DWORD dwProperties = 0;

// 	if (fRich)
// 	{
// 		dwProperties = TXTBIT_RICHTEXT;
// 	}

	if (m_dwStyle & ES_MULTILINE)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (m_dwStyle & ES_READONLY)
	{
		dwProperties |= TXTBIT_READONLY;
	}


	if (m_dwStyle & ES_PASSWORD)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (!(m_dwStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

// 	if (fEnableAutoWordSel)
// 	{
// 		dwProperties |= TXTBIT_AUTOWORDSEL;
// 	}
// 
// 	if (fVertical)
// 	{
// 		dwProperties |= TXTBIT_VERTICAL;
// 	}
// 
// 	if (fWordWrap)
// 	{
// 		dwProperties |= TXTBIT_WORDWRAP;
// 	}
// 
// 	if (fAllowBeep)
// 	{
// 		dwProperties |= TXTBIT_ALLOWBEEP;
// 	}
// 
// 	if (fSaveSelection)
// 	{
// 		dwProperties |= TXTBIT_SAVESELECTION;
// 	}

	*pdwBits = dwProperties & dwMask; 
	return NOERROR;
	return S_OK;
}

//@cmember Notify host of events
HRESULT ITextHostImpl::TxNotify(DWORD iNotify, void *pv)
{
	return S_OK;
}

// Far East Methods for getting the Input Context
//#ifdef WIN95_IME
HIMC ITextHostImpl::TxImmGetContext()
{
	return NULL;
}
void ITextHostImpl::TxImmReleaseContext( HIMC himc )
{

}
//#endif

//@cmember Returns HIMETRIC size of the control bar.
// 注：如果不设置该变量，会导致cursor显示异常，显示成为一个行选择样式（箭头朝向右上角）
//	   像VS的一个功能
//
// 从google上找到的有关该设置的信息：
// Re: What ES_SELECTIONBAR option Does for a RichEdit 
//		ES_SELECTIONBAR is a new rich edit control style. it is really confusing since 
//		it has been defined with the same value as WS_MAXIMIZE, therefore it cannot be 
//		used as style passed to Create.
//
//		ES_SELECTIONBAR is equivalent to ECO_SELECTIONBAR that (I think) should replace 
//		confusing ES_SELECTIONBAR. 
//
//		ECO_SELECTIONBAR can be used as parameter when SetOption member is called or 
//		EM_SETOPTIONS message is sent.
//
//		When ECO_SELECTIONBAR for reach edit control is set, there is small left margin 
//		(wider than default) where cursor changes to right-up arrow allowing full line(s)
//		selection. 

HRESULT ITextHostImpl::TxGetSelectionBarWidth (LONG *lSelBarWidth)
{
	*lSelBarWidth = 0;
	return S_OK;
}

LRESULT WindowlessRichEdit::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0;
	HRESULT hr = m_spTextServices->TxSendMessage(uMsg, wParam, lParam, &lr);

// 	if (hr == S_FALSE)
// 	{
// 		lr = ::DefWindowProc(hWnd, Msg, wParam, lParam);
// 	}
	return lr;
}

// @cmember Retrieves the coordinates of a window's client area
HRESULT WindowlessRichEdit::TxGetClientRect(LPRECT prc)
{
	m_pRichEditBase->GetWindowRect((CRect*)prc);
	return S_OK;
}

