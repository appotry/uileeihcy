#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include "UISDK\Control\Src\Control\Caret\caret.h"

#pragma comment(lib, "Riched20.lib")

//  RichEdit����Callback��ʵ�֣����Բο�MFC CRichEditView
//  RichEidt��ole view��ʵ�֣����Բο�ATL����һ���ؼ������ʵ�ִ���


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
//      ��Ӧ��Ҳ��ˢ������δ�����ԭ��
//
//  Q13.�����ʾ��������
//      ��TxGetScrollBars�У��������m_dwStyle��ʽ�������Ҫ��m_dwStyle��ָ��
//      WM_HSCROLL,WM_VSCROLL...
//
//  Q14.OLE����������.
//      ��Ϊrichedit��undo���ܣ������һֱ�������ڴ��У�ֱ��richedit�رա�????
//
//  Q15.Rich Edit ��������� 
//
//		Rich Edit�ؼ�ע�����ּ������ʽ��"���ı���ʽ(RTF)"��һ�ֽ���"RichEdit �ı������"�ĸ�ʽ��Ӧ�ó���
//      ����ʹ��RegisterClipboardFormat������ע����Щ��ʽ����ȡֵΪCF_RTF��CF_RETEXTOBJ
//
//   Q16.IOleRichEditCallBack�е�QueryAcceptData��QueryInsertData��ʲô����?
//       
//      QueryInsertData��Ե�����richedit��InsertObject����һ��OLEʱ������
//      QueryAcceptData����ճ������קʱ���ᴥ����
//
//   Q17.������richedit�Ļ���Ч��
//       1. ע��txinvalidaterect�еĵ�һ������������������ʱ��prc��ֻ��һ�е����򣬶���������client rect.
//          �ڷֲ㴰���ϻ���ʱ���Ը���������м���ˢ��
//       2. ���GIF����������һ�����ƣ�һ��֮��ֻˢ��N�Ρ��ɲο�m_nLastTimerKickCount����
//
//   Q18.�س�ʱ��������˸��ô���£�
//       ���WindowlessRichEdit::TxSetScrollPos����ע��
//
//   Q19. ��ô������ʾwindowless richedit
//      
//   Q20. ����WM_IME_COMPOSITION�����Ϣ�ᵼ�����뷨����һ���ְ��س����Ȼ��ʾ������
//        ����Ϊ�ڶԻ��򴰿��з�����һ��0�����¶Ի������ȥ����Ĭ�ϵ�dialog proc�����¶෢����һ��WM_CHAR��Ϣ

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

	// �ⲿ���÷��� �����ֲο�microsoft windowlessRE���̣�
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

	HWND   m_hParentWnd; // ���ڵĴ��ھ��
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

class  WindowlessRichEdit : public ITextHostImpl
	                           , public ITextEditControl
	                           , public IRichEditOleCallback
{
public:
	WindowlessRichEdit();
	~WindowlessRichEdit(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// ����Ӹ�������ת����������Ϣ

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

        MESSAGE_HANDLER_EX( WM_IME_STARTCOMPOSITION, OnDefaultHandle )  // ����RE���������Ϣ�ᵼ�����뷨�Ĵ��ڲ�����
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

    // ��������
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
	
    // Ole����
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
	// ��Ҫ���ݿؼ����Խ��ж��ƵĽӿڷ�������ʵ�֣������ӿڽӿڷ���ITextHostImpl��ʵ��
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

	int    m_nLastTimerKickCount; // ˢ��FPS���ơ����ﻹ��Ҫ�������Ż�����ʹֻ��һ��GIF��Ҳ��Ҫ����client rectˢ����
	
	RECT   m_rcInvalidate;        // TxInvalidateRect������������Ч����ļ���
	bool   m_bNeedRedrawScrollbar;// �Ƿ���Ҫˢ�¹���������
    bool   m_bDuringTxDraw;       // ��־��ǰ�Ƿ����ڵ���TxDraw��������ڼ�Ĺ�����������Ի����ͣ�����Ӧ����ȥˢ��RE�ˣ�������ѭ��
    bool   m_bFocus;

    int    m_nAutoResizeMaxSize;  // ����Ӧ��Сʱ�����ֵ��singleline��Ӧwidth,multiline��Ӧheight��С��0��ʾ������

protected:
	// ��windowless richeditҪ���õĳ�ʼ������
	static LPCTSTR  GetLibraryName();
	void   InitRichEidtDll();
	void   ReleaseRichEidtDll();
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
public:
	//RichEdit formats
	static UINT     s_cfRichTextFormat;      // CLIPFORMAT
	static UINT     s_cfRichTextAndObjects;  // CLIPFORMAT
	static UINT     s_cfRichEditOleFormat;   // ole ��ճ���ʽ
	static UINT     s_cfUnicodeRichEditOleFormat; // ole ��ճ���ʽ
};

}
