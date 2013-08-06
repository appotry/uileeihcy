#pragma once
#include "UISDK\Control\Inc\Interface\iiewrap.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"

// http://www.cnblogs.com/lucc/archive/2010/11/24/1886087.html  WebBrowser介绍——Javascript与C++互操作 
//
// TODO: pSite最后被泄露了！
//
namespace UI
{
	class IEWrap;
	class PopupIEWindow;

	class IEEmbeddingSite : 
		  public IOleClientSite
		, public IOleControlSite
		, public IAdviseSink       // 控件通过该接口调用OnViewChange来刷新自己
		, public IOleInPlaceSite   // , public IOleInPlaceSiteWindowless
// 		, public IDocHostUIHandler
// 		, public IOleInPlaceFrame
// 		, public IDispatch
		, public IDocHostUIHandler // 支持js扩展

	{
	public:
		IEEmbeddingSite(IEWrap* p);
		~IEEmbeddingSite();

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
		{ *ppContainer = NULL; return E_NOTIMPL; }  // 直接返回S_OK会导致IE崩溃
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

#pragma region // IDocHostUIHandler
		virtual HRESULT STDMETHODCALLTYPE GetExternal(/* [out] */ IDispatch **ppDispatch);

		virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(/* [in] */ DWORD dwID, /* [in] */ POINT *ppt, /* [in] */ IUnknown *pcmdtReserved, /* [in] */ IDispatch *pdispReserved)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetHostInfo(/* [out][in] */ DOCHOSTUIINFO *pInfo)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE ShowUI(/* [in] */ DWORD dwID, /* [in] */ IOleInPlaceActiveObject *pActiveObject, /* [in] */ IOleCommandTarget *pCommandTarget, /* [in] */ IOleInPlaceFrame *pFrame, /* [in] */ IOleInPlaceUIWindow *pDoc)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE HideUI( void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE UpdateUI( void)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE EnableModeless(/* [in] */ BOOL fEnable)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(/* [in] */ BOOL fActivate)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(/* [in] */ BOOL fActivate)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE ResizeBorder(/* [in] */ LPCRECT prcBorder, /* [in] */ IOleInPlaceUIWindow *pUIWindow, /* [in] */ BOOL fRameWindow)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(/* [in] */ LPMSG lpMsg, /* [in] */ const GUID *pguidCmdGroup, /* [in] */ DWORD nCmdID)
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(/* [annotation][out] */  __out  LPOLESTR *pchKey, /* [in] */ DWORD dw) 
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE GetDropTarget(/* [in] */ IDropTarget *pDropTarget, /* [out] */ IDropTarget **ppDropTarget) 
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE TranslateUrl(/* [in] */ DWORD dwTranslate, /* [annotation][in] */ __in __nullterminated  OLECHAR *pchURLIn, /* [annotation][out] */ __out  OLECHAR **ppchURLOut) 
			{ return S_OK; }
		virtual HRESULT STDMETHODCALLTYPE FilterDataObject(/* [in] */ IDataObject *pDO, /* [out] */ IDataObject **ppDORet) 
			{ return S_OK; }
#pragma endregion

	private:
		long         m_lRef;
		IEWrap*      m_pIEWrap;
        IIEWrap*     m_pIIEWrap;
	};
	class IEWrap : public MessageProxy
	{
	public:
        IEWrap();
        ~IEWrap();

		UI_DECLARE_OBJECT3(IEWrap, OBJ_CONTROL|CONTROL_IE, _T("UICtrl/Control"))

		UI_BEGIN_MSG_MAP
			UIMSG_WM_SIZE(OnSize)
            UIMSG_WM_QUERYINTERFACE(QueryInterface)
            UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
            UIMSG_WM_SETATTRIBUTE(SetAttribute)
	    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(IEWrap, IControl)

        void  SetIIEWrap(IIEWrap* p) { m_pIIEWrap = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
        IIEWrap*  GetIIEWrap() { return m_pIIEWrap; }
	public:
		void  SetAttribute(IMapAttribute* pMatAttrib, bool bReload);
		void  OnSize( UINT nType, int cx, int cy );

		PopupIEWindow*  GetPopupIEWindow() 
			{ return m_pPopupIEWnd; }

		HRESULT    InvokeJsFunction(BSTR bstrFuncName);

	protected:
		void      CreateControl();
		HRESULT   SetIESize(int cx, int cy);
		HRESULT   SetIEPos(LPRECT lprc);

	protected:
        IIEWrap*   m_pIIEWrap;

		IEEmbeddingSite*  m_pSite;

		IWebBrowser2*     m_pWebBrowser;

		PopupIEWindow*    m_pPopupIEWnd;
//		HWND              m_hWndShellEmbedding;

		IOleObject*       m_pOleObject;
		IViewObject2*     m_pViewObject;
	};

	class PopupIEWindow : public ICustomWindow
	{
	public:
		PopupIEWindow();
        UI_BEGIN_MSG_MAP_Ixxx(PopupIEWindow)
        UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)
    public:
		IEWrap*  m_pIEWrap;
	};
}