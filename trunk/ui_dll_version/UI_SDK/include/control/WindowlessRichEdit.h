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
	
	bool    InsertGif(const TCHAR* szGifPath);

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


class UIAPI IRichEditOleCallbackImpl : public IRichEditOleCallback
{
public:
	IRichEditOleCallbackImpl(){ m_hRichEditWnd = NULL; }

	// *** IUnknown methods ***
// 	virtual HRESULT __stdcall QueryInterface(REFIID riid, LPVOID FAR * lplpObj) { return E_NOTIMPL; }
// 	virtual ULONG   __stdcall AddRef() { return 1; }
// 	virtual ULONG   __stdcall Release(){ return 0; }

	// *** IRichEditOleCallback methods ***
	virtual HRESULT __stdcall GetNewStorage(LPSTORAGE FAR * lplpstg);
	virtual HRESULT __stdcall GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame, LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo) ;
	virtual HRESULT __stdcall ShowContainerUI(BOOL fShow) ;
	virtual HRESULT __stdcall QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp) ;
	virtual HRESULT __stdcall DeleteObject(LPOLEOBJECT lpoleobj) ;
	virtual HRESULT __stdcall QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict) ;
	virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) ;
	virtual HRESULT __stdcall GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj) ;
	virtual HRESULT __stdcall GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect) ;
	virtual HRESULT __stdcall GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu) ;

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

class UIAPI WindowlessRichEdit : public ITextHostImpl, public ITextEditControl,
	                             public IRichEditOleCallbackImpl
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

#include <atlctl.h>

// http://www.cnblogs.com/wlwel/archive/2012/08/05/2623899.html
// ����ԭ�ģ������ҵĵ��飨����ͨ��ʵ����ͨ��QueryInterface�۲죩���ҷ���ʵ��һ��
// richedit�еĶ����ؼ�ֻ��Ҫʵ�ֶ����ӿڣ�IOleObject��IViewObject2��ǰ��Ϊ������
// ��richedit�����У�����Ϊ����Ⱦ��ʾ������richeditĬ��ֻϲ���޴���ģʽ���������
// IOleInPlaceSiteWindowless֮��ģ���ȥʵ������Ҳ������Ϊ�˼����������㣬��Ȼ��
// ��IPersistϵ�нӿڣ����ڱ�׼�Ļ������ã�����IDE���������ﲢ���Ǻ���Ҫ����������
// ���������ܼ��ٺܶ����󡣸���Ȼ�����ҵĿؼ�û����ATL��ܣ���Ϊ�˿ؼ�������richedit
// �������������Ҳ���󣬸���������û��Ҫ��ʹ���Ϊ��׼��Ҳû���ܣ���������Ϊ����
// һ��ϵͳ�е�richedit�и��õ�չʾ��ʵ�ֵĽӿ�Խ�٣�������鷳Խ�٣���������ʹ����
// ��������Ҫ�����ϡ�
class GifObject : public IOleObject, public IViewObject2
{
public:
	//////////////////////////////////////////////////////////////////////////

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (::IsEqualIID(riid, IID_IUnknown) || ::IsEqualIID(riid, IID_IOleObject))
		{
			*ppvObject = this;
		}
		else if (::IsEqualIID(riid, IID_IViewObject))
		{
			*ppvObject = static_cast<IViewObject*>(this);
		}
		else if (::IsEqualIID(riid, IID_IViewObject2))
		{
			*ppvObject = static_cast<IViewObject2*>(this);
		}
		else
		{
			return E_NOINTERFACE;
		}

		this->AddRef();
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void) 
	{
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release( void) 
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
#pragma region // ole object 
	virtual HRESULT STDMETHODCALLTYPE SetClientSite( 
		/* [unique][in] */ __RPC__in_opt IOleClientSite *pClientSite)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetClientSite( 
		/* [out] */ __RPC__deref_out_opt IOleClientSite **ppClientSite) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE SetHostNames( 
		/* [in] */ __RPC__in LPCOLESTR szContainerApp,
		/* [unique][in] */ __RPC__in_opt LPCOLESTR szContainerObj) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Close( 
		/* [in] */ DWORD dwSaveOption) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE SetMoniker( 
		/* [in] */ DWORD dwWhichMoniker,
		/* [unique][in] */ __RPC__in_opt IMoniker *pmk)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetMoniker( 
		/* [in] */ DWORD dwAssign,
		/* [in] */ DWORD dwWhichMoniker,
		/* [out] */ __RPC__deref_out_opt IMoniker **ppmk) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE InitFromData( 
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObject,
		/* [in] */ BOOL fCreation,
		/* [in] */ DWORD dwReserved)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetClipboardData( 
		/* [in] */ DWORD dwReserved,
		/* [out] */ __RPC__deref_out_opt IDataObject **ppDataObject)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE DoVerb( 
		/* [in] */ LONG iVerb,
		/* [unique][in] */ __RPC__in_opt LPMSG lpmsg,
		/* [unique][in] */ __RPC__in_opt IOleClientSite *pActiveSite,
		/* [in] */ LONG lindex,
		/* [in] */ __RPC__in HWND hwndParent,
		/* [unique][in] */ __RPC__in_opt LPCRECT lprcPosRect) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumVerbs( 
		/* [out] */ __RPC__deref_out_opt IEnumOLEVERB **ppEnumOleVerb) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Update( void) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE IsUpToDate( void) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUserClassID( 
		/* [out] */ __RPC__out CLSID *pClsid) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUserType( 
		/* [in] */ DWORD dwFormOfType,
		/* [out] */ __RPC__deref_out_opt LPOLESTR *pszUserType) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE SetExtent( 
		/* [in] */ DWORD dwDrawAspect,
		/* [in] */ __RPC__in SIZEL *psizel) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetExtent( 
		/* [in] */ DWORD dwDrawAspect,
		/* [out] */ __RPC__out SIZEL *psizel) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Advise( 
		/* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink,
		/* [out] */ __RPC__out DWORD *pdwConnection) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Unadvise( 
		/* [in] */ DWORD dwConnection) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumAdvise( 
		/* [out] */ __RPC__deref_out_opt IEnumSTATDATA **ppenumAdvise) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetMiscStatus( 
		/* [in] */ DWORD dwAspect,
		/* [out] */ __RPC__out DWORD *pdwStatus) 
	{
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE SetColorScheme( 
		/* [in] */ __RPC__in LOGPALETTE *pLogpal)
	{
		return E_NOTIMPL;
	}
#pragma  endregion
	//////////////////////////////////////////////////////////////////////////

#pragma region // iviewobject

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Draw( 
		/* [in] */ DWORD dwDrawAspect,
		/* [in] */ LONG lindex,
		/* [unique][in] */ void *pvAspect,
		/* [unique][in] */ DVTARGETDEVICE *ptd,
		/* [in] */ HDC hdcTargetDev,
		/* [in] */ HDC hdcDraw,
		/* [in] */ LPCRECTL lprcBounds,
		/* [unique][in] */ LPCRECTL lprcWBounds,
		/* [in] */ BOOL ( STDMETHODCALLTYPE *pfnContinue )( 
		ULONG_PTR dwContinue),
		/* [in] */ ULONG_PTR dwContinue) 
	{
		return E_NOTIMPL;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetColorSet( 
		/* [in] */ DWORD dwDrawAspect,
		/* [in] */ LONG lindex,
		/* [unique][in] */ void *pvAspect,
		/* [unique][in] */ DVTARGETDEVICE *ptd,
		/* [in] */ HDC hicTargetDev,
		/* [out] */ LOGPALETTE **ppColorSet) 
	{
		return E_NOTIMPL;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Freeze( 
		/* [in] */ DWORD dwDrawAspect,
		/* [in] */ LONG lindex,
		/* [unique][in] */ void *pvAspect,
		/* [out] */ DWORD *pdwFreeze) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Unfreeze( 
		/* [in] */ DWORD dwFreeze) 
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE SetAdvise( 
		/* [in] */ DWORD aspects,
		/* [in] */ DWORD advf,
		/* [unique][in] */ __RPC__in_opt IAdviseSink *pAdvSink) 
	{
		return E_NOTIMPL;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetAdvise( 
		/* [unique][out] */ DWORD *pAspects,
		/* [unique][out] */ DWORD *pAdvf,
		/* [out] */ IAdviseSink **ppAdvSink)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetExtent( 
		/* [in] */ DWORD dwDrawAspect,
		/* [in] */ LONG lindex,
		/* [unique][in] */ __RPC__in_opt DVTARGETDEVICE *ptd,
		/* [out] */ __RPC__out LPSIZEL lpsizel) 
	{
		lpsizel->cx = lpsizel->cy = 20;
		return S_OK;
//		return E_NOTIMPL;
	}
#pragma endregion
	HRESULT LoadGif(const TCHAR*  szPath)
	{
		return E_NOTIMPL;
	}
};

/*
LPLOCKBYTES lpLockBytes = NULL;
SCODE sc;
HRESULT hr;
//print to RichEdit' s IClientSite
LPOLECLIENTSITE m_lpClientSite;
//A smart point to IAnimator
IGifAnimatorPtr    m_lpAnimator;
//ptr 2 storage    
LPSTORAGE m_lpStorage;
//the object 2 b insert 2
LPOLEOBJECT    m_lpObject;

//Create lockbytes
sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
if (sc != S_OK)
AfxThrowOleException(sc);
ASSERT(lpLockBytes != NULL);

//use lockbytes to create storage
sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
									STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &m_lpStorage);
if (sc != S_OK)
{
	VERIFY(lpLockBytes->Release() == 0);
	lpLockBytes = NULL;
	AfxThrowOleException(sc);
}
ASSERT(m_lpStorage != NULL);

//get the ClientSite of the very RichEditCtrl
GetIRichEditOle()->GetClientSite(&m_lpClientSite);
ASSERT(m_lpClientSite != NULL);

try
{
	//Initlize COM interface
	hr = ::CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	if( FAILED(hr) )
		_com_issue_error(hr);

	//Get GifAnimator object
	//here, I used a smart point, so I do not need to free it
	hr = m_lpAnimator.CreateInstance(CLSID_GifAnimator);    
	if( FAILED(hr) )
		_com_issue_error(hr);
	//COM operation need BSTR, so get a BSTR
	BSTR path = strPicPath.AllocSysString();

	//Load the gif
	hr = m_lpAnimator->LoadFromFile(path);
	if( FAILED(hr) )
		_com_issue_error(hr);

	TRACE0( m_lpAnimator->GetFilePath() );

	//get the IOleObject
	hr = m_lpAnimator.QueryInterface(IID_IOleObject, (void**)&m_lpObject);
	if( FAILED(hr) )
		_com_issue_error(hr);

	//Set it 2 b inserted
	OleSetContainedObject(m_lpObject, TRUE);

	//2 insert in 2 richedit, you need a struct of REOBJECT
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));

	reobject.cbStruct = sizeof(REOBJECT);    
	CLSID clsid;
	sc = m_lpObject->GetUserClassID(&clsid);
	if (sc != S_OK)
		AfxThrowOleException(sc);
	//set clsid
	reobject.clsid = clsid;
	//can be selected
	reobject.cp = REO_CP_SELECTION;
	//content, but not static
	reobject.dvaspect = DVASPECT_CONTENT;
	//goes in the same line of text line
	reobject.dwFlags = REO_BELOWBASELINE; //REO_RESIZABLE |
	reobject.dwUser = 0;
	//the very object
	reobject.poleobj = m_lpObject;
	//client site contain the object
	reobject.polesite = m_lpClientSite;
	//the storage 
	reobject.pstg = m_lpStorage;

	SIZEL sizel;
	sizel.cx = sizel.cy = 0;
	reobject.sizel = sizel;
	HWND hWndRT = this->m_hWnd;

	//Sel all text
	//        ::SendMessage(hWndRT, EM_SETSEL, 0, -1);
	//        DWORD dwStart, dwEnd;
	//        ::SendMessage(hWndRT, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
	//        ::SendMessage(hWndRT, EM_SETSEL, dwEnd+1, dwEnd+1);

	//Insert after the line of text
	GetIRichEditOle()->InsertObject(&reobject);
	::SendMessage(hWndRT, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
	VARIANT_BOOL ret;
	//do frame changing
	ret = m_lpAnimator->TriggerFrameChange();
	//show it
	m_lpObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, m_lpClientSite, 0, 
		m_hWnd, NULL);
	m_lpObject->DoVerb(OLEIVERB_SHOW, NULL, m_lpClientSite, 0, m_hWnd, 
		NULL);

	//redraw the window to show animation
	RedrawWindow();

	if (m_lpClientSite)
	{
		m_lpClientSite->Release();
		m_lpClientSite = NULL;
	}
	if (m_lpObject)
	{
		m_lpObject->Release();
		m_lpObject = NULL;
	}
	if (m_lpStorage)
	{
		m_lpStorage->Release();
		m_lpStorage = NULL;
	}

	SysFreeString(path);
}
catch( _com_error e )
{
	AfxMessageBox(e.ErrorMessage());
	::CoUninitialize();    
}
*/