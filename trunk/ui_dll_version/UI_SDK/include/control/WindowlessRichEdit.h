#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
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
//	Q11.�ڿؼ�λ�øı����ôȥͬ������λ�ã�
//      a. ��txt service����һ��֪ͨ��m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
//
//  Q12.Ϊʲô��קʱ��ˢ�»�����쳣������Ӻ�ɾ�������Ƿֿ�ˢ�µ�
//
//  Q13.�����ʾ��������
//      ��TxGetScrollBars�У��������m_dwStyle��ʽ�������Ҫ��m_dwStyle��ָ��
//      WM_HSCROLL,WM_VSCROLL...
//
//
class UIAPI ITextHostImpl : public ITextHost
{
public:
	ITextHostImpl();
	~ITextHostImpl(){};

	// ITextHost Interface
	virtual HDC 		TxGetDC();
	virtual INT			TxReleaseDC(HDC hdc);
// 	virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);
// 	virtual BOOL 		TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
// 	virtual BOOL 		TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
// 	virtual BOOL 		TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
//	virtual void		TxInvalidateRect(LPCRECT prc, BOOL fMode);
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
//	virtual HRESULT		TxGetViewInset(LPRECT prc);
	virtual HRESULT 	TxGetCharFormat(const CHARFORMATW **ppCF );
	virtual HRESULT		TxGetParaFormat(const PARAFORMAT **ppPF);
	virtual COLORREF	TxGetSysColor(int nIndex);
//	virtual HRESULT		TxGetBackStyle(TXTBACKSTYLE *pstyle);
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

	// �ⲿ���÷��� �����ֲο�microsoft windowlessRE���̣�
	bool    IsPasswordMode() { return m_fPassword; }
	void    SetPasswordMode(bool b);
	WCHAR   SetPasswordChar(WCHAR chPasswordChar);
	LONG    SetAccelPos(LONG l_accelpos);
	bool    SetFont(LOGFONT* plf);
	void    InitDefaultParaFormat();
	bool    IsWordWrap() { return m_fWordWrap; }
	void    SetWordWrap(bool fWordWrap);
	bool    IsReadOnly() { return (m_dwStyle & ES_READONLY) != 0; }
	void    SetReadOnly(bool fReadOnly);
	DWORD   GetMaxLength() { return m_dwMaxLength; }
	void    SetMaxLegnth(DWORD dw);
	LONG    GetSelBarWidth();
	LONG    SetSelBarWidth(LONG l_SelBarWidth);
	bool    GetRichTextFlag() { return m_fRich; }
	void    SetRichTextFlag(bool b);
	void    RevokeDragDrop(void);
	void    RegisterDragDrop(void);
	bool    SetText(const TCHAR* szText);
	
protected:

	// unknown attribute
// 	SIZE    m_sizeExtent;        // text service ����ʵ�����ŵĲ�����Each HIMETRIC unit corresponds to 0.01 millimeter.
	int     m_nxPerInch;
	int     m_nyPerInch;    
	LONG	m_laccelpos;         // Accelerator position

	// ��֪����
	DWORD   m_dwStyle;           // �༭����ʽ
	WCHAR	m_chPasswordChar;    // Password character, TODO: �ýӿ�δ���Թ�
	DWORD   m_dwMaxLength;       // ����������ݳ���
	LONG    m_lSelBarWidth;      // ������VS����ࣨ��ʾ�����֣���ר�����ڵ��ѡ��һ�еĶ���
	
	unsigned	  m_fWordWrap:1; // Whether control should word wrap
	unsigned	  m_fRich:1;     // Whether control is rich text
	unsigned	  m_fRegisteredForDrop:1; // whether host has registered for drop
	unsigned      m_fPassword:1; // 

	CHARFORMAT2W  m_cf;          // Default character format
	PARAFORMAT    m_pf;          // Default paragraph format

	//  ������Դ������
	CComPtr<ITextServices>  m_spTextServices;
	CComPtr<IRichEditOle>   m_spOle;

	HWND          m_hParentWnd; // ���ڵĴ��ھ��
	CCaret        m_caret;      // ��꣨������ϵͳ���ͷֲ㴰��ģ�������ף�
};

interface ITextEditControl
{
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;
};


template<class T>
class UIAPI IRichEditOleCallbackImpl : public IRichEditOleCallback
{
public:
	IRichEditOleCallbackImpl(){ m_hRichEditWnd = NULL; }

	// *** IUnknown methods ***
// 	virtual HRESULT __stdcall QueryInterface(REFIID riid, LPVOID FAR * lplpObj) { return E_NOTIMPL; }
// 	virtual ULONG   __stdcall AddRef() { return 1; }
// 	virtual ULONG   __stdcall Release(){ return 0; }

	// *** IRichEditOleCallback methods ***
// 	virtual HRESULT __stdcall GetNewStorage(LPSTORAGE FAR * lplpstg);
// 	virtual HRESULT __stdcall GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame, LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo) ;
// 	virtual HRESULT __stdcall ShowContainerUI(BOOL fShow) ;
// 	virtual HRESULT __stdcall QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) ;
// 	virtual HRESULT __stdcall DeleteObject(LPOLEOBJECT lpoleobj) ;
// 	virtual HRESULT __stdcall QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict) ;
// 	virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) ;
// 	virtual HRESULT __stdcall GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj) ;
// 	virtual HRESULT __stdcall GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect) ;
// 	virtual HRESULT __stdcall GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu) ;

	// This method must be implemented to allow cut, copy, paste, drag, 
	// and drop operations of Component Object Model (COM) objects.
	// ������richedit���������һ�������ϵ�ͼ�꣬�ͻ���øú���
	HRESULT __stdcall GetNewStorage(LPSTORAGE FAR * lplpstg)
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
	HRESULT __stdcall GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame,
		LPOLEINPLACEUIWINDOW FAR * lplpDoc,
		LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		return E_NOTIMPL;
	}
	HRESULT __stdcall ShowContainerUI(BOOL fShow)
	{
		return E_NOTIMPL;
	}
	// �ڴ��ⲿ����һ���ļ���richeditʱ������Ӧ��GetNewStorage�ɹ��󣬾ͻ��ٵ�������ӿں���
	// ������S_OKʱ��������󽫱����룬����FALSEʱ�����󽫲��ᱻ����
	HRESULT __stdcall QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg,
		LONG cp)
	{
		return S_OK;
	}
	// ���罫richedit�е�һ��COM����ɾ����������һ�θýӿں���
	// ���罫richedit�е�һ��COM�����������ק����һ��λ�ã�������һ�θýӿں���
	// �ú�������һ��֪ͨ������������һ������Ҫ��deleted from rich edit control;
	HRESULT __stdcall DeleteObject(LPOLEOBJECT lpoleobj)
	{
		lpoleobj->Release();
		return S_OK;
	}

	// ��richedit��ʹ�� CTRL+Vʱ������
	HRESULT __stdcall QueryAcceptData(LPDATAOBJECT lpdataobj,
		CLIPFORMAT FAR * lpcfFormat, DWORD reco,
		BOOL fReally, HGLOBAL hMetaPict)
	{
		return E_NOTIMPL;
	}
	HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode)
	{
		return E_NOTIMPL;
	}
	// ��richedit��ʹ�� CTRL+C ʱ������
	HRESULT __stdcall GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco,
		LPDATAOBJECT FAR * lplpdataobj)
	{
		return E_NOTIMPL;
	}

	// ��richedit��ʹ�������קʱ������
	HRESULT __stdcall GetDragDropEffect(BOOL fDrag, DWORD grfKeyState,
		LPDWORD pdwEffect)
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

	// �һ�RichEditʱ�����ã���������Ҽ�ʱ���������Ķ���Ĳ�ͬ���õ��Ĳ���Ҳ��ͬ��
	// �����ڿհ״��һ���seltype=0, lpoleobj=NULL
	// ������һ��COM�����һ�������seltype=2, lpoleobj = xxx;
	HRESULT __stdcall GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj,
		CHARRANGE FAR * lpchrg,
		HMENU FAR * lphmenu)
	{
#ifdef _DEBUG
		T* pThis = dynamic_cast<T*>(this);
		return pThis->InsertGif(_T("C:\\richedit.gir"));
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


	HWND  GetRichEidtHWND(){ return m_hRichEditWnd; }
	void  SetRichEditHWND(HWND hWnd){ m_hRichEditWnd = hWnd; }

private:
	HWND   m_hRichEditWnd;
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

class UIAPI WindowlessRichEdit : public ITextHostImpl, 
	                             public ITextEditControl
	                           , public IRichEditOleCallbackImpl<WindowlessRichEdit>
{
public:
	WindowlessRichEdit(RichEditBase*);
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// ����Ӹ�������ת����������Ϣ

	BEGIN_MSG_MAP_EX(WindowlessRichEdit)
		PRE_HANDLE_MSG()
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)

		MESSAGE_HANDLER_EX(WM_KEYDOWN,  OnDefaultHandle)
		MESSAGE_HANDLER_EX(WM_CHAR,     OnChar)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST, OnDefaultHandle)
		MESSAGE_HANDLER_EX(WM_SETFOCUS, OnDefaultHandle)
		MESSAGE_HANDLER_EX(WM_VSCROLL,  OnDefaultHandle)
		MESSAGE_HANDLER_EX(WM_HSCROLL,  OnDefaultHandle)
//		POST_HANDLE_MSG()		
	END_MSG_MAP()

protected:
	LRESULT OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	BOOL    OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	LRESULT OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnKillFocus(HWND wndFocus);
	void    OnObjectPosChanged(UINT fwSide, LPRECT pRect);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnWindowPosChanged(LPWINDOWPOS);

public:
	bool    Create(HWND hWndParent);
	void    Draw(HDC);
	bool    HitTest(POINT pt);

	// 
	bool    InsertGif(const TCHAR* szGifPath);

public:
	// IUnknown  Interface
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

	// ITextHost Interface
	// ��Ҫ���ݿؼ����Խ��ж��ƵĽӿڷ�������ʵ�֣������ӿڽӿڷ���ITextHostImpl��ʵ��
	virtual BOOL    TxShowScrollBar(INT fnBar, BOOL fShow);
	virtual BOOL    TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	virtual BOOL    TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
	virtual BOOL 	TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	virtual HRESULT TxGetClientRect(LPRECT prc);
	virtual void    TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
	virtual HRESULT	TxGetViewInset(LPRECT prc);

protected:
	RichEditBase*   m_pRichEditBase;

	vector<IUnknown*>  m_vecpUnkOleObject;

protected:
	// ��windowless richeditҪ���õĳ�ʼ������
	static LPCTSTR  GetLibraryName();
	void   InitRichEidtDll();
	void   ReleaseRichEidtDll();
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
};

//
// http://blog.csdn.net/tangaowen/article/details/6198238
//
// �洢ole�������ص���Ϣ
class OleObjectInfo
{
public:
	OleObjectInfo()
	{
		nPos = 0; nIndex = 0;
		szPath = NULL;
	}
	~OleObjectInfo()
	{
		SAFE_ARRAY_DELETE(szPath);
	}

	int     nPos;    // ole������richedit�е�λ��nPos. �����Ƶ�ʱ����Ҫͨ��������жϣ����Ƶ��Ƿ������ֻ���ole����
	int     nIndex;  // 
	TCHAR*  szPath;  // �ļ�·��

};
// ����richedit�е� ole�����Ӧ�Ľṹ���б�
class OleObjectManager
{
public:

	// �����������һ��λ��nPos,������λ����һ��ole obj ,��ô�������ole object��Ӧ��vector�е�OleStruct���󣬷��򷵻�NULL.
	// �ڴ����Ƶ�ʱ�򣬾͵�������������������е�ole obj�����ݣ��滻Ϊ�������OleStruct��������ݡ�Ȼ���ڴ���ճ����ʱ���ֽ��룬����Ӧ��Ole object������뵽
	// RichEdit�С�
	OleObjectInfo*  GetObjectByPos(int nPos)
	{
		return NULL;
	}

protected:
	vector<OleObjectInfo*>  m_vecOleObject;
};

//#include <atlctl.h>
