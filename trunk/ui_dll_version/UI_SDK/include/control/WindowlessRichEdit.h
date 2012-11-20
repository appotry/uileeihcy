#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include "CaretWindow.h"
#include "richeditolemgr.h"

#pragma comment(lib, "Riched20.lib")
class   RichEditBase;

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
//
//  Q13.如何显示滚动条？
//      在TxGetScrollBars中，返回你的m_dwStyle样式。因此需要在m_dwStyle中指定
//      WM_HSCROLL,WM_VSCROLL...
//
//  Q14.OLE的生命周期.
//      因为richedit的undo功能，对象会一直保存在内存中，直到richedit关闭。
//
//      1. WM_KEYDOWN delete键删除对象时，IRichEditOleCallback::DeleteObject。这里不能调用Release
// 			>	UIDLL.dll!IRichEditOleCallbackImpl<WindowlessRichEdit>::DeleteObject(IOleObject * lpoleobj=0x0011fc90)  行251	C++
// 				riched20.dll!CObjectMgr::ReplaceRange()  + 0x9a 字节	
// 				riched20.dll!CRchTxtPtr::ReplaceRange()  + 0x18dde 字节	
// 				riched20.dll!CTxtRange::ReplaceRange()  + 0x80 字节	
// 				riched20.dll!CTxtSelection::ReplaceRange()  + 0x1f9 字节	
// 				riched20.dll!CTxtSelection::Backspace()  + 0xc0 字节	
// 				riched20.dll!CTxtEdit::OnTxKeyDown()  + 0x5d4 字节	
// 				riched20.dll!CTxtEdit::TxSendMessage()  + 0x21f3 字节	
// 				UIDLL.dll!WindowlessRichEdit::OnPostHandleMsg(HWND__ * hWnd=0x00220428, unsigned int Msg=256, unsigned int wParam=8, long lParam=917505)  行196 + 0x39 字节	C++
//
//      2. DeleteObject之后，接着会响应IOleObject::Close。 这里不能调用Release
//         将一个OLE对象用delete键给删除掉时，会调用。然后将对象用ctrl+z再还原，然后再删除时，又会调用一次
//
// 			>	UIDLL.dll!CGifOleObject::Close(unsigned long dwSaveOption=0)  行50	C++
// 				riched20.dll!COleObject::Close()  + 0x2d 字节	
// 				riched20.dll!CReplaceObjectAE::OnCommit()  + 0xa 字节	
// 				riched20.dll!CommitAEList()  + 0x21 字节	
// 				riched20.dll!CGenUndoBuilder::Done()  + 0xf276 字节	
// 				riched20.dll!CTxtEdit::TxSendMessage()  + 0x11b 字节	
// 				UIDLL.dll!WindowlessRichEdit::OnPostHandleMsg(HWND__ * hWnd=0x001504fc, unsigned int Msg=256, unsigned int wParam=8, long lParam=917505)  行196 + 0x39 字节	C++
//
//      3. 销毁. 在ole对象已不在undo范围时，richedit才会将该对象release掉，此时CGifOleObject被释放
//         堆栈如下：
//           >	UIDLL.dll!CGifOleObject::~CGifOleObject()  行13	C++
// 				UIDLL.dll!CGifOleObject::`scalar deleting destructor'()  + 0x2b 字节	C++
// 				UIDLL.dll!CGifOleObject::Release()  行66 + 0x2b 字节	C++
// 				riched20.dll!SafeReleaseAndNULL()  + 0x17 字节	
// 				riched20.dll!COleObject::DisconnectObject()  + 0x95 字节	
// 				riched20.dll!COleObject::CleanupState()  + 0x33 字节	
// 				riched20.dll!COleObject::MakeZombie()  + 0xa 字节	
// 				riched20.dll!CReplaceObjectAE::Destroy()  + 0x1a 字节	
// 				riched20.dll!DestroyAEList()  + 0x1d 字节	
// 				riched20.dll!CUndoStack::ClearAll()  + 0x2c 字节	
// 				riched20.dll!CGenUndoBuilder::Done()  + 0x1c56d 字节	
// 				riched20.dll!CTxtEdit::TxSendMessage()  + 0x11b 字节	
// 				UIDLL.dll!WindowlessRichEdit::OnChar(unsigned int uMsg=258, unsigned int wParam=97, long lParam=1966081)  行297 + 0x39 字节	C++
//
//   Q15.  Rich Edit 剪贴板操作 
//      http://zjqzy03080312.blog.163.com/blog/static/1857428072012124112714348/
//
//		应用程序可以粘贴剪贴板中内容到一个Rich Edit控件中，采用最佳可用剪贴板格式或者指定的剪贴板格式。
//      你也可以决定是否一个Rich Edit控件可以粘贴某种剪贴板格式。 
//
//		对于一个Edit控件而言，你可以使用WM_COPY或者WM_CUT消息来拷贝或者剪切当前选中内容。同样的，你可
//      以使用WM_PASTE消息将这些剪贴板内容粘贴到一个Rich Edit控件中。控件将粘贴它所识别的第一个可用格式，
//      这大概是最具描述性的格式。 
//
//		你可以使用EM_PASTESPECIAL消息来粘贴指定的剪贴板格式。这个消息对具有"特殊粘贴"命令的应用程序很有
//      用，该命令可以让用户选择剪贴板格式。你可以使用EM_CANPASTE消息来决定控件是否识别某种指定的格式。 
//
//		你也可以使用EM_CANPASTE消息来决定Rich Edit控件是否识别所有可用的剪贴板格式。该消息在处理WM_INITMENUPOPUP
//      消息时很有用。应用程序可以启用或者屏蔽"粘贴"命令，取决于控件是否可以粘贴任一个可用类型。 
//
//		Rich Edit控件注册两种剪贴板格式："富文本格式(RTF)"和一种叫做"RichEdit 文本与对象"的格式。应用程序
//      可以使用RegisterClipboardFormat函数来注册这些格式，其取值为CF_RTF与CF_RETEXTOBJ
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

	HWND          m_hParentWnd; // 所在的窗口句柄
	CCaret        m_caret;      // 光标（集成了系统光标和分层窗口模拟光标两套）
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

class UIAPI WindowlessRichEdit : public ITextHostImpl, 
	                             public ITextEditControl
	                           , public IRichEditOleCallback
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
	// Helper 
	WORD GetSelectionType() const
	{
		LRESULT lr = 0;
		m_spTextServices->TxSendMessage(EM_SELECTIONTYPE, 0, 0L, &lr);
		return (WORD)lr;
	}
	bool GetSelectionOleObject(RichEditOleObjectItem** ppItem);

	// Function
	bool    Create(HWND hWndParent);
	void    Draw(HDC);
	bool    HitTest(POINT pt);

	// 
	bool    InsertOleObject(RichEditOleObjectItem* pItem);
	bool    InsertGif(const TCHAR* szGifPath);

	// Call this function to paste the OLE item in dataobj into this rich edit document/view.
	void    DoPaste(LPDATAOBJECT pDataObject, CLIPFORMAT cf, HMETAFILEPICT hMetaPict);

protected:
	
	// Creation IDataObject from IDataObject (used for drag-drop, paste)
	IOleObject*  CreateOleObjectFromData(LPDATAOBJECT pDataObject, bool bOleCreateFromDataOrOleCreateStaticFromData = true, OLERENDER render = OLERENDER_DRAW, CLIPFORMAT cfFormat = 0, LPFORMATETC lpFormatEtc = NULL);
	int     GetObjectTypeByOleObject(LPOLEOBJECT pOleObject); 

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
	void    ClearOleObjects();

protected:
	RichEditBase*   m_pRichEditBase;
	RichEditOleObjectManager  m_olemgr;


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
};


//#include <atlctl.h>
