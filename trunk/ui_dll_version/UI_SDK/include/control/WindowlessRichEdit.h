#pragma once
#include <richedit.h>
#include <textserv.h>
#include <RichOle.h>
#include "CaretWindow.h"
#include "richeditolemgr.h"

#pragma comment(lib, "Riched20.lib")
class   RichEditBase;

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
//
//  Q13.�����ʾ��������
//      ��TxGetScrollBars�У��������m_dwStyle��ʽ�������Ҫ��m_dwStyle��ָ��
//      WM_HSCROLL,WM_VSCROLL...
//
//  Q14.OLE����������.
//      ��Ϊrichedit��undo���ܣ������һֱ�������ڴ��У�ֱ��richedit�رա�
//
//      1. WM_KEYDOWN delete��ɾ������ʱ��IRichEditOleCallback::DeleteObject�����ﲻ�ܵ���Release
// 			>	UIDLL.dll!IRichEditOleCallbackImpl<WindowlessRichEdit>::DeleteObject(IOleObject * lpoleobj=0x0011fc90)  ��251	C++
// 				riched20.dll!CObjectMgr::ReplaceRange()  + 0x9a �ֽ�	
// 				riched20.dll!CRchTxtPtr::ReplaceRange()  + 0x18dde �ֽ�	
// 				riched20.dll!CTxtRange::ReplaceRange()  + 0x80 �ֽ�	
// 				riched20.dll!CTxtSelection::ReplaceRange()  + 0x1f9 �ֽ�	
// 				riched20.dll!CTxtSelection::Backspace()  + 0xc0 �ֽ�	
// 				riched20.dll!CTxtEdit::OnTxKeyDown()  + 0x5d4 �ֽ�	
// 				riched20.dll!CTxtEdit::TxSendMessage()  + 0x21f3 �ֽ�	
// 				UIDLL.dll!WindowlessRichEdit::OnPostHandleMsg(HWND__ * hWnd=0x00220428, unsigned int Msg=256, unsigned int wParam=8, long lParam=917505)  ��196 + 0x39 �ֽ�	C++
//
//      2. DeleteObject֮�󣬽��Ż���ӦIOleObject::Close�� ���ﲻ�ܵ���Release
//         ��һ��OLE������delete����ɾ����ʱ������á�Ȼ�󽫶�����ctrl+z�ٻ�ԭ��Ȼ����ɾ��ʱ���ֻ����һ��
//
// 			>	UIDLL.dll!CGifOleObject::Close(unsigned long dwSaveOption=0)  ��50	C++
// 				riched20.dll!COleObject::Close()  + 0x2d �ֽ�	
// 				riched20.dll!CReplaceObjectAE::OnCommit()  + 0xa �ֽ�	
// 				riched20.dll!CommitAEList()  + 0x21 �ֽ�	
// 				riched20.dll!CGenUndoBuilder::Done()  + 0xf276 �ֽ�	
// 				riched20.dll!CTxtEdit::TxSendMessage()  + 0x11b �ֽ�	
// 				UIDLL.dll!WindowlessRichEdit::OnPostHandleMsg(HWND__ * hWnd=0x001504fc, unsigned int Msg=256, unsigned int wParam=8, long lParam=917505)  ��196 + 0x39 �ֽ�	C++
//
//      3. ����. ��ole�����Ѳ���undo��Χʱ��richedit�ŻὫ�ö���release������ʱCGifOleObject���ͷ�
//         ��ջ���£�
//           >	UIDLL.dll!CGifOleObject::~CGifOleObject()  ��13	C++
// 				UIDLL.dll!CGifOleObject::`scalar deleting destructor'()  + 0x2b �ֽ�	C++
// 				UIDLL.dll!CGifOleObject::Release()  ��66 + 0x2b �ֽ�	C++
// 				riched20.dll!SafeReleaseAndNULL()  + 0x17 �ֽ�	
// 				riched20.dll!COleObject::DisconnectObject()  + 0x95 �ֽ�	
// 				riched20.dll!COleObject::CleanupState()  + 0x33 �ֽ�	
// 				riched20.dll!COleObject::MakeZombie()  + 0xa �ֽ�	
// 				riched20.dll!CReplaceObjectAE::Destroy()  + 0x1a �ֽ�	
// 				riched20.dll!DestroyAEList()  + 0x1d �ֽ�	
// 				riched20.dll!CUndoStack::ClearAll()  + 0x2c �ֽ�	
// 				riched20.dll!CGenUndoBuilder::Done()  + 0x1c56d �ֽ�	
// 				riched20.dll!CTxtEdit::TxSendMessage()  + 0x11b �ֽ�	
// 				UIDLL.dll!WindowlessRichEdit::OnChar(unsigned int uMsg=258, unsigned int wParam=97, long lParam=1966081)  ��297 + 0x39 �ֽ�	C++
//
//   Q15.  Rich Edit ��������� 
//      http://zjqzy03080312.blog.163.com/blog/static/1857428072012124112714348/
//
//		Ӧ�ó������ճ�������������ݵ�һ��Rich Edit�ؼ��У�������ѿ��ü������ʽ����ָ���ļ������ʽ��
//      ��Ҳ���Ծ����Ƿ�һ��Rich Edit�ؼ�����ճ��ĳ�ּ������ʽ�� 
//
//		����һ��Edit�ؼ����ԣ������ʹ��WM_COPY����WM_CUT��Ϣ���������߼��е�ǰѡ�����ݡ�ͬ���ģ����
//      ��ʹ��WM_PASTE��Ϣ����Щ����������ճ����һ��Rich Edit�ؼ��С��ؼ���ճ������ʶ��ĵ�һ�����ø�ʽ��
//      ��������������Եĸ�ʽ�� 
//
//		�����ʹ��EM_PASTESPECIAL��Ϣ��ճ��ָ���ļ������ʽ�������Ϣ�Ծ���"����ճ��"�����Ӧ�ó������
//      �ã�������������û�ѡ��������ʽ�������ʹ��EM_CANPASTE��Ϣ�������ؼ��Ƿ�ʶ��ĳ��ָ���ĸ�ʽ�� 
//
//		��Ҳ����ʹ��EM_CANPASTE��Ϣ������Rich Edit�ؼ��Ƿ�ʶ�����п��õļ������ʽ������Ϣ�ڴ���WM_INITMENUPOPUP
//      ��Ϣʱ�����á�Ӧ�ó���������û�������"ճ��"���ȡ���ڿؼ��Ƿ����ճ����һ���������͡� 
//
//		Rich Edit�ؼ�ע�����ּ������ʽ��"���ı���ʽ(RTF)"��һ�ֽ���"RichEdit �ı������"�ĸ�ʽ��Ӧ�ó���
//      ����ʹ��RegisterClipboardFormat������ע����Щ��ʽ����ȡֵΪCF_RTF��CF_RETEXTOBJ
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

	HWND          m_hParentWnd; // ���ڵĴ��ھ��
	CCaret        m_caret;      // ��꣨������ϵͳ���ͷֲ㴰��ģ�������ף�
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

class UIAPI WindowlessRichEdit : public ITextHostImpl, 
	                             public ITextEditControl
	                           , public IRichEditOleCallback
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
	void    ClearOleObjects();

protected:
	RichEditBase*   m_pRichEditBase;
	RichEditOleObjectManager  m_olemgr;


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
};


//#include <atlctl.h>
