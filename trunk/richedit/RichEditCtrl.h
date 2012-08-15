#pragma once

class CRichEditOleCallbackImpl : public IRichEditOleCallback
{
public:
	CRichEditOleCallbackImpl(){ m_hRichEditWnd = NULL; }

	// *** IUnknown methods ***
	virtual HRESULT __stdcall QueryInterface(REFIID riid, LPVOID FAR * lplpObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();

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

class RichEditCtrl : public CRichEditCtrl, public CRichEditOleCallbackImpl
{
public:
	RichEditCtrl(void);
	~RichEditCtrl(void);

public:
	// 对外接口
	bool    CreateControl(HWND hWndParent, CRect rect, UINT nCtrlID, bool bMultiLine=true);
	void    Destroy();


protected:
	// 内部方法
	void    InitRichEidtDll();
	void    ReleaseRichEidtDll();


protected:
	static HMODULE  s_RichEditDll;
	static LONG     s_refDll;
};
