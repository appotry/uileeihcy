#pragma once
#include "flash.tlh"
#include "UISDK\Control\Inc\Interface\iflashwrap.h"

namespace UI
{


#if 0
	1. site 站点对象是什么?
		每一个嵌入对象都会对应窗口(Container)中的一个site对象，该site对象为嵌入对象提供位置等信息
		容器中维护了site列表。

	2. swf一般是不能暂停的

#endif
	class FlashWrap;
	class FlashEmbeddingSite : 
					public IOleClientSite,
					public IOleControlSite,
					public IAdviseSink,       // 控件通过该接口调用OnViewChange来刷新自己
					public IOleInPlaceSiteWindowless
		
	{
	public:
		FlashEmbeddingSite(FlashWrap* p);
		~FlashEmbeddingSite();

#pragma region // IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject);
		virtual ULONG   STDMETHODCALLTYPE AddRef(void);
		virtual ULONG   STDMETHODCALLTYPE Release(void);
#pragma endregion

#pragma region // IAdviseSink
		virtual void STDMETHODCALLTYPE OnDataChange(FORMATETC *pFormatetc, STGMEDIUM *pStgmed)
			{ }
		virtual  void STDMETHODCALLTYPE OnViewChange(DWORD dwAspect, LONG lindex)
			{ }
		virtual void STDMETHODCALLTYPE OnRename(IMoniker *pmk)
			{ }
		virtual void STDMETHODCALLTYPE OnSave(void) 
			{ }
		virtual void STDMETHODCALLTYPE OnClose(void)
			{ }
#pragma endregion

#pragma region // IOleClientSite
		virtual HRESULT STDMETHODCALLTYPE SaveObject(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,  IMoniker **ppmk)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer)
			{ *ppContainer = NULL; return E_NOTIMPL; } 
		virtual HRESULT STDMETHODCALLTYPE ShowObject(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void)
			{ return S_OK; }
#pragma endregion

#pragma region // IOleControlSite
		virtual HRESULT STDMETHODCALLTYPE OnControlInfoChanged(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE LockInPlaceActive(BOOL fLock)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetExtendedControl(IDispatch **ppDisp)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE TransformCoords(POINTL *pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnFocus(BOOL fGotFocus)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE ShowPropertyFrame(void)
			{ return S_OK; }
#pragma endregion

#pragma region  // IOleInPlaceSiteWindowless
		virtual HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void) 
			{ return S_OK; } // support Windowless activation
		virtual HRESULT STDMETHODCALLTYPE GetCapture(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE SetCapture(BOOL fCapture)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetFocus(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE SetFocus(BOOL fFocus)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetDC(LPCRECT pRect, DWORD grfFlags, HDC *phDC)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hDC)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE InvalidateRect(LPCRECT pRect, BOOL fErase);
		virtual HRESULT STDMETHODCALLTYPE InvalidateRgn(HRGN hRGN, BOOL fErase)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE AdjustRect(LPRECT prc)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
			{ return S_OK; }
#pragma endregion

#pragma region  // IOleInPlaceSiteEx
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL *pfNoRedraw, DWORD dwFlags)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL fNoRedraw)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE RequestUIActivate(void)
			{ return S_OK; }
#pragma endregion

#pragma region // IOleInPlaceSite
		virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect)
			{ return S_OK; }
#pragma endregion

#pragma region // IOleWindow
		virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) 
			{ return S_OK; }
#pragma endregion

	public:
		HRGN   GetInvalidateRect(bool bClear);
		void   SetFlashInvalidateListener(IFlashInvalidateListener* p)
		{ m_pListener = p; }

	private:
		long         m_lRef;
		FlashWrap*   m_pFlashWrap;
        IFlashWrap*  m_pIFlashWrap;

		CRect        m_rcInvalidate;
	public:
		HRGN         m_hInvalidateRgn;
		IFlashInvalidateListener*  m_pListener;
	};

class FlashWrap : public MessageProxy
{
public:
	UI_DECLARE_OBJECT3(FlashWrap, OBJ_CONTROL|CONTROL_FLASH, _T("UICtrl/Control"))
	FlashWrap();
	~FlashWrap();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_REDRAWOBJECT(OnRedrawObject)
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(FlashWrap, IControl)

    void  SetIFlashWrap(IFlashWrap* p){ m_pIFlashWrap = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    inline IFlashWrap*  GetIFlashWrap() { return m_pIFlashWrap; }

public:
    void  SetFlashUri(BSTR bstr);
    HRESULT  CallFlashFunction(const TCHAR* szFuncName, const TCHAR* szArgs, BSTR* pbstrRet);
    void  Pause();
    void  Play();

public:
	HRESULT  FinalConstruct(IUIApplication* p);
	void  GetDesiredSize(SIZE* pSize);
	void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);

	void   SetFlashInvalidateListener(IFlashInvalidateListener* p);
	void   OnPaint(IRenderTarget* pRenderTarget);  // <- 做成public的，便于render layer中直接调用
	bool   IsAvailable();

protected:
	void   OnSize( UINT nType, int cx, int cy );
	void   OnRedrawObject();

	void   SetWMode(FLASH_WMODE e);

protected:
	bool      CreateControl();
	void      DrawFlash(HDC hDC);
	HRESULT   SetFlashPos(LPRECT lprc);
	HRESULT   SetFlashSize(int cx, int cy);

protected:
    IFlashWrap*  m_pIFlashWrap;

	FlashEmbeddingSite*  m_pSite;
	IOleObject*          m_pOleObject;
	IViewObjectEx*       m_pViewObject;
public:
	IShockwaveFlash*     m_pFlash;
protected:

	String   m_strFlashUri;
	FLASH_WMODE  m_eWMode;
	int      m_nFlashWidth;
	int      m_nFlashHeight;

};

struct  FlashPropertyUtil
{
	static int  GetFlashWidth(IShockwaveFlash* pFlash);
	static int  GetFlashHeight(IShockwaveFlash* pFlash);
};
}