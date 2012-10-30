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
	// 例如向richedit中随便拖入一个桌面上的图标，就会调用该函数
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
	// 在从外部拖入一个文件到richedit时，先响应了GetNewStorage成功后，就会再调到这个接口函数
	// 当返回S_OK时，这个对象将被插入，返回FALSE时，对象将不会被插入
	HRESULT __stdcall QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg,
		LONG cp)
	{
		return S_OK;
	}
	// 例如将richedit中的一人COM对象删除，则会调用一次该接口函数
	// 例如将richedit中的一个COM对象用鼠标拖拽到另一个位置，则会调用一次该接口函数
	// 该函数仅是一个通知，告诉我们有一个对象要被deleted from rich edit control;
	HRESULT __stdcall DeleteObject(LPOLEOBJECT lpoleobj)
	{
		return S_OK;
	}

	// 在richedit中使用 CTRL+V时被调用
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
	// 在richedit中使用 CTRL+C 时被调用
	HRESULT __stdcall GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco,
		LPDATAOBJECT FAR * lplpdataobj)
	{
		return E_NOTIMPL;
	}

	// 在richedit中使用鼠标拖拽时被调用
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

	// 右击RichEdit时被调用，根据鼠标右键时，鼠标下面的对象的不同，得到的参数也不同，
	// 例如在空白处右击，seltype=0, lpoleobj=NULL
	// 例如在一个COM对象处右击，可能seltype=2, lpoleobj = xxx;
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

class UIAPI WindowlessRichEdit : public ITextHostImpl, 
	                             public ITextEditControl
	                           , public IRichEditOleCallbackImpl<WindowlessRichEdit>
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

	// 
	bool    InsertGif(const TCHAR* szGifPath);

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

	vector<IUnknown*>  m_vecpUnkOleObject;

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

// {2EAE75F5-D78F-43ca-811D-8F8B01CCE05B}
static const GUID IID_IGifOleObject = { 0x2eae75f5, 0xd78f, 0x43ca, { 0x81, 0x1d, 0x8f, 0x8b, 0x1, 0xcc, 0xe0, 0x5b } };
class IGifOleObject : public IOleObject
{
public:
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath) = 0;
	virtual HRESULT __stdcall Refresh() = 0;
};
