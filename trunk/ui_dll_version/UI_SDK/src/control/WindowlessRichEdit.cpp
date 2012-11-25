#include "StdAfx.h"
#include "control\WindowlessRichEdit.h"
#include "control\gifoleobject.h"

// 备注：在CreateTextSerives之后，调用QueryInterface获取ITextServices接口时，总是返回E_NOINTERFACE
//       问题原来是从riched20.lib联系在一起的IID_ITextServices，是错误的。
const IID IID_ITextServices_Fix = { 0x8d33f740, 0xcf58, 0x11ce, {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5} };

HMODULE WindowlessRichEdit::s_RichEditDll = NULL;
LONG    WindowlessRichEdit::s_refDll = 0;
UINT    WindowlessRichEdit::s_cfRichTextFormat = 0;
UINT    WindowlessRichEdit::s_cfRichTextAndObjects = 0;

WindowlessRichEdit::WindowlessRichEdit(RichEditBase* pRichEditBase)
{
	m_pRichEditBase = pRichEditBase;
	m_pOleMgr = new RichEditOleObjectManager(this);
	m_nLastTimerKickCount = 0;
	this->InitRichEidtDll();
}

WindowlessRichEdit::~WindowlessRichEdit(void)
{
	// 销毁所有的ole对象
	this->ClearOleObjects();

	m_pRichEditBase = NULL;

	// Revoke our drop target
	RevokeDragDrop();

	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxInPlaceDeactivate();
		m_spTextServices.Release();
		m_spTextServices = NULL;
	}

// 	for (int i = 0; i < (int)m_vecpUnkOleObject.size(); i++)
// 	{
// 		IUnknown* pUnk = m_vecpUnkOleObject[i];
// 		pUnk->Release();
// 	}
//	m_vecpUnkOleObject.clear();

	this->ReleaseRichEidtDll();
	SAFE_DELETE(m_pOleMgr);
}

//
// 有几个注意事项非常重要正确启用以这种功能的工作的： 
//
//  框架窗口将工具栏和状态栏的父项的需要创建具有 WS_CLIPCHILDREN 样式。如果不存在此样式应
//  用程序将在对象处于活动的 RichEdit 控件中时表现绘画的一些问题。 
//
//  应该用 WS_CLIPSIBLING 样式创建 RichEdit 控件本身。 此处太，如果不存在样式 RichEdit 控
//  件将出现绘画问题时对象处于活动的过程中创建子窗口。 
//
//  销毁 RichEdit 控制时，您的应用程序应停用任何处于活动对象和调用 IOleObject->Close() 
//  RichEdit 控件中的所有嵌入的对象上。如果这不进行某些对象应用程序可能不关闭，从而导致它
//  们保留在内存，即使 RichEdit 控件已被破坏。下面是演示如何处理结算的 OLE 对象的代码段：
//
void WindowlessRichEdit::ClearOleObjects()
{
	if (m_spOle)
	{
		HRESULT hr = 0;

		// 
		// Start by getting the total number of objects in the control.
		// 
		int objectCount = m_spOle->GetObjectCount();

		// 
		// Loop through each object in the control and if active
		// deactivate, and if open, close.
		// 
		for (int i = 0; i < objectCount; i++)
		{
			REOBJECT reObj;
			ZeroMemory(&reObj, sizeof(REOBJECT));
			reObj.cbStruct  = sizeof(REOBJECT);

			// 
			// Get the Nth object
			// 
			hr = m_spOle->GetObject(i,&reObj,REO_GETOBJ_POLEOBJ);
			if(SUCCEEDED(hr))
			{
				// 
				// If active, deactivate.
				// 
				if (reObj.dwFlags && REO_INPLACEACTIVE)
					m_spOle->InPlaceDeactivate();

				// 
				// If the object is open, close it.
				// 
				if(reObj.dwFlags&&REO_OPEN)
					hr = reObj.poleobj->Close(OLECLOSE_NOSAVE);

				reObj.poleobj->Release();  // release GetObject
			}
		}
	}
}

bool WindowlessRichEdit::Create(HWND hWndParent)
{
	if (NULL == m_pRichEditBase)
		return false;

	LRESULT lr = 0;

	HDC hdc = GetDC(hWndParent);
	m_nxPerInch = GetDeviceCaps(hdc, LOGPIXELSX); 
	m_nyPerInch = GetDeviceCaps(hdc, LOGPIXELSY); 
	ReleaseDC(hWndParent, hdc);

	if (!(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)))
	{
		m_fWordWrap = true;  // 注：初始化m_fWordWrap不能放在CreateTextService后面，否则没作用
	}

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
	UIASSERT(SUCCEEDED(hr));
// 	hr = m_spTextServices->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
// 	assert(SUCCEEDED(hr));

	// 避免英文字体与中文字体不统一的问题
	if (NULL != m_spTextServices)
	{
		m_spTextServices->TxSendMessage(EM_SETLANGOPTIONS, IMF_DUALFONT, 0, &lr);
	}

	if (!(m_dwStyle & ES_READONLY))
	{
		// This isn't a read only window so we need a drop target.
		RegisterDragDrop();
	}

	IRichEditOle* pOle = NULL;
	hr = m_spTextServices->TxSendMessage(EM_GETOLEINTERFACE,0, (LPARAM)&pOle, &lr);
	if (SUCCEEDED(hr) && NULL != pOle)
	{
		m_spOle = pOle;
	}

	// 注：这里千万不要直接传递this进去，否则会导致txservice调用texthost时，指针错误。
	hr = m_spTextServices->TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)static_cast<IRichEditOleCallback*>(this), &lr);
	
	return true;
}


LPCTSTR WindowlessRichEdit::GetLibraryName()
{
#if (_RICHEDIT_VER >= 0x0200)
	return _T("RICHED20.DLL");
#else
	return _T("RICHED32.DLL");
#endif
}

void WindowlessRichEdit::InitRichEidtDll()
{
	if (NULL == s_RichEditDll)
	{
		s_RichEditDll = ::LoadLibrary(GetLibraryName());
		s_cfRichTextFormat = ::RegisterClipboardFormat(_T("Rich Text Format"));
		s_cfRichTextAndObjects = ::RegisterClipboardFormat(_T("RichEdit Text and Objects"));
	}
	s_refDll++;
}
void WindowlessRichEdit::ReleaseRichEidtDll()
{
	s_refDll--;
	if (0 == s_refDll)
	{
		FreeLibrary(s_RichEditDll);
		s_RichEditDll = NULL;
	}
}

void WindowlessRichEdit::Draw(HDC hDC)
{
	if(NULL == m_spTextServices || NULL == hDC)  // Remark:如果传递NULL hdc给txdraw，居然会造成内存泄露...
		return;

// 	HBRUSH hBrush = ::CreateSolidBrush(RGB(240,240,240));
	RECT  rcClient;
	this->TxGetClientRect(&rcClient);
// 	::FillRect(hDC, &rcClient, hBrush);
// 	::DeleteObject(hBrush);

#if 0
	::OffsetRect(&rcClient, -rcClient.left, -rcClient.top);
//	::FillRect(hDC, &rcClient, (HBRUSH)::GetStockObject(BLACK_BRUSH));  // <- 经测试，该绘制第一次的偏移也是正确的！证明就是txdraw内部有问题，无视了偏移和剪裁

	m_spTextServices->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hDC,
		NULL, (RECTL *)&rcClient, NULL, NULL, NULL, NULL, TXTVIEW_ACTIVE);
#else
	// TODO: 这里为什么要这么做？？
	// 如果不这么做，第一次绘制时，TxDraw TNND居然会无视hDC的偏移，直接画在了0，0位置
 	SaveDC(hDC);
 	::SetViewportOrgEx(hDC, 0,0,NULL);
	m_spTextServices->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hDC,
							NULL, (RECTL *)&rcClient, NULL, NULL, NULL, NULL, TXTVIEW_ACTIVE);
	::RestoreDC(hDC,-1);
#endif
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

// 	if (hr == S_FALSE)
// 	{
// 		lr = ::DefWindowProc(hWnd, Msg, wParam, lParam);
// 	}
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

void WindowlessRichEdit::OnWindowPosChanged(LPWINDOWPOS)
{
	SetMsgHandled(FALSE);

	// 让windowless richedit重新设置光标的位置
	if (NULL != m_spTextServices && m_pRichEditBase->IsFocus())
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);  // 会强制显示光标，因此加了个isfocus判断
		// 该操作并不是立即去更新光标位置，而是会在下一个更新中调整，堆栈如下：
		// UIDLL.dll!CCaret::SetCaretPos(int x=232, int y=91)  行282	C++
		// UIDLL.dll!ITextHostImpl::TxSetCaretPos(int x=232, int y=91)  行317	C++
		// riched20.dll!CTxtSelection::CreateCaret()  + 0xfe 字节	
		// riched20.dll!CTxtSelection::UpdateCaret()  + 0x12d7f 字节	
		// riched20.dll!CDisplay::Draw()  + 0x151 字节	
		// riched20.dll!CTxtEdit::TxDraw()  + 0xeb 字节	
		// UIDLL.dll!WindowlessRichEdit::Draw(HDC__ * hDC=0xd90112c6)  行103 + 0x42 字节	C++
		// UIDLL.dll!RichEditBase::OnPaint(UI::IRenderDC * hRDC=0x01f98bd0)  行48	C++
	}
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


LRESULT WindowlessRichEdit::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
// 	IGifOleObject* pGifOleObject = NULL;
// 	m_vecpUnkOleObject[0]->QueryInterface(IID_IGifOleObject, (void**)&pGifOleObject);
// 	if (NULL != pGifOleObject)
// 	{
// 		pGifOleObject->Refresh();
// 	}
// 	return 0;
#endif

	LRESULT lr = 0;

	HRESULT hr = m_spTextServices->TxSendMessage(uMsg, wParam, lParam, &lr);

	// 	if (hr == S_FALSE)
	// 	{
	// 		lr = ::DefWindowProc(hWnd, Msg, wParam, lParam);
	// 	}
	return lr;
}

bool WindowlessRichEdit::DoGifOleUpdateRequst()
{
	int nNow = GetTickCount();
	int nDiff = nNow - m_nLastTimerKickCount;
	if (nDiff < 40)
	{
		return false;
	}
	m_nLastTimerKickCount = nNow;
	return true;
}
LRESULT WindowlessRichEdit::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	if (NULL != m_pRichEditBase && 0 == wParam)
	{
		if (false == DoGifOleUpdateRequst()) // 该函数主要用于限制动画刷新帧数
			return 0;
		
		m_pRichEditBase->UpdateObject();
	}
	return 0;
}


//@cmember Get the view rectangle relative to the inset
// 其实就是文字与边框的padding
// 由于UI控件的padding属性是绘制
HRESULT WindowlessRichEdit::TxGetViewInset(LPRECT prc)
{
	SetRectEmpty(prc);

#if 0 // TODO: delete this
	CRegion4 rPadding;
	m_pRichEditBase->GetPaddingRegion(&rPadding);
 	::CopyRect(prc, &rPadding);

	// Convert the inset to himetric that must be returned to ITextServices

	prc->left = DXtoHimetricX(prc->left, m_nxPerInch);
	prc->top  = DYtoHimetricY(prc->top, m_nyPerInch);
	prc->right  = DXtoHimetricX(prc->right, m_nxPerInch);
	prc->bottom = DYtoHimetricY(prc->bottom, m_nyPerInch);
#endif

	return S_OK;
}


// @cmember Retrieves the coordinates of a window's client area
HRESULT WindowlessRichEdit::TxGetClientRect(LPRECT prc)
{
	m_pRichEditBase->GetClientRectInWindow((CRect*)prc);
	
#if 0 // TODO: delete 
	// 减去padding的大小，因为padding的已经在richedit中作为inset view rect来实现
	CRegion4 rPadding;
	m_pRichEditBase->GetPaddingRegion(&rPadding);
	prc->left -= rPadding.left;
	prc->top  -= rPadding.top;
	prc->right  += rPadding.right;
	prc->bottom += rPadding.bottom;
#endif

	return S_OK;
}

//@cmember InvalidateRect
void /*ITextHostImpl*/WindowlessRichEdit::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	if (NULL != m_pRichEditBase)
	{
		m_caret.HideCaret();  // -- 为了解决在输入字符或者backspace删除字符时，上一个光标会残留在屏幕上的问题
		m_pRichEditBase->UpdateObject();
		m_caret.ShowCaret();
	}

//	::InvalidateRect(m_hParentWnd, prc, fMode);
}

//@cmember Get the background (either opaque or transparent)
// 注：设置TXTBACK_TRANSPARENT后，richedit将变成背景透明
//     不设置或设置其它值后，将会有白色背景
HRESULT /*ITextHostImpl*/WindowlessRichEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	// 默认使用RichEdit backrender的背景，便于统一扩展
// 	if (m_pRichEditBase->IsTransparent())
 		*pstyle = TXTBACK_TRANSPARENT;
// 	else
// 		*pstyle = TXTBACK_OPAQUE;

	return S_OK;
}



//@cmember Show the scroll bar
BOOL WindowlessRichEdit::TxShowScrollBar(INT fnBar, BOOL fShow) 
{
	UI_LOG_DEBUG(_T("%s fnBar=%d, fShow=%d"),FUNC_NAME, fnBar, fShow);
	if (FALSE == fShow)
	{
		switch (fnBar)
		{
		case SB_HORZ:
			m_pRichEditBase->GetScrollMgr().GetHScrollBar()->SetScrollRange(0);
			break;

		case SB_VERT:
			m_pRichEditBase->GetScrollMgr().GetVScrollBar()->SetScrollRange(0);
			break;
		}
	}
	return FALSE;
}

//@cmember Enable the scroll bar
BOOL WindowlessRichEdit::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
	UI_LOG_DEBUG(FUNC_NAME);
	return FALSE;
}

//@cmember Set the scroll range
BOOL WindowlessRichEdit::TxSetScrollRange(
									 INT fnBar,
									 LONG nMinPos,
									 INT nMaxPos, 
									 BOOL fRedraw)  // -- fRedraw：由RichEditBase::OnScroll来负责刷新，因为RichEditBase::OnScroll中还有一些处理要做
{
//	UI_LOG_DEBUG(_T("%s nMinPos=%d, nMaxPos=%d, fRedraw=%d"), FUNC_NAME, nMinPos, nMaxPos, fRedraw);

	if (SB_HORZ == fnBar)
	{
		m_pRichEditBase->GetScrollMgr().GetHScrollBar()->SetScrollRange(nMaxPos - nMinPos);
	}
	else if (SB_VERT == fnBar)
	{
		m_pRichEditBase->GetScrollMgr().SetVScrollRange(nMaxPos - nMinPos);
	}


	return TRUE;
}

//@cmember Set the scroll position
BOOL WindowlessRichEdit::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)    // -- fRedraw：由RichEditBase::OnScroll来负责刷新，因为RichEditBase::OnScroll中还有一些处理要做
{
//	UI_LOG_DEBUG(_T("%s, nPos=%d, fRedraw=%d"), FUNC_NAME, nPos, fRedraw);
	if (SB_HORZ == fnBar)
	{
		m_pRichEditBase->GetScrollMgr().GetHScrollBar()->SetScrollPos(nPos);
	}
	else if (SB_VERT == fnBar)
	{
		m_pRichEditBase->GetScrollMgr().GetVScrollBar()->SetScrollPos(nPos);
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE WindowlessRichEdit::QueryInterface(REFIID riid,void **ppvObject)
{
	if (NULL == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown,riid) || ::IsEqualIID(IID_ITextHost,riid))
	{
		*ppvObject = static_cast<ITextHost*>(this);
	}
	else if (::IsEqualIID(riid, IID_IRichEditOleCallback))
	{
		*ppvObject = static_cast<IRichEditOleCallback*>(this);
	}
	else
		return E_NOINTERFACE;
	
	this->AddRef();
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
//
//               ITempHostImpl 
//
//////////////////////////////////////////////////////////////////////////

//@cmember Get the DC for the host
HDC ITextHostImpl::TxGetDC() 
{
	return ::GetDC(m_hParentWnd);
}

//@cmember Release the DC gotten from the host
INT ITextHostImpl::TxReleaseDC(HDC hdc)
{
	return ::ReleaseDC (m_hParentWnd, hdc);
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
// 该接口并不仅仅设置text的ibean光标，还包括sel bar的指针
// If the fText parameter is TRUE, the text services object 
// is trying to set the text cursor (the cursor appears as 
// an I-beam when it is over text that is not selected). In 
// this case, the host can set it to whatever the control 
// MousePointer property is. 
void ITextHostImpl::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	::SetCursor(hcur);
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

//@cmember Get the default character format for the text
HRESULT ITextHostImpl::TxGetCharFormat(const CHARFORMATW **ppCF )
{
	*ppCF = &m_cf;
	return S_OK;
}

//@cmember Get the default paragraph format for the text
HRESULT ITextHostImpl::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	*ppPF = &m_pf;
	return S_OK;
}

//@cmember Get the background color for the window
COLORREF ITextHostImpl::TxGetSysColor(int nIndex) 
{
	return ::GetSysColor(nIndex);
}

//@cmember Get the maximum length for the text
HRESULT ITextHostImpl::TxGetMaxLength(DWORD *plength)
{
	*plength = m_dwMaxLength;
	return S_OK;
}

//@cmember Get the bits representing requested scroll bars for the window
HRESULT ITextHostImpl::TxGetScrollBars(DWORD *pdwScrollBar)
{
	*pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL
		 | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

	return S_OK;
}

//@cmember Get the character to display for password input
HRESULT ITextHostImpl::TxGetPasswordChar(TCHAR *pch)
{
#ifdef UNICODE
	*pch = m_chPasswordChar;
#else
	WideCharToMultiByte(CP_ACP, 0, &m_chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
	return NOERROR;
}

//@cmember Get the accelerator character
HRESULT ITextHostImpl::TxGetAcceleratorPos(LONG *pcp) 
{
	*pcp = m_laccelpos;
	return S_OK;
}

//@cmember Get the native size
HRESULT ITextHostImpl::TxGetExtent(LPSIZEL lpExtent)
{
// 	CRect  rcClient;
// 	this->TxGetClientRect(&rcClient);
// 
// 	HDC hDC = GetDC(m_hParentWnd);
// 	int xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
// 	int yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
// 	::ReleaseDC(m_hParentWnd, hDC);
// 
// 	// The extent matches the full client rectangle in HIMETRIC
// 	lpExtent->cx = DXtoHimetricX(rcClient.Width(), xPerInch);
// 	lpExtent->cy = DYtoHimetricY(rcClient.Height(), yPerInch);

	return E_NOTIMPL;
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

	if (m_fRich)
	{
		dwProperties = TXTBIT_RICHTEXT;
	}

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
 
	if (m_fWordWrap)
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}
 
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
	*lSelBarWidth = m_lSelBarWidth;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//               ITempHostImpl 其它设置方法
//
//////////////////////////////////////////////////////////////////////////

ITextHostImpl::ITextHostImpl()
{
	m_nxPerInch = m_nyPerInch = 96;
	m_hParentWnd = NULL;
	m_dwStyle = ES_MULTILINE|ES_NOHIDESEL|WS_VSCROLL | /*WS_HSCROLL |*/ ES_AUTOVSCROLL /*| ES_AUTOHSCROLL | ES_DISABLENOSCROLL*/;
	m_dwMaxLength = INFINITE;
	m_lSelBarWidth = 0;

	m_chPasswordChar = L'*';
	m_laccelpos = -1;
	m_fWordWrap = false;
	m_fRich = true;
	m_fRegisteredForDrop = false;
	m_fPassword = false;

	memset(&m_cf, 0, sizeof(CHARFORMAT2W));
	InitDefaultParaFormat();

	if(!(m_dwStyle & ES_LEFT))
	{
		if(m_dwStyle & ES_CENTER)
			m_pf.wAlignment = PFA_CENTER;
		else if(m_dwStyle & ES_RIGHT)
			m_pf.wAlignment = PFA_RIGHT;
	}
}

bool ITextHostImpl::SetFont(LOGFONT* plf)
{
	if (NULL == plf)
		return false;

	// Set CHARFORMAT structure
	m_cf.cbSize = sizeof(CHARFORMAT2W);

	HWND hWnd = GetDesktopWindow();
	HDC  hDC = GetDC(hWnd);
	LONG yPixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
	m_cf.yHeight = abs(plf->lfHeight * 1440 / yPixPerInch);
	ReleaseDC(hWnd, hDC);

	m_cf.yOffset = 0;
//	m_cf.crTextColor = RGB(0,0,0);  // 不涉及颜色的修改

	m_cf.dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;
	m_cf.dwEffects &= ~(CFE_PROTECTED | CFE_LINK);

	if(plf->lfWeight < FW_BOLD)
		m_cf.dwEffects &= ~CFE_BOLD;
	if(!plf->lfItalic)
		m_cf.dwEffects &= ~CFE_ITALIC;
	if(!plf->lfUnderline)
		m_cf.dwEffects &= ~CFE_UNDERLINE;
	if(!plf->lfStrikeOut)
		m_cf.dwEffects &= ~CFE_STRIKEOUT;

	m_cf.dwMask = CFM_ALL | CFM_BACKCOLOR;
	m_cf.bCharSet = plf->lfCharSet;
	m_cf.bPitchAndFamily = plf->lfPitchAndFamily;
#ifdef UNICODE
	_tcscpy(m_cf.szFaceName, plf->lfFaceName);
#else
	//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
	MultiByteToWideChar(CP_ACP, 0, plf->lfFaceName, LF_FACESIZE, m_cf.szFaceName, LF_FACESIZE) ;
#endif

	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
					TXTBIT_CHARFORMATCHANGE);
	}

	return true;
}

void ITextHostImpl::InitDefaultParaFormat() 
{	
	memset(&m_pf, 0, sizeof(PARAFORMAT));

	m_pf.cbSize = sizeof(PARAFORMAT);
	m_pf.dwMask = PFM_ALL;
	m_pf.wAlignment = PFA_LEFT;
	m_pf.cTabCount = 1;
	m_pf.rgxTabs[0] = lDefaultTab;
}

WCHAR ITextHostImpl::SetPasswordChar(WCHAR ch_PasswordChar)
{
	WCHAR chOldPasswordChar = m_chPasswordChar;

	m_chPasswordChar = ch_PasswordChar;

	// notify text services of property change
	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
			(m_chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);
	}

	return chOldPasswordChar;
}

LONG ITextHostImpl::SetAccelPos(LONG l_accelpos)
{
	LONG laccelposOld = l_accelpos;

	m_laccelpos = l_accelpos;

	// notify text services of property change
	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);
	}

	return laccelposOld;
}

void ITextHostImpl::SetWordWrap(bool fWordWrap)
{
	m_fWordWrap = fWordWrap;
	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
	}
}


void ITextHostImpl::SetReadOnly(bool fReadOnly)
{
	DWORD dwUpdatedBits = 0;

	if (fReadOnly)
	{
		m_dwStyle |= ES_READONLY;

		// Turn off Drag Drop 
		RevokeDragDrop();
		dwUpdatedBits |= TXTBIT_READONLY;
	}
	else
	{
		m_dwStyle &= ~ES_READONLY;

		// Turn drag drop back on
		RegisterDragDrop();	
	}

	// Notify control of property change
	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_READONLY, dwUpdatedBits);
	}
}

void ITextHostImpl::SetMaxLegnth(DWORD dw)
{
	m_dwMaxLength = dw;
	
	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, 
			TXTBIT_MAXLENGTHCHANGE);
	}
}

LONG ITextHostImpl::GetSelBarWidth()
{
	return Util::HimetricXtoDX(m_lSelBarWidth, m_nxPerInch);
}

LONG ITextHostImpl::SetSelBarWidth(LONG l_SelBarWidth)
{
	LONG lOldSelBarWidth = m_lSelBarWidth;

	m_lSelBarWidth = Util::DXtoHimetricX(l_SelBarWidth, m_nxPerInch);

	if (m_lSelBarWidth)
	{
		m_dwStyle |= ES_SELECTIONBAR;
	}
	else
	{
		m_dwStyle &= (~ES_SELECTIONBAR);
	}

	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);
	}
	return lOldSelBarWidth;
} 

void ITextHostImpl::SetRichTextFlag(bool b)
{
	m_fRich = b;

	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
			b ? TXTBIT_RICHTEXT : 0);
	}
}


void ITextHostImpl::RevokeDragDrop(void)
{
	if (m_fRegisteredForDrop)
	{
		::RevokeDragDrop(m_hParentWnd);
		m_fRegisteredForDrop = FALSE;
	}
}

//
// 备注：
// Applications that use the following functionality must call OleInitialize 
// before calling any other function in the COM library: 
//		Clipboard
//		Drag and drop
//		Object linking and embedding (OLE)
//		In-place activation
// 用oleInitialize初始化COM库，而不是使用CoInitialize
//
void ITextHostImpl::RegisterDragDrop(void)
{
	IDropTarget *pdt = NULL;

	if(!m_fRegisteredForDrop && m_spTextServices->TxGetDropTarget(&pdt) == NOERROR)
	{
		HRESULT hr = ::RegisterDragDrop(m_hParentWnd, pdt);

		if(hr == NOERROR || hr == 0x80040101)
		{	
			m_fRegisteredForDrop = true;
		}
		else
		{
			UIASSERT(0);
		}

		pdt->Release();
	}
}

void ITextHostImpl::SetPasswordMode(bool b)
{
	m_fPassword = b;

	if (NULL != m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
			b? TXTBIT_USEPASSWORD:0);
	}
}

bool ITextHostImpl::SetText(const TCHAR* szText)
{
	if (NULL == m_spTextServices)
		return false;

	// For RichEdit 1.0, the max text length would be reset by a settext so 
	// we follow pattern here as well.

	LRESULT lr = 0;
	HRESULT hr = m_spTextServices->TxSendMessage(WM_SETTEXT, 0, (LPARAM)szText, &lr);

	if (SUCCEEDED(hr))
	{
		// Update succeeded.
		DWORD cNewText = _tcslen(szText);

		// If the new text is greater than the max set the max to the new
		// text length.
		if (cNewText > m_dwMaxLength)
		{
			m_dwMaxLength = cNewText;                
		}
		return true;
	}

	return false; 
}

//
//	如果InsertObject时不去提供/创建storage对象,那么复制对象后粘贴操作将失败!
//
bool WindowlessRichEdit::InsertOleObject(IRichEditOleObjectItem* pItem)
{
	if (NULL == pItem)
		return false;

	bool       bRet = false;
	HRESULT    hr = E_FAIL;
	IOleObject*     pOleObject = NULL;
	IStorage*       pStorage = NULL;
	ILockBytes*     pLockbytes = NULL;
	LPOLECLIENTSITE pClientSite = NULL;

	do 
	{
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockbytes);
		if (sc != S_OK)
			break;

		sc = ::StgCreateDocfileOnILockBytes(pLockbytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
		if (sc != S_OK)
			break;

		hr = m_spOle->GetClientSite(&pClientSite);
		if (FAILED(hr))
			break;

		pItem->GetOleObject(&pOleObject);
		if (NULL == pOleObject)
			break;

		REOBJECT reObj;
		ZeroMemory(&reObj, sizeof(REOBJECT));
		reObj.cbStruct = sizeof(REOBJECT);
		reObj.poleobj = pOleObject;
		reObj.polesite = pClientSite;
		reObj.dvaspect = DVASPECT_CONTENT;
		reObj.dwFlags = REO_BELOWBASELINE;
		reObj.pstg = pStorage;
		reObj.dwUser = 0;
		reObj.cp = REO_CP_SELECTION;

		SIZEL  sizel = {0,0};
		reObj.sizel.cx = 0;
		reObj.sizel.cy = 0;

		// The rich edit control automatically increments the reference count for the interfaces 
		// if it holds onto them, so the caller can safely release the interfaces if they are not 
		// needed. 
		HRESULT hr = m_spOle->InsertObject(&reObj);
		if (FAILED(hr))
			break;

		OleSetContainedObject(static_cast<IOleObject*>(pOleObject), TRUE);

		this->m_pOleMgr->AddOleItem(pItem);
		bRet = true;
	} while (0);
	
	
	SAFE_RELEASE(pClientSite);
	SAFE_RELEASE(pOleObject);
	SAFE_RELEASE(pStorage);
	SAFE_RELEASE(pLockbytes);
	
	return bRet;
}
bool WindowlessRichEdit::InsertGif(const TCHAR* szGifPath)
{
	GifOleObject* pGifOle = new GifOleObject(m_pOleMgr->GetGifManager(), static_cast<Message*>(this->m_pRichEditBase));
	HRESULT hr = pGifOle->LoadGif(szGifPath);
	if (FAILED(hr))
	{
		SAFE_DELETE(pGifOle);
		return false;
	}

	return this->InsertOleObject(pGifOle);
}

bool WindowlessRichEdit::InsertComObject(CLSID clsid)
{
	RichEditOleObjectItem_Com* pItem = new RichEditOleObjectItem_Com;
	pItem->Attach(clsid);

	bool bRet = this->InsertOleObject(pItem);

	IOleObject* p = NULL;
	pItem->GetOleObject(&p, false);
	p->Release();
	
	return bRet;
}

void WindowlessRichEdit::DoPaste(LPDATAOBJECT pDataObject, CLIPFORMAT cf, HMETAFILEPICT hMetaPict)
{

	LPOLEOBJECT     lpOleObject = NULL;
	IStorage*       pStorage = NULL;
	ILockBytes*     pLockbytes = NULL;
	LPOLECLIENTSITE pClientSite = NULL;

	bool  bRet = false;
	do 
	{
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockbytes);
		if (sc != S_OK)
			break;

		sc = ::StgCreateDocfileOnILockBytes(pLockbytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
		if (sc != S_OK)
			break;

		HRESULT hr = m_spOle->GetClientSite(&pClientSite);
		if (FAILED(hr))
			break;

// 		sc = ::OleCreateFromData(pDataObject, IID_IUnknown, OLERENDER_DRAW, NULL, pClientSite, pStorage, (void**)&lpOleObject);
// 		if (FAILED(sc))
			sc = ::OleCreateStaticFromData(pDataObject, IID_IUnknown, OLERENDER_DRAW, NULL, pClientSite, pStorage, (void**)&lpOleObject);

		if (sc != S_OK)
			break;

		// all items are "contained" -- this makes our reference to this object
		//  weak -- which is needed for links to embedding silent update.
		OleSetContainedObject(lpOleObject, TRUE);


		REOBJECT reObj;
		memset(&reObj, 0, sizeof(reObj));
		reObj.cbStruct = sizeof(REOBJECT);
		reObj.cp = REO_CP_SELECTION;

		reObj.poleobj = lpOleObject;
		reObj.polesite = pClientSite;
		reObj.dvaspect = DVASPECT_CONTENT;
		reObj.dwFlags = REO_BELOWBASELINE;
		reObj.pstg = pStorage;
		reObj.dwUser = 0;
		reObj.cp = REO_CP_SELECTION;

		hr = m_spOle->InsertObject(&reObj);
		if (FAILED(hr))
			break;

		bRet = true;
	} while (0);

	SAFE_RELEASE(pLockbytes);
	SAFE_RELEASE(pStorage);
	SAFE_RELEASE(pClientSite);
	if (!bRet)
	{
		SAFE_RELEASE(lpOleObject);
	}


}

IOleObject* WindowlessRichEdit::CreateOleObjectFromData(LPDATAOBJECT pDataObject, bool bOleCreateFromDataOrOleCreateStaticFromData, OLERENDER render, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtc)
{
	LPOLEOBJECT     lpOleObject = NULL;
	IStorage*       pStorage = NULL;
	ILockBytes*     pLockbytes = NULL;
	LPOLECLIENTSITE pClientSite = NULL;

	bool  bRet = false;
	do 
	{
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockbytes);
		if (sc != S_OK)
			break;

		sc = ::StgCreateDocfileOnILockBytes(pLockbytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
		if (sc != S_OK)
			break;

		HRESULT hr = m_spOle->GetClientSite(&pClientSite);
		if (FAILED(hr))
			break;

		if (bOleCreateFromDataOrOleCreateStaticFromData)
			sc = ::OleCreateFromData(pDataObject, IID_IUnknown, render, NULL, pClientSite, pStorage, (void**)&lpOleObject);
		else
			sc = ::OleCreateStaticFromData(pDataObject, IID_IUnknown, render, NULL, pClientSite, pStorage, (void**)&lpOleObject);

		if (sc != S_OK)
			break;

		// all items are "contained" -- this makes our reference to this object
		//  weak -- which is needed for links to embedding silent update.
		OleSetContainedObject(lpOleObject, TRUE);

		bRet = true;
	} while (0);

	SAFE_RELEASE(pLockbytes);
	SAFE_RELEASE(pStorage);
	SAFE_RELEASE(pClientSite);
	if (!bRet)
	{
		SAFE_RELEASE(lpOleObject);
	}

	return lpOleObject;
}

// int WindowlessRichEdit::GetObjectTypeByOleObject(LPOLEOBJECT*  pOleObject)
// {
// 	if (NULL == pOleObject)
// 		return OT_STATIC;
// 
// 	// check for linked object
// 	LPOLELINK lpOleLink = NULL;
// 	m_lpObject->QueryInterface(IID_IOleLink, (void**)&lpOleLink);
// 	if (lpOleLink != NULL)
// 	{
// 		lpOleLink->Release();
// 		return OT_LINK;
// 	}
// 
// 	// check for static object
// 	DWORD dwStatus;
// 	if (pOleObject->GetMiscStatus(DVASPECT_CONTENT, &dwStatus) == S_OK
// 		&& (dwStatus & OLEMISC_STATIC) == 0)
// 	{
// 		return OT_EMBEDDED;
// 	}
// 
// 	// not not link, not embedding -- must be static
// 	return OT_STATIC;
// }

bool WindowlessRichEdit::GetSelectionOleObject(IRichEditOleObjectItem** ppItem)
{
	if (NULL == ppItem)
		return false;

	REOBJECT reObj;
	ZeroMemory(&reObj, sizeof(REOBJECT));
	reObj.cbStruct = sizeof(REOBJECT);

	HRESULT hr = m_spOle->GetObject(REO_IOB_USE_CP, &reObj, REO_GETOBJ_NO_INTERFACES);
	if (SUCCEEDED(hr))
	{
		*ppItem = m_pOleMgr->GetOleItem(reObj.dwUser);
		return true;
	}

	return false;
}

#pragma region  // IRichEditOleCallback
// This method must be implemented to allow cut, copy, paste, drag, 
// and drop operations of Component Object Model (COM) objects.
// 例如向richedit中随便拖入一个桌面上的图标，就会调用该函数
HRESULT __stdcall WindowlessRichEdit::GetNewStorage(LPSTORAGE FAR * lplpstg)
{
	if (NULL == lplpstg)
	{
		return E_INVALIDARG;
	}
	LPLOCKBYTES lpLockBytes = NULL;
	SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (sc != S_OK)
	{
		return E_OUTOFMEMORY;
	}

	sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
		STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, lplpstg);
	if (sc != S_OK)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}
HRESULT __stdcall WindowlessRichEdit::GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame,
									LPOLEINPLACEUIWINDOW FAR * lplpDoc,
									LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return E_NOTIMPL;
}
HRESULT __stdcall WindowlessRichEdit::ShowContainerUI(BOOL fShow)
{
	return E_NOTIMPL;
}
// 在从外部拖入一个文件到richedit时，先响应了GetNewStorage成功后，就会再调到这个接口函数
// 当返回S_OK时，这个对象将被插入，返回FALSE时，对象将不会被插入
HRESULT __stdcall WindowlessRichEdit::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg,
									LONG cp)
{
	return S_OK;
}
// 例如将richedit中的一个COM对象删除，则会调用一次该接口函数
// 例如将richedit中的一个COM对象用鼠标拖拽到另一个位置，则会调用一次该接口函数
// 该函数仅是一个通知，告诉我们有一个对象要被deleted from rich edit control;
// 这里不用调用release lpoleobj?
HRESULT __stdcall WindowlessRichEdit::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}

//	CRichEditView::QueryAcceptData
// 在richedit中使用 CTRL+V、拖放时被调用
//
// 1. 如何粘贴HTML格式？
//    在MSDN中搜索 HTML Clipboard Format
//
HRESULT __stdcall WindowlessRichEdit::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	if (!fReally) // not actually pasting
		return S_OK;


	// if direct pasting a particular native format allow it
	if (*lpcfFormat == CF_TEXT ||
		*lpcfFormat == WindowlessRichEdit::s_cfRichTextFormat ||
		*lpcfFormat == WindowlessRichEdit::s_cfRichTextAndObjects
		)
		return S_OK;

	// if format is 0, then force particular formats if available
	if (*lpcfFormat == 0 /*&& (m_nPasteType == 0)*/)
	{
		if (IsClipboardFormatAvailable((CLIPFORMAT)WindowlessRichEdit::s_cfRichTextAndObjects)) // native avail, let richedit do as it wants
			return S_OK;
		else if (IsClipboardFormatAvailable((CLIPFORMAT)WindowlessRichEdit::s_cfRichTextFormat))
		{
			*lpcfFormat = (CLIPFORMAT)WindowlessRichEdit::s_cfRichTextFormat;
			return S_OK;
		}
		else if (IsClipboardFormatAvailable(CF_TEXT))
		{
			*lpcfFormat = CF_TEXT;
			return S_OK;
		}
	}

	// paste OLE formats
	// GetThisPtr()->DoPaste(lpdataobj, *lpcfFormat, hMetaPict);

	return S_FALSE;
}
HRESULT __stdcall WindowlessRichEdit::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}
// 在richedit中使用 CTRL+C/拖拽 时被调用，获取要复制的数据
// This method returns success status. If the SCODE of the return value is E_NOTIMPL, the 
// rich edit control creates its own clipboard object. 
// If the SCODE of the return value is a failure other than E_NOTIMPL, the operation fails.
//
// 之后richedit内部将会调用OleSetClipboard, SetClipboardDataObject (将IDataObject AddRef)
//
HRESULT __stdcall WindowlessRichEdit::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
	WORD wSelType = this->GetSelectionType();
	if (SEL_OBJECT == wSelType)
	{
		IRichEditOleObjectItem* pItem = NULL;
		bool bRet = this->GetSelectionOleObject(&pItem);
		if (!bRet || NULL == pItem)
			return S_OK  ;

		if (FAILED(pItem->GetClipboardData(lpchrg, reco, lplpdataobj)))
		{
			// 调用默认的方法。内部堆栈为：
			// 0. COM组件的：     ATL::IOleObjectImpl<XXX>::GetClipboardData <-- 默认ATL内部没有实现
			// 1. riched20.dll的: CDataTransferObj::Create()
			// 2. riched20.dll的: CLightDTEngine::RangeToDataObject()
			// 3. riched20.dll的: CTxtEdit::GetClipboardData()
			// 4. UIDLL.dll的:    这里m_spOle->GetClipboardData
			m_spOle->GetClipboardData(lpchrg,reco,lplpdataobj);
			return S_OK;
		}

		return S_OK;   // 注：如果SEL_OBJECT == wSelType时返回不是S_OK会造成内存pItem被AddRef，最后导致内存泄露.什么原理?/
		               // 貌似是因为为IDataObject放到clipboard中增加了一个引用计数。当再去复制一个其它内容时，引用计数就会正常

	}
	return E_NOTIMPL;
}

// 在richedit中使用鼠标拖拽时被调用
HRESULT __stdcall WindowlessRichEdit::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	if (!fDrag) // allowable dest effects
	{
		DWORD dwEffect;
		// check for force link
		if ((grfKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
			dwEffect = DROPEFFECT_LINK;
		// check for force copy
		else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
			dwEffect = DROPEFFECT_COPY;
		// check for force move
		else if ((grfKeyState & MK_ALT) == MK_ALT)
			dwEffect = DROPEFFECT_MOVE;
		// default -- recommended action is move
		else
			dwEffect = DROPEFFECT_MOVE;
		if (dwEffect & *pdwEffect) // make sure allowed type
			*pdwEffect = dwEffect;
	}
	return S_OK;
}

#include "E:\\编程\\workingpath\\test\\richeditole\\richeditole_i.h"

// 右击RichEdit时被调用，根据鼠标右键时，鼠标下面的对象的不同，得到的参数也不同，
// 例如在空白处右击，seltype=0, lpoleobj=NULL
// 例如在一个COM对象处右击，可能seltype=2, lpoleobj = xxx;
HRESULT __stdcall WindowlessRichEdit::GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu)
{
#ifdef _DEBUG
	//return this->InsertGif(_T("C:\\aaa.gif"));
	return this->InsertComObject(__uuidof(GifImageObject));
#endif
#ifdef _DEBUG
	HMENU& hMenu = *lphmenu;
	TCHAR szInfo[128] = _T("");
	_stprintf(szInfo, _T("GetContextMenu Args: seltype=%d, lpoleobj=%08x, lpchrg=%d,%d"),
		seltype, lpoleobj, lpchrg->cpMin, lpchrg->cpMax);

	hMenu = CreatePopupMenu();
	BOOL bRet = ::AppendMenu(hMenu, MF_STRING, 10001, szInfo);
#endif
	return S_OK;
}
#pragma endregion