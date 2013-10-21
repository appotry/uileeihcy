#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include "UISDK\Control\Src\Control\Caret\caret.h"

#pragma comment(lib, "Riched20.lib")

//  RichEdit及其Callback的实现，可以参考MFC CRichEditView
//  RichEidt的ole view的实现，可以参考ATL插入一个控件对象的实现代码


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
//      这应该也是刷新区域未处理的原因
//
//  Q13.如何显示滚动条？
//      在TxGetScrollBars中，返回你的m_dwStyle样式。因此需要在m_dwStyle中指定
//      WM_HSCROLL,WM_VSCROLL...
//
//  Q14.OLE的生命周期.
//      因为richedit的undo功能，对象会一直保存在内存中，直到richedit关闭。????
//
//  Q15.Rich Edit 剪贴板操作 
//
//		Rich Edit控件注册两种剪贴板格式："富文本格式(RTF)"和一种叫做"RichEdit 文本与对象"的格式。应用程序
//      可以使用RegisterClipboardFormat函数来注册这些格式，其取值为CF_RTF与CF_RETEXTOBJ
//
//   Q16.IOleRichEditCallBack中的QueryAcceptData和QueryInsertData有什么区别?
//       
//      QueryInsertData针对调用了richedit的InsertObject插入一个OLE时触发。
//      QueryAcceptData是在粘贴和拖拽时将会触发。
//
//   Q17.如果提高richedit的绘制效率
//       1. 注意txinvalidaterect中的第一个参数，在输入文字时，prc会只是一行的区域，而不是整个client rect.
//          在分层窗口上绘制时可以根据这个进行剪裁刷新
//       2. 针对GIF动画，增加一个限制，一秒之内只刷新N次。可参考m_nLastTimerKickCount变量
//
//   Q18.回车时滚动条闪烁怎么回事？
//       详见WindowlessRichEdit::TxSetScrollPos函数注释
//
//   Q19. 怎么隐藏显示windowless richedit
//      
//   Q20. 处理WM_IME_COMPOSITION这个消息会导致输入法输入一个字按回车后居然显示两个字
//        是因为在对话框窗口中返回了一个0，导致对话框会再去调用默认的dialog proc，导致多发出来一个WM_CHAR消息

namespace UI
{

class  RichEdit;
class  IRichEditOleObjectItem;
class  RichEditOleObjectManager;

class ITextHostImpl : public ITextHost
{
public:
	ITextHostImpl();
	~ITextHostImpl();

	// ITextHost Interface
	virtual HDC 		TxGetDC();
	virtual INT			TxReleaseDC(HDC hdc);
// 	virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);
// 	virtual BOOL 		TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
// 	virtual BOOL 		TxSetScrollRange(INT fnBar,LONG nMinPos,INT nMaxPos,BOOL fRedraw);
// 	virtual BOOL 		TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
//	virtual void		TxInvalidateRect(LPCRECT prc, BOOL fMode);
	virtual void 		TxViewChange(BOOL fUpdate);
//	virtual BOOL		TxShowCaret(BOOL fShow);
// 	virtual BOOL 		TxSetTimer(UINT idTimer, UINT uTimeout);
// 	virtual void 		TxKillTimer(UINT idTimer);
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
	// Far East Methods for getting the Input Context
	//#ifdef WIN95_IME
	virtual HIMC		TxImmGetContext();
	virtual void		TxImmReleaseContext( HIMC himc );
	//#endif
	virtual HRESULT		TxGetSelectionBarWidth (LONG *lSelBarWidth);

	// 外部设置方法 （部分参考microsoft windowlessRE工程）
	bool  IsPasswordMode() { return m_fPassword; }
	void  SetPasswordMode(bool b);
	WCHAR SetPasswordChar(WCHAR chPasswordChar);
	LONG  SetAccelPos(LONG l_accelpos);
    bool  SetCharFormatByLogFont(LOGFONT* plf);
	bool  SetCharFormat(CHARFORMAT2* pcf);
    void  GetCharFormat(CHARFORMAT2* pcf);
	void  InitDefaultParaFormat();
	bool  IsWordWrap() { return m_fWordWrap; }
	void  SetWordWrap(bool fWordWrap);
	bool  IsReadOnly() { return (m_dwStyle & ES_READONLY) != 0; }
	void  SetReadOnly(bool fReadOnly);
	DWORD GetMaxLength() { return m_dwMaxLength; }
	void  SetMaxLegnth(DWORD dw);
	LONG  GetSelBarWidth();
	LONG  SetSelBarWidth(LONG l_SelBarWidth);
	bool  GetRichTextFlag() { return m_fRich; }
	void  SetRichTextFlag(bool b);
    void  SetMultiLine(bool b);
    bool  IsMultiLine();
	void  RevokeDragDrop(void);
	void  RegisterDragDrop(void);

	IRichEditOle* GetRichEditOle() { return m_spOle; }
	
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

	HWND   m_hParentWnd; // 所在的窗口句柄
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

class  WindowlessRichEdit : public ITextHostImpl
	                           , public ITextEditControl
	                           , public IRichEditOleCallback
{
public:
	WindowlessRichEdit();
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// 处理从父窗口中转发过来的消息

	BEGIN_MSG_MAP_EX(WindowlessRichEdit)
		PRE_HANDLE_MSG()
		MSG_WM_SETCURSOR( OnSetCursor )
		MSG_WM_KILLFOCUS( OnKillFocus )
        MSG_WM_SETFOCUS( OnSetFocus )
		MSG_WM_WINDOWPOSCHANGED( OnWindowPosChanged )
        MESSAGE_HANDLER_EX( WM_LBUTTONDOWN, OnLButtonDown)

		MESSAGE_HANDLER_EX( WM_KEYDOWN,  OnDefaultHandle )
		MESSAGE_HANDLER_EX( WM_CHAR,     OnChar )
		MESSAGE_RANGE_HANDLER_EX( WM_MOUSEFIRST, WM_MOUSELAST, OnDefaultHandle )
		MESSAGE_HANDLER_EX( WM_VSCROLL,  OnDefaultHandle )
		MESSAGE_HANDLER_EX( WM_HSCROLL,  OnDefaultHandle )

        MESSAGE_HANDLER_EX( WM_IME_STARTCOMPOSITION, OnDefaultHandle )  // 不让RE处理这个消息会导致输入法的窗口不跟随
        MESSAGE_HANDLER_EX( WM_IME_ENDCOMPOSITION,   OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_COMPOSITION,      OnDefaultHandle )  
        MESSAGE_HANDLER_EX( WM_IME_SETCONTEXT,       OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_NOTIFY,           OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_CONTROL,          OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_COMPOSITIONFULL,  OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_SELECT,           OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_CHAR,             OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_REQUEST,          OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_KEYDOWN,          OnDefaultHandle )
        MESSAGE_HANDLER_EX( WM_IME_KEYUP,            OnDefaultHandle )

		MESSAGE_HANDLER_EX( WM_TIMER,    OnTimer )
//		POST_HANDLE_MSG()		
	END_MSG_MAP()

protected:
	LRESULT OnPreHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT OnPostHandleMsg( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	BOOL    OnSetCursor(HWND wnd, UINT nHitTest, UINT message);
	LRESULT OnDefaultHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnKillFocus(HWND wndFocus);
    void    OnSetFocus(HWND wndOld);
	void    OnObjectPosChanged(UINT fwSide, LPRECT pRect);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnWindowPosChanged(LPWINDOWPOS);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool    DoGifOleUpdateRequst();

protected:
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

public:

	// Function
	void  BindControl(RichEdit* p);
	bool  Create(HWND hWndParent);
	void  Draw(HDC);
	bool  GetInvalidateRect(RECT* prc, bool* pbNeedRedrawScrollbar, bool bClear=true);
	bool  HitTest(POINT pt);

    // 基本操作
    bool  SetText(const TCHAR* szText);
    int   GetText(TCHAR* szBuf, int nLen);
    void  GetTextW(IBuffer** ppBuffer);
    int   GetTextLengthW();
    bool  GetEncodeTextW(IBuffer** ppBuffer);
    bool  AppendText(const TCHAR* szText, int nSize=-1);
    bool  AppendEncodeTextW(const TCHAR* szText, int nSize);
    bool  ReplaceSelectionText(const TCHAR* szText, int nSize);

    void  SetSel(int nPos, int nLen);
    void  GetSel(int* pnPos, int* pnLen);

    // event mask
    long  GetEventMask();
    void  SetEventMask(long n);
    void  ModifyEventMask(long nAdd, long nRemove);
    void  SetAutoResize(bool b, int nMaxSize = 0);
	
    // Ole操作
    WORD  GetSelectionType() const;
    IRichEditOleObjectItem*  GetSelectionOleObject();
    IRichEditOleObjectItem*  GetOleObjectByCharPos(int ncp);

    bool  InsertOleObject(IRichEditOleObjectItem* pItem);
    bool  InsertGif(const TCHAR* szGifPath);
    bool  InsertSkinGif(const TCHAR* szGifId);
    bool  InsertComObject(REFCLSID clsid);

    // Call this function to paste the OLE item in dataobj into this rich edit document/view.
    HRESULT DoPaste(LPDATAOBJECT pDataObject, BOOL fReally, CLIPFORMAT* pOutClipFormat);

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
    virtual BOOL 	TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void 	TxKillTimer(UINT idTimer);
    virtual BOOL	TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL	TxSetCaretPos(INT x, INT y);
    virtual BOOL    TxShowCaret(BOOL fShow);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);
protected:
	void    ClearOleObjects();
    HRESULT DefTxNotify(DWORD iNotify, void* pv);
    void    OnRequestResize(RECT* prc);

    // Creation IDataObject from IDataObject (used for drag-drop, paste)
    IOleObject*  CreateOleObjectFromData(LPDATAOBJECT pDataObject, bool bOleCreateFromDataOrOleCreateStaticFromData = true, OLERENDER render = OLERENDER_DRAW, CLIPFORMAT cfFormat = 0, LPFORMATETC lpFormatEtc = NULL);
    int     GetObjectTypeByOleObject(LPOLEOBJECT pOleObject); 

protected:
	RichEdit*   m_pRichEdit;
	RichEditOleObjectManager*  m_pOleMgr;

	int    m_nLastTimerKickCount; // 刷新FPS限制。这里还需要考虑下优化：即使只有一个GIF，也需要整个client rect刷新吗
	
	RECT   m_rcInvalidate;        // TxInvalidateRect辅助变量。无效区域的集合
	bool   m_bNeedRedrawScrollbar;// 是否需要刷新滚动条区域
    bool   m_bDuringTxDraw;       // 标志当前是否正在调用TxDraw。在这个期间的光标操作（针对自绘类型）都不应该再去刷新RE了，否则死循环
    bool   m_bFocus;

    int    m_nAutoResizeMaxSize;  // 自适应大小时的最大值。singleline对应width,multiline对应height。小于0表示无限制

protected:
	// 非windowless richedit要调用的初始化函数
	static LPCTSTR  GetLibraryName();
	void   InitRichEidtDll();
	void   ReleaseRichEidtDll();
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
public:
	//RichEdit formats
	static UINT     s_cfRichTextFormat;      // CLIPFORMAT
	static UINT     s_cfRichTextAndObjects;  // CLIPFORMAT
	static UINT     s_cfRichEditOleFormat;   // ole 剪粘板格式
	static UINT     s_cfUnicodeRichEditOleFormat; // ole 剪粘板格式
};

}
