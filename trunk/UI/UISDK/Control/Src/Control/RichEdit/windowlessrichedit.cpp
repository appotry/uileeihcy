#include "StdAfx.h"
#include "windowlessrichedit.h"
#include "gifoleobject.h"
#include "UISDK\Control\Src\Control\RichEdit\richedit.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Kernel\Inc\Util\ibuffer.h"
#include "3rd\markup\markup.h"

// ��ע����CreateTextSerives֮�󣬵���QueryInterface��ȡITextServices�ӿ�ʱ�����Ƿ���E_NOINTERFACE
//       ����ԭ���Ǵ�riched20.lib�е�IID_ITextServices���Ǵ���ġ�
const IID IID_ITextServices_Fix = { 0x8d33f740, 0xcf58, 0x11ce, {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5} };

HMODULE WindowlessRichEdit::s_RichEditDll = NULL;
LONG    WindowlessRichEdit::s_refDll = 0;
UINT    WindowlessRichEdit::s_cfRichTextFormat = 0;
UINT    WindowlessRichEdit::s_cfRichTextAndObjects = 0;
UINT    WindowlessRichEdit::s_cfRichEditOleFormat = 0;
UINT    WindowlessRichEdit::s_cfUnicodeRichEditOleFormat = 0;

WindowlessRichEdit::WindowlessRichEdit()
{
	m_pRichEdit = NULL;
	m_pOleMgr = new RichEditOleObjectManager(this);
	m_nLastTimerKickCount = 0;
	SetRectEmpty(&m_rcInvalidate);
	m_bNeedRedrawScrollbar = false;
    m_bDuringTxDraw = false;

    m_nAutoResizeMaxSize = 0;
    m_bFocus = false;
	this->InitRichEidtDll();
}

void WindowlessRichEdit::BindControl(RichEdit* p)
{
	m_pRichEdit = p;
	m_pOleMgr->SetUIApplication(m_pRichEdit->GetIRichEdit()->GetUIApplication());
}
WindowlessRichEdit::~WindowlessRichEdit(void)
{
	// �������е�ole����
	this->ClearOleObjects();

	m_pRichEdit = NULL;

	// Revoke our drop target
	RevokeDragDrop();

	if (m_spTextServices)
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
// �м���ע������ǳ���Ҫ��ȷ���������ֹ��ܵĹ����ģ� 
//
//  ��ܴ��ڽ���������״̬���ĸ������Ҫ�������� WS_CLIPCHILDREN ��ʽ����������ڴ���ʽӦ
//  �ó����ڶ����ڻ�� RichEdit �ؼ���ʱ���ֻ滭��һЩ���⡣ 
//
//  Ӧ���� WS_CLIPSIBLING ��ʽ���� RichEdit �ؼ����� �˴�̫�������������ʽ RichEdit ��
//  �������ֻ滭����ʱ�����ڻ�Ĺ����д����Ӵ��ڡ� 
//
//  ���� RichEdit ����ʱ������Ӧ�ó���Ӧͣ���κδ��ڻ����͵��� IOleObject->Close() 
//  RichEdit �ؼ��е�����Ƕ��Ķ����ϡ�����ⲻ����ĳЩ����Ӧ�ó�����ܲ��رգ��Ӷ�������
//  �Ǳ������ڴ棬��ʹ RichEdit �ؼ��ѱ��ƻ�����������ʾ��δ������� OLE ����Ĵ���Σ�
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
	if (NULL == m_pRichEdit)
		return false;

	LRESULT lr = 0;

	HDC hdc = GetDC(hWndParent);
	m_nxPerInch = GetDeviceCaps(hdc, LOGPIXELSX); 
	m_nyPerInch = GetDeviceCaps(hdc, LOGPIXELSY); 
	ReleaseDC(hWndParent, hdc);

	if (!(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)))
	{
		m_fWordWrap = true;  // ע����ʼ��m_fWordWrap���ܷ���CreateTextService���棬����û����
	}

	//////////////////////////////////////////////////////////////////////////
	// ����Text Service

	//GdiSetBatchLimit(1);  ///??Ϊʲô�������ôһ����룿

	IUnknown* pUnknown = NULL;
	HRESULT hr = ::CreateTextServices(NULL, static_cast<ITextHost*>(this), &pUnknown);
	if (FAILED(hr))
		return false;

	hr = pUnknown->QueryInterface(IID_ITextServices_Fix, (void**)&m_spTextServices);
 	pUnknown->Release();

	if (FAILED(hr) || NULL == m_spTextServices)
		return false;

	m_hParentWnd = hWndParent;

	RECT  rcClient;
	this->TxGetClientRect(&rcClient);
	hr = m_spTextServices->OnTxInPlaceActivate(&rcClient);
    m_spTextServices->OnTxUIActivate();
	UIASSERT(SUCCEEDED(hr));

	// ����Ӣ���������������岻ͳһ������
	if (m_spTextServices)
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

	// ע������ǧ��Ҫֱ�Ӵ���this��ȥ������ᵼ��txservice����texthostʱ��ָ�����
	hr = m_spTextServices->TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)static_cast<IRichEditOleCallback*>(this), &lr);
    
    // 4.1���ϵ�RE����֧��TSF(Text Service Framework)�������Ϳ��Խ���΢��ƴ�����뷨��ʾ����ȷ������
//    m_spTextServices->TxSendMessage(EM_SETEDITSTYLE, SES_USECTF, SES_USECTF, &lr);

    ModifyEventMask(ENM_CHANGE, 0);
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
		s_cfRichEditOleFormat = ::RegisterClipboardFormat(_T("UI_RichEdit_Ole_Format"));
		s_cfUnicodeRichEditOleFormat = ::RegisterClipboardFormat(_T("UI_Unicode_RichEdit_Ole_Format"));
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
	if(NULL == m_spTextServices || NULL == hDC)  // Remark:�������NULL hdc��txdraw����Ȼ������ڴ�й¶...
		return;

	CRect  rcClient;
	this->TxGetClientRect(&rcClient);

#if 0
	Image image;
	image.Create(rcClient.Width(), -rcClient.Height(), 32, Image::createAlphaChannel);
	image.SetAlpha(1);
	HBITMAP hBitmap = image.Detach();
	HDC hCacheDC = UI_GetCacheDC();
	HBITMAP holdBitmap = (HBITMAP)::SelectObject(hCacheDC, hBitmap);

	::OffsetRect(&rcClient, -rcClient.left, -rcClient.top);

	m_spTextServices->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hCacheDC/*hDC*/,
		NULL, (RECTL *)&rcClient, NULL, NULL, NULL, NULL, TXTVIEW_ACTIVE);

	::SelectObject(hCacheDC, holdBitmap);
	UI_ReleaseCacheDC(hCacheDC);

	image.Attach(hBitmap);
	image.FixGDIAlphaChannel(1);
	image.ForceUseAlpha();
	image.Draw(hDC, 0,0);
#endif

	// ע��TxDraw�����һ�������ܽ�����
	//     ������û�н���ʱ������TXTVIEW_ACTIVE��������TxDraw���ֽ������ʾ����
	//     �������н���ʱ������TXTVIEW_INACTIVE��������caretλ�ò���ˢ�£����細�ڴ�С�ı�ʱ����
	::OffsetRect(&rcClient, -rcClient.left, -rcClient.top);

    m_bDuringTxDraw = true;
	m_spTextServices->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hDC,
					NULL, (RECTL *)&rcClient, NULL, NULL, NULL, NULL, 
					m_pRichEdit->GetIRichEdit()->IsFocus()?TXTVIEW_ACTIVE:TXTVIEW_INACTIVE);
    m_bDuringTxDraw = false;
}

// handled��ʾrichedit�����������Ϣ
LRESULT WindowlessRichEdit::OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	SetMsgHandled(FALSE);
	if (NULL == m_spTextServices) 
	{
		SetMsgHandled(TRUE);
		return 0;
	}

	return 0;
}
LRESULT WindowlessRichEdit::OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return OnDefaultHandle(Msg, wParam, lParam);
}

LRESULT  WindowlessRichEdit::OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lr = 0;
    HRESULT hr = m_spTextServices->TxSendMessage(uMsg, wParam, lParam, &lr);

    if (hr == S_FALSE)
    {
        lr = ::DefWindowProc(m_hParentWnd, uMsg, wParam, lParam);
    }
    return 1;
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
    m_bFocus = false;
	m_pRichEdit->GetCaret()->DestroyCaret(m_pRichEdit->GetIRichEdit(), false);
    m_pRichEdit->GetIRichEdit()->UpdateObject();
	SetMsgHandled(FALSE);
}

//
// TODO:
// Ϊ�˽������һ�ε��RE�϶�û��Ӧ�����⣬
// ��LButtonDown���ж����û�н������ֶ�����һ��SETFOCUS
// �²��������Ϊ������WM_LBUTTONDOWN��Ϣ�����͸�RE��Ȼ���ٴ���WM_FUCOS��Ϣ���͸�RE��������ק�жϡ�
void  WindowlessRichEdit::OnSetFocus(HWND wndOld)
{
    if (!m_bFocus)
    {
        m_bFocus = true;
        OnDefaultHandle(WM_SETFOCUS, 0, 0); 
    }
}
LRESULT WindowlessRichEdit::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetMsgHandled(FALSE);
    if (!m_bFocus)
    {
        m_bFocus = true;
        OnDefaultHandle(uMsg, wParam, lParam);  // ���ڶ�λ�¹��λ�á�ֱ�ӷ���WM_SETFOCUS�ᵼ�¾ɵĹ��λ����ʾ������Ȼ�����ܵ���λ����ȥ
        OnDefaultHandle(WM_SETFOCUS, 0, 0);     // ���ý��㣬����WM_FUCOS��Ϣ�еĴ���
    }
    return 0;
}

void WindowlessRichEdit::OnWindowPosChanged(LPWINDOWPOS)
{
	SetMsgHandled(FALSE);

	if (m_spTextServices)
	{
		// �����������RICHEDIT�Ͳ�֪ͨ����TxSetScrollRange����ʾ������
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TRUE);  
// 		UIDLL.dll!UI::WindowlessRichEdit::TxSetScrollRange(int fnBar=1, long nMinPos=0, int nMaxPos=464, int fRedraw=0)  Line 553	C++
// 		riched20.dll!CDisplayML::UpdateScrollBar()  + 0x12b56 bytes	
// 		riched20.dll!CDisplayML::RecalcScrollBars()  + 0x7994 bytes	
// 		riched20.dll!CDisplayML::RecalcView()  + 0x9a bytes	
// 		riched20.dll!CDisplay::UpdateView()  + 0x6e bytes	
// 		riched20.dll!CTxtEdit::NeedViewUpdate()  + 0x8 bytes	
// 		riched20.dll!CTxtEdit::OnTxPropertyBitsChange()  - 0xd bytes	
// 		UIDLL.dll!UI::WindowlessRichEdit::OnWindowPosChanged(tagWINDOWPOS * __formal=0x0049decc)  Line 326 + 0x30 bytes	C++

		// ��windowless richedit�������ù���λ��
		if (m_pRichEdit->GetIRichEdit()->IsFocus())  
		{
			m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);  // ��ǿ����ʾ��꣬��˼��˸�isfocus�ж�
		}

		// �ò�������������ȥ���¹��λ�ã����ǻ�����һ�������е�������ջ���£�
		// UIDLL.dll!CCaret::SetCaretPos(int x=232, int y=91)  ��282	C++
		// UIDLL.dll!ITextHostImpl::TxSetCaretPos(int x=232, int y=91)  ��317	C++
		// riched20.dll!CTxtSelection::CreateCaret()  + 0xfe �ֽ�	
		// riched20.dll!CTxtSelection::UpdateCaret()  + 0x12d7f �ֽ�	
		// riched20.dll!CDisplay::Draw()  + 0x151 �ֽ�	
		// riched20.dll!CTxtEdit::TxDraw()  + 0xeb �ֽ�	
		// UIDLL.dll!WindowlessRichEdit::Draw(HDC__ * hDC=0xd90112c6)  ��103 + 0x42 �ֽ�	C++
		// UIDLL.dll!RichEditBase::OnPaint(UI::IRenderDC * hRDC=0x01f98bd0)  ��48	C++
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
// 	if (pGifOleObject)
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
	if (m_pRichEdit)
	{
        if (0 == wParam)
        {
		    if (false == DoGifOleUpdateRequst()) // �ú�����Ҫ�������ƶ���ˢ��֡��
			    return 0;
    		
		    m_pRichEdit->GetIRichEdit()->UpdateObject();
        }
        else  // ƽ������
        {
            TimerItem*  pItem = (TimerItem*)lParam;
            if (pItem->wParam == 1)
            {
                LRESULT lres = 0;
                m_spTextServices->TxSendMessage(WM_TIMER, pItem->nId, 0, &lres);
            }
        }
	}
    
	return 0;
}


//@cmember Create a timer with the specified timeout
BOOL WindowlessRichEdit::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    //TODO:	fTimer = TRUE;
    TimerItem item ;
    item.pNotify = m_pRichEdit->GetIRichEdit();
    item.nId = idTimer;
    item.wParam = 1;
    item.lParam = 0;
    return m_pRichEdit->GetIRichEdit()->GetUIApplication()->SetTimer(uTimeout, &item);
//        return ::SetTimer(m_hParentWnd, idTimer, uTimeout, NULL);
}

//@cmember Destroy a timer
void WindowlessRichEdit::TxKillTimer(UINT idTimer)
{
   // ::KillTimer(m_hParentWnd, idTimer);
    //TODO:	fTimer = FALSE;
    m_pRichEdit->GetIRichEdit()->GetUIApplication()->KillTimerById(idTimer, m_pRichEdit->GetIRichEdit());
}

//@cmember Get the view rectangle relative to the inset
// ��ʵ����������߿��padding
// ����UI�ؼ���padding�����ǻ���
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
	m_pRichEdit->GetIRichEdit()->GetClientRectInWindow((CRect*)prc);
	
#if 0 // TODO: delete 
	// ��ȥpadding�Ĵ�С����Ϊpadding���Ѿ���richedit����Ϊinset view rect��ʵ��
	CRegion4 rPadding;
	m_pRichEditBase->GetPaddingRegion(&rPadding);
	prc->left -= rPadding.left;
	prc->top  -= rPadding.top;
	prc->right  += rPadding.right;
	prc->bottom += rPadding.bottom;
#endif

	return S_OK;
}

bool WindowlessRichEdit::GetInvalidateRect(RECT* prc, bool* pbNeedRedrawScrollbar, bool bClear)
{
	if (::IsRectEmpty(&m_rcInvalidate))
		return false;

	::CopyRect(prc, &m_rcInvalidate);
	if (pbNeedRedrawScrollbar)
	{
		*pbNeedRedrawScrollbar = m_bNeedRedrawScrollbar;
	}

	if (bClear)
	{
		m_bNeedRedrawScrollbar = false;
		::SetRectEmpty(&m_rcInvalidate);
	}

	return true;
}
//@cmember InvalidateRect
// ע��prc����ֻ��һ�е������С����ˢ�¸�����Ļ�����߻���Ч�ʣ�
void /*ITextHostImpl*/WindowlessRichEdit::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if (m_pRichEdit && m_pRichEdit->GetIRichEdit()->CanRedraw())
	{
        if (prc)
        {
		    UnionRect(&m_rcInvalidate, &m_rcInvalidate, prc);
        }
        else
        {
            // ����ģʽ����קѡ�лᴥ���öδ��룬prcΪ��
            CRect rcWnd;
            m_pRichEdit->GetIRichEdit()->GetWindowRect(&rcWnd);

            UnionRect(&m_rcInvalidate, &m_rcInvalidate, &rcWnd);
        }

        UIMSG msg;
        msg.message = UI_WM_REDRAWOBJECT;
        msg.pMsgTo = m_pRichEdit->GetIRichEdit();
        UIPostMessage(m_pRichEdit->GetIRichEdit()->GetUIApplication(), &msg);  // ����RECT������post��Ϣ�в��ô��ݣ���˽����򱣴�Ϊ��Ա������ͬʱҲ�������ϲ���Ч���������

//         if (prc)
//             UI_LOG_DEBUG(_T("%s RECT:%d,%d, %d,%d"), FUNC_NAME, prc->left, prc->top, prc->right, prc->bottom);
//         else
//             UI_LOG_DEBUG(_T("%s All"), FUNC_NAME);
	}

//	::InvalidateRect(m_pRichEdit->GetHWND(), prc, fMode);
//	::InvalidateRect(m_hParentWnd, prc, fMode);
}

//@cmember Get the background (either opaque or transparent)
// ע������TXTBACK_TRANSPARENT��richedit����ɱ���͸��
//     �����û���������ֵ�󣬽����а�ɫ����
HRESULT /*ITextHostImpl*/WindowlessRichEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	// Ĭ��ʹ��RichEdit backrender�ı���������ͳһ��չ
// 	if (m_pRichEdit->IsTransparent())
 		*pstyle = TXTBACK_TRANSPARENT;
// 	else
// 		*pstyle = TXTBACK_OPAQUE;

	return S_OK;
}



//@cmember Show the scroll bar
BOOL WindowlessRichEdit::TxShowScrollBar(INT fnBar, BOOL fShow) 
{
    IScrollBarManager*  pScrollBarMgr = m_pRichEdit->GetScrollMgr();
    if (NULL == pScrollBarMgr)
        return FALSE;

	m_bNeedRedrawScrollbar = true;
	if (FALSE == fShow)
	{
		if (fnBar == SB_HORZ || fnBar == SB_BOTH)
		{
            pScrollBarMgr->SetHScrollRange(0);
		}
		if (fnBar == SB_VERT || fnBar == SB_BOTH)
		{
			//--Ϊ�˽��ɾ������ʱ��ɾ������������ʧʱ�������漸����Ȼ�����ص����⡣��ʱӦ�ý���һ����ʾ����
			if (0 != pScrollBarMgr->GetVScrollPos())
			{
				LRESULT lr = 0;
				m_spTextServices->TxSendMessage(WM_VSCROLL,MAKEWPARAM(SB_TOP,0),0, &lr);
				pScrollBarMgr->SetVScrollPos(0);
			}
			//--

			pScrollBarMgr->SetVScrollRange(0);
			m_pRichEdit->GetIRichEdit()->UpdateObject();
		}
	}
	return TRUE;
}

//@cmember Enable the scroll bar
BOOL WindowlessRichEdit::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
{
//	UI_LOG_DEBUG(FUNC_NAME);
	m_bNeedRedrawScrollbar = true;
	return TRUE;
}

//@cmember Set the scroll range
// PS: ����RICHEDIT������һ��BUG.�ڸı�RICHEDIT��Сʱ��RICHEDIT������TxShowScrollbar��TxSetScrollPos
//     ��֪ͨ�����Ǿ�Ȼû�з���TxSetScrollRagne��֪ͨ�����¹�������֪�����ʾ��
//     (QQ2012 Input EditҲ��������⣬��Output Edit��������ʾ���ѵ���READONLY�й�ϵ����
BOOL WindowlessRichEdit::TxSetScrollRange(
									 INT fnBar,
									 LONG nMinPos,
									 INT nMaxPos, 
									 BOOL fRedraw)   // ���fRedraw������false 
{
    IScrollBarManager*  pScrollBarMgr = m_pRichEdit->GetScrollMgr();
    if (NULL == pScrollBarMgr)
        return FALSE;

	m_bNeedRedrawScrollbar = true;

	if (SB_HORZ == fnBar)
	{
		pScrollBarMgr->SetHScrollRange(nMaxPos - nMinPos);
	}
	else if (SB_VERT == fnBar)
	{
		pScrollBarMgr->SetVScrollRange(nMaxPos - nMinPos);
	}

	return TRUE;
}


//
//@cmember Set the scroll position
// ���������ݵ����������һ���س����������Ӧ����TxSetScrollPos�����ҵ�һ�ε�nPos�Ǿ�ֵ����������fRedraw��Ϊ1
// ����¹���������˸һ�Σ���֪��Ϊ��Ҫ������
//
// Ϊ�˹��������⣬����������һЩ����
//  1. �����ﲻȥ����fRedraw������ˢ�¹�������������txinvalidateectˢ�¿ؼ�ʱͬʱˢ�¹�����
//  2. ��TxInvalidateRectʱ����������ȥˢ�£�����Ҳ�����¹�������˸��������ͨ��ģ��windowsϵͳ����Ϊ��postһ��
//     UI_WM_REDRAWOBJECT��Ϣ��richedit�ؼ����ɴ���ʵ���ӳ�ˢ�¡�
//  3. ��RichEditBase::OnRedrawObject�У�����Ƿ���Ҫˢ�¹�����
//
BOOL WindowlessRichEdit::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)   
{
    IScrollBarManager*  pScrollBarMgr = m_pRichEdit->GetScrollMgr();
    if (NULL == pScrollBarMgr)
        return FALSE;

	m_bNeedRedrawScrollbar = true;

	if (SB_HORZ == fnBar)
	{
        pScrollBarMgr->SetHScrollPos(nPos);
	}
	else if (SB_VERT == fnBar)
	{
	    pScrollBarMgr->SetVScrollPos(nPos);
	}
	return FALSE;
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

//
//@cmember Create the caret
// hbmp [in] 
//      Handle to the bitmap that defines the caret shape.
//      If this parameter is NULL, the caret is solid. 
//      If this parameter is (HBITMAP) 1, the caret is gray. 
//      If this parameter is a bitmap handle, the caret is the specified bitmap. 
//      The bitmap handle must have been created by the CreateBitmap, 
//      CreateDIBitmap, or LoadBitmap function. 
//
//      If hBitmap is a bitmap handle, CreateCaret ignores 
//      the nWidth and nHeight parameters; the bitmap 
//      defines its own width and height.
//
BOOL WindowlessRichEdit::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    if (!m_bFocus)
        return FALSE;

//     if (NULL == hbmp)  // �����Ϊ�գ���ǰ��richedit����ѡ�����򣬲���ʾ���
//     {
        bool  bNeedFixGdiAlpha = m_pRichEdit->IsNeedFixGdiAlpha();
        return m_pRichEdit->GetCaret()->CreateCaret(m_pRichEdit->GetIRichEdit(), hbmp, xWidth, yHeight, /*bNeedFixGdiAlpha?CARET_TYPE_WINDOW:*/CARET_TYPE_UNKNOWN);
//     }
//     else
//     {
//         m_pRichEdit->GetCaret()->DestroyCaret(false);  // TODO: ���ﲻ����ˢ���ˣ���Ϊ��������ܾ�����REά���������ˣ��ᵼ����ѭ��
//     }
// 	return TRUE;
}

//@cmember Show the caret
BOOL WindowlessRichEdit::TxShowCaret(BOOL fShow) 
{
    // �����TxDraw����������Ϣ������ˢ��
	if (fShow)
		m_pRichEdit->GetCaret()->ShowCaret(m_pRichEdit->GetIRichEdit(), !m_bDuringTxDraw);
	else
		m_pRichEdit->GetCaret()->HideCaret(m_pRichEdit->GetIRichEdit(), !m_bDuringTxDraw);

	return TRUE;
}

//@cmember Set the caret position
BOOL WindowlessRichEdit::TxSetCaretPos(INT x, INT y)
{

    // �п����������������λ���޸ģ����ʱ��Ҫˢ�¹�ꡣ�������TxDraw��������Ϣ������ˢ��

    CRect rc;
    m_pRichEdit->GetIRichEdit()->GetWindowRect(&rc);
	m_pRichEdit->GetCaret()->SetCaretPos(m_pRichEdit->GetIRichEdit(), x-rc.left, y-rc.top, !m_bDuringTxDraw);

	return TRUE;
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
// �ýӿڲ�����������text��ibean��꣬������sel bar��ָ��
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

// ��������ʵ��
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

// Far East Methods for getting the Input Context
//#ifdef WIN95_IME
// ���ö�ջ��
//  ���ڽ��յ�WM_IME_STARTCOMPOSITION��Ϣ��ת����richedit
//  
// UICtrls.dll!UI::ITextHostImpl::TxImmGetContext()  ��983	C++
// riched20.dll!CTxtEdit::GetImmContext()  + 0x2e �ֽ�	
// riched20.dll!LocalGetImmContext()  + 0x1b �ֽ�	
// riched20.dll!CIme::SetCompositionFont()  + 0x28 �ֽ�	
// riched20.dll!CIme_Lev2::StartComposition()  + 0x1a �ֽ�	
// riched20.dll!StartCompositionGlue()  + 0x15d �ֽ�	
// riched20.dll!CTextMsgFilter::HandleMessage()  + 0x3b6 �ֽ�	
// riched20.dll!CTxtEdit::TxSendMessage()  + 0x1d4f �ֽ�	
HIMC ITextHostImpl::TxImmGetContext()
{
    return ImmGetContext(m_hParentWnd);
}
void ITextHostImpl::TxImmReleaseContext(HIMC himc)
{
    ImmReleaseContext(m_hParentWnd, himc);
}
//#endif

//@cmember Returns HIMETRIC size of the control bar.
// ע����������øñ������ᵼ��cursor��ʾ�쳣����ʾ��Ϊһ����ѡ����ʽ����ͷ�������Ͻǣ�
//	   ��VS��һ������
//
// ��google���ҵ����йظ����õ���Ϣ��
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
//               ITempHostImpl �������÷���
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
    m_cf.cbSize = sizeof(CHARFORMAT2W);
    m_cf.dwMask = CFM_ALL;
    
    _tcscpy(m_cf.szFaceName, _T("����"));
	InitDefaultParaFormat();

	if (!(m_dwStyle & ES_LEFT))
	{
		if (m_dwStyle & ES_CENTER)
			m_pf.wAlignment = PFA_CENTER;
		else if (m_dwStyle & ES_RIGHT)
			m_pf.wAlignment = PFA_RIGHT;
	}
}

ITextHostImpl::~ITextHostImpl()
{
}


bool ITextHostImpl::SetCharFormatByLogFont(LOGFONT* plf)
{
    if (NULL == plf)
        return false;

    HWND hWnd = GetDesktopWindow();
    HDC  hDC = GetDC(hWnd);
    LONG yPixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
    m_cf.yHeight = abs(plf->lfHeight * 1440 / yPixPerInch);
    ReleaseDC(hWnd, hDC);

    if (plf->lfWeight >= FW_BOLD)
        m_cf.dwEffects |= CFE_BOLD;
    else
        m_cf.dwEffects &= ~CFE_BOLD;

    if (plf->lfItalic)
        m_cf.dwEffects |= CFE_ITALIC;
    else
        m_cf.dwEffects &= ~CFE_ITALIC;

    if (plf->lfUnderline)
        m_cf.dwEffects |= CFE_UNDERLINE;
    else
        m_cf.dwEffects &= ~CFE_UNDERLINE;

    if (plf->lfStrikeOut)
        m_cf.dwEffects |= CFE_STRIKEOUT;
    else
        m_cf.dwEffects &= ~CFE_STRIKEOUT;
        
    m_cf.bCharSet = plf->lfCharSet;
    m_cf.bPitchAndFamily = plf->lfPitchAndFamily;
#ifdef UNICODE
    _tcscpy(m_cf.szFaceName, plf->lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, plf->lfFaceName, LF_FACESIZE, m_cf.szFaceName, LF_FACESIZE) ;
#endif

    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
            TXTBIT_CHARFORMATCHANGE);
    }

    return true;
}
bool ITextHostImpl::SetCharFormat(CHARFORMAT2* pcf)
{
	if (NULL == pcf)
		return false;

	// Set CHARFORMAT structure
    if (pcf->dwMask & CFM_SIZE)
        m_cf.yHeight = pcf->yHeight;

    if (pcf->dwMask & CFM_COLOR)
    {
        m_cf.crTextColor = pcf->crTextColor; 
    }
    if (pcf->dwMask & CFM_BACKCOLOR)
    {
        m_cf.crBackColor = pcf->crBackColor; 
    }

    if (pcf->dwMask & CFM_BOLD)
    {
        if (pcf->dwEffects & CFE_BOLD)
            m_cf.dwEffects |= CFE_BOLD;
        else
            m_cf.dwEffects &= ~CFE_BOLD;
    }
    if (pcf->dwMask & CFM_ITALIC)
    {
        if (pcf->dwEffects & CFE_ITALIC)
            m_cf.dwEffects |= CFE_ITALIC;
        else
            m_cf.dwEffects &= ~CFE_ITALIC;
    }
    if (pcf->dwMask & CFM_UNDERLINE)
    {
        if (pcf->dwEffects & CFM_UNDERLINE)
            m_cf.dwEffects |= CFM_UNDERLINE;
        else
            m_cf.dwEffects &= ~CFM_UNDERLINE;
    }

    if (pcf->dwMask & CFM_FACE)
    {
        _tcscpy(m_cf.szFaceName, pcf->szFaceName);
    }
// 	m_cf.dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;
// 	m_cf.dwEffects &= ~(CFE_PROTECTED | CFE_LINK | CFE_AUTOCOLOR);

//  LRESULT lr = 0;
//  m_spTextServices->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&m_cf, &lr);

    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
            TXTBIT_CHARFORMATCHANGE);
    }
	return true;
}

void  ITextHostImpl::GetCharFormat(CHARFORMAT2* pcf)
{
    if (NULL == pcf)
        return;

    if (pcf->dwMask & CFM_SIZE)
        pcf->yHeight = m_cf.yHeight;

    if (m_cf.dwMask & CFM_COLOR)
        pcf->crTextColor = m_cf.crTextColor; 
    if (m_cf.dwMask & CFM_BACKCOLOR)
        pcf->crBackColor = m_cf.crBackColor; 

    if (pcf->dwMask & CFM_BOLD)
    {
        if (m_cf.dwEffects & CFE_BOLD)
            pcf->dwEffects |= CFE_BOLD;
    }
    if (pcf->dwMask & CFM_ITALIC)
    {
        if (m_cf.dwEffects & CFE_ITALIC)
            pcf->dwEffects |= CFE_ITALIC;
    }
    if (pcf->dwMask & CFM_UNDERLINE)
    {
        if (m_cf.dwEffects & CFM_UNDERLINE)
            pcf->dwEffects |= CFM_UNDERLINE;
    }

    if (pcf->dwMask & CFM_FACE)
    {
        _tcscpy(pcf->szFaceName, m_cf.szFaceName);
    }
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
	if (m_spTextServices)
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
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);
	}

	return laccelposOld;
}

void ITextHostImpl::SetWordWrap(bool fWordWrap)
{
    if (m_fWordWrap == (int)fWordWrap)
        return;

	m_fWordWrap = fWordWrap;
	if (m_spTextServices)
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
	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_READONLY, dwUpdatedBits);
	}
}
void  ITextHostImpl::SetMultiLine(bool b)
{
    if (IsMultiLine() == b)
        return;

    if (b)
    {
        m_dwStyle |= ES_MULTILINE; 
        m_dwStyle &= ~ES_AUTOHSCROLL;
    }
    else
    {
        m_dwStyle  &= ~ES_MULTILINE;  // Ϊ����ʱ����Ҫ�ܹ�������ʱ�Զ�����
        m_dwStyle |= ES_AUTOHSCROLL;
    }

    // Notify control of property change
    if (m_spTextServices)
    {
        m_spTextServices->OnTxPropertyBitsChange(TXTBIT_MULTILINE, b ? 1:0);
    }
}
bool  ITextHostImpl::IsMultiLine()
{
    return (m_dwStyle & ES_MULTILINE)?true:false;
}

void ITextHostImpl::SetMaxLegnth(DWORD dw)
{
	m_dwMaxLength = dw;
	
	if (m_spTextServices)
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

	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);
	}
	return lOldSelBarWidth;
} 

void ITextHostImpl::SetRichTextFlag(bool b)
{
	m_fRich = b;

	if (m_spTextServices)
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
// ��ע��
// Applications that use the following functionality must call OleInitialize 
// before calling any other function in the COM library: 
//		Clipboard
//		Drag and drop
//		Object linking and embedding (OLE)
//		In-place activation
// ��oleInitialize��ʼ��COM�⣬������ʹ��CoInitialize
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

	if (m_spTextServices)
	{
		m_spTextServices->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
			b? TXTBIT_USEPASSWORD:0);
	}
}

bool WindowlessRichEdit::SetText(const TCHAR* szText)
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

int  WindowlessRichEdit::GetText(TCHAR* szBuf, int nLen)
{
    if (NULL == m_spTextServices)
        return 0;

    GETTEXTEX textex = {0};
    textex.cb = (nLen+1)*sizeof(TCHAR);  // nSize��Ҫ����ĩβ��\0��С
    textex.flags = GT_DEFAULT;
    textex.codepage = 1200; // unicode

    LRESULT lr = 0;
    return m_spTextServices->TxSendMessage(EM_GETTEXTEX, (WPARAM)&textex, (LPARAM)szBuf, &lr);
}

// [ע��]Ӧ�÷��ص���lr��������TxSendMessage!!
int   WindowlessRichEdit::GetTextLengthW()
{
    LRESULT lr = 0;

    GETTEXTLENGTHEX data = {0};
    data.flags = GTL_DEFAULT;
    data.codepage = 1200; // unicode
    m_spTextServices->TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&data, (LPARAM)0, &lr);
    return lr;

    // m_spTextServices->TxSendMessage(WM_GETTEXTLENGTH, 0, 0, &lr);
}
void  WindowlessRichEdit::GetTextW(IBuffer** ppBuffer)
{
    int nSize = GetTextLengthW();
    if (0 == nSize)
        return;

    TCHAR*  szBuffer = new TCHAR[nSize];
    GetText(szBuffer, nSize);

    UI::CreateTCHARBuffer(ppBuffer);
    (*ppBuffer)->SetBuffer((void*)szBuffer, nSize);
}

bool  WindowlessRichEdit::GetEncodeTextW(IBuffer** ppBuffer)
{
    if (NULL == ppBuffer)
        return false;

    int nSize = GetTextLengthW();
    if (0 == nSize)
        return false;

    TCHAR*  szBuffer = new TCHAR[nSize+1];
    GetText(szBuffer, nSize);

    CMarkup markup;
    markup.AddElem(L"RE");
    markup.AddAttrib(L"version", L"1.0");

    // ����������Ϣ
    if (m_cf.dwEffects & CFE_BOLD)
        markup.AddAttrib(L"bold", 1);
    if (m_cf.dwEffects & CFE_ITALIC)
        markup.AddAttrib(L"italic", 1);
    if (m_cf.dwEffects & CFE_UNDERLINE)
        markup.AddAttrib(L"underline", 1);
    
    markup.AddAttrib(L"color", m_cf.crTextColor);
    markup.AddAttrib(L"face", m_cf.szFaceName);
    markup.AddAttrib(L"height", (int)m_cf.yHeight);
    markup.IntoElem();

    // �����ı���ole
    int nTextStartIndex = 0;
    int i = 0;
    for (; i < nSize; i++)
    {
        if (szBuffer[i] == WCH_EMBEDDING)
        {
            int nCount = i - nTextStartIndex;
            if (nCount > 0)
            {
                String str;
                str.append(szBuffer+nTextStartIndex, nCount);
                markup.AddElem(L"text", str);
                markup.AddAttrib(L"length", nCount);

                nTextStartIndex = i+1;
            }

            IRichEditOleObjectItem* pOleObj = GetOleObjectByCharPos(i);
            if (pOleObj)
            {
                CComBSTR bstrEncode;
                pOleObj->GetEncodeText(&bstrEncode);
                if (bstrEncode.Length() > 0)
                {
                    markup.AddSubDoc((BSTR)bstrEncode);
                }
            }
        }
    }
    // ���һ���ı�
    int nCount = i - nTextStartIndex;
    if (nCount > 0)
    {
        String str;
        str.append(szBuffer+nTextStartIndex, nCount);
        markup.AddElem(L"text", str);
        markup.AddAttrib(L"length", nCount);
    }
    SAFE_ARRAY_DELETE(szBuffer);

    UI::CreateTCHARBuffer(ppBuffer);

    szBuffer = new TCHAR[markup.GetDoc().length()+1];
    _tcscpy(szBuffer, markup.GetDoc().c_str());
    (*ppBuffer)->SetBuffer(szBuffer, markup.GetDoc().length());
    return true;
}

bool  WindowlessRichEdit::ReplaceSelectionText(const TCHAR* szText, int nSize)
{
    // EM_REPLACESEL��Ȼֻ������null�ַ���β���ַ���
    if (m_spTextServices)
    {
        if (-1 == nSize || szText[nSize] == _T(''))
        {
            LRESULT lr = 0;
            m_spTextServices->TxSendMessage(EM_REPLACESEL, TRUE, (LPARAM)szText, &lr);
        }
        else
        {
            TCHAR* szTemp = new TCHAR[nSize+1];
            memcpy(szTemp, szText, nSize*sizeof(TCHAR));
            szTemp[nSize] = _T('');

            LRESULT lr = 0;
            m_spTextServices->TxSendMessage(EM_REPLACESEL, TRUE, (LPARAM)szTemp, &lr);

            SAFE_ARRAY_DELETE(szTemp);
        }
    }
    return true;
}
bool  WindowlessRichEdit::AppendText(const TCHAR* szText, int nSize)
{
    SetSel(-1, 0);
    return ReplaceSelectionText((TCHAR*)szText, nSize);
}
bool  WindowlessRichEdit::AppendEncodeTextW(const TCHAR* szText, int nSize)
{
    if (NULL == szText)
        return false;

    CMarkup markup;
    if (!markup.SetDoc(szText))
        return false;

    if (!markup.FindElem(L"RE"))
        return false;

    // ����������ʽ
    String  strColor = markup.GetAttrib(L"color");
    String  strFace = markup.GetAttrib(L"face");
    String  strHeight = markup.GetAttrib(L"height");
    String  strBold = markup.GetAttrib(L"bold");
    String  strItalic = markup.GetAttrib(L"italic");
    String  strUnderline = markup.GetAttrib(L"underline");

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_ALL;
    cf.yHeight = _ttoi(strHeight.c_str());
    cf.crTextColor = _ttoi(strColor.c_str());
    _tcscpy(cf.szFaceName, strFace.c_str());
    if (strBold == L"1")
        cf.dwEffects |= CFE_BOLD;
    if (strItalic == L"1")
        cf.dwEffects |= CFE_ITALIC;
    if (strUnderline == L"1")
        cf.dwEffects |= CFE_UNDERLINE;
    
    LRESULT lr = 0;
    SetSel(-1, 0);
    m_spTextServices->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lr);

    // �����ı���ole����
    if (markup.IntoElem())
    {
        while (markup.FindElem())
        {
            String strTag = markup.GetTagName();
            if (strTag == _T("text"))
            {
                AppendText(markup.GetData().c_str(), -1);
            }
            else if (strTag == _T("com"))
            {
                String strCLSID = markup.GetAttrib(_T("clsid"));
    
                CLSID clsid = {0};
                CLSIDFromString((LPOLESTR)strCLSID.c_str(), &clsid);
                InsertComObject(clsid);
            }
            else if (strTag == _T("emotion"))
            {
                String strId = markup.GetAttrib(_T("id"));
                InsertSkinGif(strId.c_str());
            }
            else if (strTag == _T("gif"))
            {
                String strPath = markup.GetAttrib(_T("path"));
                InsertGif(strPath.c_str());
            }
        }
    }
    AppendText(_T("\r\n"), -1);
    return true; 
}

//
//	���InsertObjectʱ��ȥ�ṩ/����storage����,��ô���ƶ����ճ��������ʧ��!
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
		reObj.dwUser = (DWORD)pItem;
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
	GifOleObject* pGifOle = new GifOleObject(
        m_pRichEdit->GetIRichEdit()->GetUIApplication(), 
        static_cast<IMessage*>(this->m_pRichEdit->GetIRichEdit()));

	HRESULT hr = pGifOle->LoadGif(szGifPath);
	if (FAILED(hr))
	{
		SAFE_DELETE(pGifOle);
		return false;
	}

	return this->InsertOleObject(pGifOle);
}

bool WindowlessRichEdit::InsertSkinGif(const TCHAR* szGifId)
{
    GifOleObject* pGifOle = new GifOleObject(
        m_pRichEdit->GetIRichEdit()->GetUIApplication(), 
        static_cast<IMessage*>(this->m_pRichEdit->GetIRichEdit()));

    HRESULT hr = pGifOle->LoadSkinGif(szGifId);
    if (FAILED(hr))
    {
        SAFE_DELETE(pGifOle);
        return false;
    }

    return this->InsertOleObject(pGifOle);
}

bool WindowlessRichEdit::InsertComObject(REFCLSID clsid)
{
	RichEditOleObjectItem_Com* pItem = new RichEditOleObjectItem_Com;
	pItem->Attach(clsid);

	bool bRet = this->InsertOleObject(pItem);

	IOleObject* p = NULL;
	pItem->GetOleObject(&p, false);
	p->Release();
	
	return bRet;
}

// ���pDataObject�еĸ��ָ�ʽ�������ȼ����в��� 
HRESULT WindowlessRichEdit::DoPaste(LPDATAOBJECT pDataObject, BOOL fReally, CLIPFORMAT* pOutClipFormat)
{
	if (NULL == pDataObject)
		return E_INVALIDARG;

	// �����Լ�ʵ�ֵ�OLE�����綯��
	do 
	{
		FORMATETC format = {0};
		format.cfFormat = s_cfUnicodeRichEditOleFormat;
		format.tymed = TYMED_HGLOBAL;
		format.dwAspect = DVASPECT_CONTENT;
        format.lindex = -1; // ��ʱ����0�ᱨ��Ч�Ľṹ

		if (!fReally)
		{
			if (FAILED(pDataObject->QueryGetData(&format)))
				break;
			return S_OK;
		}

		STGMEDIUM medium = {0};
		medium.tymed = TYMED_HGLOBAL;
		if (FAILED(pDataObject->GetData(&format, &medium)))
			break;

		if (TYMED_HGLOBAL != medium.tymed)
		{
			::ReleaseStgMedium(&medium);
			break;
		}

		HGLOBAL hGlobal = medium.hGlobal;
		LPWSTR lpwstr = (LPWSTR)::GlobalLock(hGlobal);
		bool bRet = m_pOleMgr->ParseOleFormatXml(lpwstr);
		GlobalUnlock(hGlobal);

		if (false == bRet)
		{
			::ReleaseStgMedium(&medium);
			break;
		}
		
		return S_OK;
	}
	while(0);

	// static object���ͣ���CF_DIB/CF_BITMAP

	return E_FAIL;


#if 0
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
		{
			// CF_METAFILEPICT, CF_DIB, or CF_ BITMAP, and CF_ENHMF
			sc = ::OleCreateStaticFromData(pDataObject, IID_IUnknown, OLERENDER_DRAW, NULL, pClientSite, pStorage, (void**)&lpOleObject);
		}

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
#endif
	//return bRet ? S_OK:E_FAIL;
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

WORD  WindowlessRichEdit::GetSelectionType() const
{
    LRESULT lr = 0;
    m_spTextServices->TxSendMessage(EM_SELECTIONTYPE, 0, 0L, &lr);
    return (WORD)lr;
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

IRichEditOleObjectItem* WindowlessRichEdit::GetSelectionOleObject()
{
	REOBJECT reObj;
	ZeroMemory(&reObj, sizeof(REOBJECT));
	reObj.cbStruct = sizeof(REOBJECT);

	HRESULT hr = m_spOle->GetObject(REO_IOB_SELECTION, &reObj, REO_GETOBJ_NO_INTERFACES);
    if (SUCCEEDED(hr) && reObj.dwUser)
    {
        return (IRichEditOleObjectItem*)reObj.dwUser;
    }
	return NULL;
}
IRichEditOleObjectItem*  WindowlessRichEdit::GetOleObjectByCharPos(int ncp)
{
    REOBJECT reObj;
    ZeroMemory(&reObj, sizeof(REOBJECT));
    reObj.cbStruct = sizeof(REOBJECT);
    reObj.cp = ncp;

    HRESULT hr = m_spOle->GetObject(REO_IOB_USE_CP, &reObj, REO_GETOBJ_NO_INTERFACES);
    if (SUCCEEDED(hr) && reObj.dwUser)
    {
        return (IRichEditOleObjectItem*)reObj.dwUser;
    }
    return NULL;
}

long  WindowlessRichEdit::GetEventMask()
{
    long lRet = 0;
    HRESULT hr = m_spTextServices->TxSendMessage(EM_GETEVENTMASK, 0, 0, (LRESULT*)&lRet);
    return lRet;
}
void  WindowlessRichEdit::SetEventMask(long n)
{
    LRESULT lRet = 0;
    HRESULT hr = m_spTextServices->TxSendMessage(EM_SETEVENTMASK, 0, n, &lRet);
}
void  WindowlessRichEdit::ModifyEventMask(long nAdd, long nRemove)
{
    long lOldMask = GetEventMask();
    long lNewMask = lOldMask | nAdd;
    lNewMask = lNewMask &~ nRemove;

    if (lNewMask != lOldMask)
    {
        SetEventMask(lNewMask);
    }
}

//
// ע��autoresizeģʽRE�ڲ�������֪ͨ��������ʾ��
//     ������ڵ���ģʽ��RE����Ӧ���.����ģʽ,RE����Ӧ�߶�
//
void  WindowlessRichEdit::SetAutoResize(bool b, int nMaxSize)
{
    if (b)
    {
        ModifyEventMask(ENM_REQUESTRESIZE, 0);
        m_nAutoResizeMaxSize = nMaxSize;
    }
    else
    {
        ModifyEventMask(0, ENM_REQUESTRESIZE);
        m_nAutoResizeMaxSize = 0;
    }
}


//@cmember Notify host of events
HRESULT WindowlessRichEdit::TxNotify(DWORD iNotify, void *pv)
{
    BOOL bHandled = FALSE;
    HRESULT hRes = m_pRichEdit->OnTxNotify(iNotify, pv, bHandled);
    if (bHandled)
        return hRes;

    return DefTxNotify(iNotify, pv);
}
HRESULT WindowlessRichEdit::DefTxNotify(DWORD iNotify, void* pv)
{
    switch (iNotify)
    {
    case EN_REQUESTRESIZE:
        {
            REQRESIZE* pData = (REQRESIZE*)pv;
            OnRequestResize(&pData->rc);
        }
        break;
    }

    return S_OK;
}
void  WindowlessRichEdit::OnRequestResize(RECT* prc)
{
    int nDesiredWidth = prc->right - prc->left;
    int nDesiredHeight = prc->bottom - prc->top;

    nDesiredWidth += m_pRichEdit->GetIRichEdit()->GetNonClientW();
    nDesiredHeight += m_pRichEdit->GetIRichEdit()->GetNonClientH();

    CRect rcParent;
    m_pRichEdit->GetIRichEdit()->GetParentRect(&rcParent);

    if (IsMultiLine())  // ��ȹ̶����߶�����Ӧ
    {
        if (m_nAutoResizeMaxSize > 0 && nDesiredHeight > m_nAutoResizeMaxSize)
            nDesiredHeight = m_nAutoResizeMaxSize;

        rcParent.bottom = rcParent.top + nDesiredHeight;
    }
    else  // �߶ȹ̶����������Ӧ
    {
        if (m_nAutoResizeMaxSize > 0 && nDesiredWidth > m_nAutoResizeMaxSize)
            nDesiredWidth = m_nAutoResizeMaxSize;

        rcParent.right = rcParent.left + nDesiredWidth;
    }

    //  �����Ϣ���Ǵ�CTxEdit::TxDraw�����ģ���ˢ�¾���ѭ����
    m_pRichEdit->GetIRichEdit()->SetObjectPos(&rcParent, SWP_NOREDRAW);
}

#pragma region  // IRichEditOleCallback
// This method must be implemented to allow cut, copy, paste, drag, 
// and drop operations of Component Object Model (COM) objects.
// ������richedit���������һ�������ϵ�ͼ�꣬�ͻ���øú���
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
// ����richedit ole��InsertObject����ʱ����������ص�����
// �ڴ��ⲿ����һ���ļ���richeditʱ������Ӧ��GetNewStorage�ɹ��󣬾ͻ��ٵ�������ӿں���
// ������S_OKʱ��������󽫱����룬��������ֵʱ�����󽫲��ᱻ����
HRESULT __stdcall WindowlessRichEdit::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}

// ���罫richedit�е�һ��COM����ɾ����������һ�θýӿں���
// ���罫richedit�е�һ��COM�����������ק����һ��λ�ã�������һ�θýӿں���
// �ú�������һ��֪ͨ������������һ������Ҫ��deleted from rich edit control;
// ���ﲻ�õ���release lpoleobj?
HRESULT __stdcall WindowlessRichEdit::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}

//  
//  ��richedit��ʹ�� CTRL+Vճ������קʱ��ѯ���Ƿ����
//
// 1. ���ճ��HTML��ʽ��
//    ��MSDN������ HTML Clipboard Format
//
// 2. BOOL fReally����ʲô�ģ�
//   a. ��MSDN�Ͽ�����������EM_CANPASTE��Ϣ�ģ����ʱ���check����ȥimport
//   b. ����ק������ֱ��������ǰfReally����0������������ʱ��Ϊ1
//
//   ��ջ���£�
// >UIDLL.dll!UI::WindowlessRichEdit::QueryAcceptData(IDataObject * lpdataobj=0x0030f518, unsigned short * lpcfFormat=0x002ed81c, unsigned long reco=1, int fReally=0, void * hMetaPict=0x00000000)  ��1634	C++
// riched20.dll!CLightDTEngine::CanPaste()  + 0x69 �ֽ�	
// riched20.dll!CDropTarget::DragEnter()  + 0x5e �ֽ�	
// ole32.dll!CPrivDragDrop::PrivDragDrop()  + 0x1c9 �ֽ�	
// ole32.dll!PrivDragDrop()  + 0xa3 �ֽ�	
// ole32.dll!CDragOperation::GetDropTarget()  + 0x5e �ֽ�	
// ole32.dll!CDragOperation::UpdateTarget()  + 0x242 �ֽ�	
// ole32.dll!_DoDragDrop@16()  + 0xaa �ֽ�	
// riched20.dll!CW32System::DoDragDrop()  + 0x33 �ֽ�	
// riched20.dll!CLightDTEngine::StartDrag()  + 0x16c �ֽ�	
// riched20.dll!CTxtEdit::OnTxMouseMove()  + 0x1aa �ֽ�	
// riched20.dll!CTxtEdit::TxSendMessage()  + 0x24d4 �ֽ�	
// UIDLL.dll!UI::WindowlessRichEdit::OnPostHandleMsg(HWND__ * hWnd=0x00120626, unsigned int Msg=512, unsigned int wParam=1, long lParam=28377121)  ��270 + 0x39 �ֽ�	C++
//
HRESULT __stdcall WindowlessRichEdit::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	if (!fReally) // not actually pasting
		return S_OK;

	// if direct pasting a particular native format allow it
	if (*lpcfFormat == CF_TEXT ||
		*lpcfFormat == WindowlessRichEdit::s_cfRichTextFormat ||
		*lpcfFormat == WindowlessRichEdit::s_cfRichTextAndObjects)
	{
		return S_OK;
	}

	// if format is 0, then force particular formats if available
	// ?? TODO: �������ק������Ҳ����IsClipboardFormatAvailable�������?
	if (*lpcfFormat == 0)
	{
		if (IsClipboardFormatAvailable((CLIPFORMAT)WindowlessRichEdit::s_cfRichTextAndObjects)) // native avail, let richedit do as it wants
		{
			*lpcfFormat = (CLIPFORMAT)WindowlessRichEdit::s_cfRichTextAndObjects;
			return S_OK;
		}
		else if (IsClipboardFormatAvailable((CLIPFORMAT)WindowlessRichEdit::s_cfRichTextFormat))
		{
			*lpcfFormat = (CLIPFORMAT)WindowlessRichEdit::s_cfRichTextFormat;
			return S_OK;
		}
	}

	// paste OLE formats
	if (SUCCEEDED(DoPaste(lpdataobj, fReally, lpcfFormat)))
		return S_FALSE;  // A return of a successful SCODE other than S_OK means that the callback either checked the data itself (if fReally is FALSE) or imported the data itself (if fReally is TRUE).

	// �����һ���ı���ʽ
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		*lpcfFormat = CF_TEXT;
		return S_OK;
	}

	return E_FAIL;
}
HRESULT __stdcall WindowlessRichEdit::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}
// ��richedit��ʹ�� CTRL+C/��ק ʱ�����ã���ȡҪ���Ƶ�����
// This method returns success status. If the SCODE of the return value is E_NOTIMPL, the 
// rich edit control creates its own clipboard object. 
// If the SCODE of the return value is a failure other than E_NOTIMPL, the operation fails.
//
// ֮��richedit�ڲ��������OleSetClipboard, SetClipboardDataObject (��IDataObject AddRef)
//
// WCH_EMBEDDING
// 
HRESULT __stdcall WindowlessRichEdit::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
	WORD wSelType = this->GetSelectionType();
	if (SEL_OBJECT == wSelType)
	{
		IRichEditOleObjectItem* pItem = this->GetSelectionOleObject();
		if (!pItem)
			return S_OK;

		if (FAILED(pItem->GetClipboardData(lpchrg, reco, lplpdataobj)))
		{
			// ����Ĭ�ϵķ������ڲ���ջΪ��
			// 0. COM����ģ�     ATL::IOleObjectImpl<XXX>::GetClipboardData <-- Ĭ��ATL�ڲ�û��ʵ��
			// 1. riched20.dll��: CDataTransferObj::Create()
			// 2. riched20.dll��: CLightDTEngine::RangeToDataObject()
			// 3. riched20.dll��: CTxtEdit::GetClipboardData()
			// 4. UIDLL.dll��:    ����m_spOle->GetClipboardData
			m_spOle->GetClipboardData(lpchrg,reco,lplpdataobj);
			return S_OK;
		}

		return S_OK;   // ע�����SEL_OBJECT == wSelTypeʱ���ز���S_OK������ڴ�pItem��AddRef��������ڴ�й¶.ʲôԭ��?/
		               // ò������ΪΪIDataObject�ŵ�clipboard��������һ�����ü���������ȥ����һ����������ʱ�����ü����ͻ�����

	}
	return E_NOTIMPL;
}

// ��richedit��ʹ�������קʱ������
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

//#include "E:\\���\\workingpath\\test\\richeditole\\richeditole_i.h"
#include "tom.h"

// �һ�RichEditʱ�����ã���������Ҽ�ʱ���������Ķ���Ĳ�ͬ���õ��Ĳ���Ҳ��ͬ��
// �����ڿհ״��һ���seltype=0, lpoleobj=NULL
// ������һ��COM�����һ�������seltype=2, lpoleobj = xxx;
HRESULT __stdcall WindowlessRichEdit::GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu)
{
#ifdef _DEBUG
	HMENU& hMenu = *lphmenu;
	TCHAR szInfo[128] = _T("");
	_stprintf(szInfo, _T("GetContextMenu Args: seltype=%d, lpoleobj=%08x, lpchrg=%d,%d"),
		seltype, lpoleobj, lpchrg->cpMin, lpchrg->cpMax);

	hMenu = CreatePopupMenu();
	BOOL bRet = ::AppendMenu(hMenu, MF_STRING, 10001, szInfo);
    POINT pt;
    GetCursorPos(&pt);
    ::TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, 0, m_hParentWnd, NULL);
    DestroyMenu(hMenu);
#endif
	return S_OK;
}

void  WindowlessRichEdit::SetSel(int nPos, int nLen)
{
    if (m_spTextServices)
    {
        CHARRANGE range = {nPos, nPos+nLen};
        LRESULT lr = 0;
        m_spTextServices->TxSendMessage(EM_EXSETSEL, NULL, (LPARAM)&range, &lr);
    }
}
void  WindowlessRichEdit::GetSel(int* pnPos, int* pnLen)
{
    if (m_spTextServices)
    {
        CHARRANGE range ={0, 0};
        LRESULT lr = 0;
        m_spTextServices->TxSendMessage(EM_EXGETSEL, NULL, (LPARAM)&range, &lr);

        if (pnPos)
            *pnPos = range.cpMin;
        if (pnLen)
            *pnLen = range.cpMin+range.cpMax;
    }
}
#pragma endregion