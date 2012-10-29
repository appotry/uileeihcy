#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include "CaretWindow.h"

#pragma comment(lib, "Riched20.lib")
class   RichEditBase;

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
//  Q10.为什么我向textservice发送SETFOCUS消息后，却不会响应TxCreateCaret消息
//		a. 因为没有调用OnTxInPlaceActivate
//
//	Q11.在控件位置改变后，怎么去同步光标的位置？
//      a. 向txt service发送一个通知：m_spTextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
//
//  Q12.为什么拖拽时，刷新会出现异常？先添加后删除，但是分开刷新的
//
//  Q13.如何显示滚动条？
//      在TxGetScrollBars中，返回你的m_dwStyle样式。因此需要在m_dwStyle中指定
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

	// 外部设置方法 （部分参考microsoft windowlessRE工程）
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
// 	SIZE    m_sizeExtent;        // text service 用来实现缩放的参数。Each HIMETRIC unit corresponds to 0.01 millimeter.
	int     m_nxPerInch;
	int     m_nyPerInch;    
	LONG	m_laccelpos;         // Accelerator position

	// 已知属性
	DWORD   m_dwStyle;           // 编辑框样式
	WCHAR	m_chPasswordChar;    // Password character, TODO: 该接口未测试过
	DWORD   m_dwMaxLength;       // 最大输入内容长度
	LONG    m_lSelBarWidth;      // 类似于VS的左侧（显示行数字），专门用于点击选中一行的东东
	
	unsigned	  m_fWordWrap:1; // Whether control should word wrap
	unsigned	  m_fRich:1;     // Whether control is rich text
	unsigned	  m_fRegisteredForDrop:1; // whether host has registered for drop
	unsigned      m_fPassword:1; // 

	CHARFORMAT2W  m_cf;          // Default character format
	PARAFORMAT    m_pf;          // Default paragraph format

	//  其它资源、数据
	CComPtr<ITextServices>  m_spTextServices;
	CComPtr<IRichEditOle>   m_spOle;

	HWND          m_hParentWnd; // 所在的窗口句柄
	CCaret        m_caret;      // 光标（集成了系统光标和分层窗口模拟光标两套）
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

class UIAPI WindowlessRichEdit : public ITextHostImpl, public ITextEditControl,
	                             public IRichEditOleCallbackImpl
{
public:
	WindowlessRichEdit(RichEditBase*);
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// 处理从父窗口中转发过来的消息

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
	// 需要根据控件属性进行定制的接口放在这里实现，其它接口接口放在ITextHostImpl中实现
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
	// 非windowless richedit要调用的初始化函数
	static LPCTSTR  GetLibraryName();
	void   InitRichEidtDll();
	void   ReleaseRichEidtDll();
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
};

//
// http://blog.csdn.net/tangaowen/article/details/6198238
//
// 存储ole对象的相关的信息
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

	int     nPos;    // ole对象在richedit中的位置nPos. 处理复制的时候，需要通过这个来判断，复制的是否是文字还是ole对象
	int     nIndex;  // 
	TCHAR*  szPath;  // 文件路径

};
// 管理richedit中的 ole对象对应的结构体列表
class OleObjectManager
{
public:

	// 这个方法传入一个位置nPos,如果这个位置是一个ole obj ,那么返回这个ole object对应的vector中的OleStruct对象，否则返回NULL.
	// 在处理复制的时候，就调用这个方法，来将所有的ole obj的数据，替换为编码过的OleStruct对象的数据。然后在处理粘贴的时候，又解码，将对应的Ole object对象插入到
	// RichEdit中。
	OleObjectInfo*  GetObjectByPos(int nPos)
	{
		return NULL;
	}

protected:
	vector<OleObjectInfo*>  m_vecOleObject;
};

#include <atlctl.h>

// http://www.cnblogs.com/wlwel/archive/2012/08/05/2623899.html
// 引用原文：根据我的调查（呃，通过实践，通过QueryInterface观察），我发现实现一个
// richedit中的动画控件只需要实现二个接口：IOleObject、IViewObject2，前者为了融入
// 到richedit环境中，后者为了渲染显示。由于richedit默认只喜好无窗口模式，所以针对
// IOleInPlaceSiteWindowless之类的，你去实现意义也不大，因为人家容器不认你，当然还
// 有IPersist系列接口，对于标准的环境有用（比如IDE），但这里并不是很需要，所以认清
// 核心问题能减少很多困惑。更显然的是我的控件没有用ATL框架，因为此控件脱离了richedit
// 环境生存的意义也不大，更有甚者我没必要让使其成为标准（也没可能），仅仅是为了在
// 一个系统中的richedit中更好地展示。实现的接口越少，引入的麻烦越少，这样才能使精力
// 集中在主要问题上。
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