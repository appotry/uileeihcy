#include "stdafx.h"
#include "iewrap.h"
namespace UI
{

IEEmbeddingSite::IEEmbeddingSite(IEWrap* p)
{
	m_lRef = 0;
	m_pIEWrap = p;
    m_pIIEWrap = p->GetIIEWrap();
}
IEEmbeddingSite::~IEEmbeddingSite()
{
}
HRESULT STDMETHODCALLTYPE IEEmbeddingSite::QueryInterface(REFIID riid, void **ppvObject)
{
	if (NULL == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown, riid))
		*ppvObject = static_cast<IUnknown*>((IOleClientSite*)this);
	else if (::IsEqualIID(IID_IOleClientSite, riid))
		*ppvObject = static_cast<IOleClientSite*>(this);
	else if (::IsEqualIID(IID_IOleControlSite, riid))
		*ppvObject = static_cast<IOleControlSite*>(this);
	else if (::IsEqualIID(IID_IAdviseSink, riid))
		*ppvObject = static_cast<IAdviseSink*>(this);
	else if (::IsEqualIID(IID_IOleInPlaceSite, riid))
		*ppvObject = static_cast<IOleInPlaceSite*>(this);
	else if (::IsEqualIID(IID_IOleWindow, riid))
		*ppvObject = static_cast<IOleWindow*>(this);
	else if (::IsEqualIID(IID_IDocHostUIHandler, riid))
		*ppvObject = static_cast<IDocHostUIHandler*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}
ULONG STDMETHODCALLTYPE IEEmbeddingSite::AddRef(void)
{
	return ++m_lRef;
}
ULONG STDMETHODCALLTYPE IEEmbeddingSite::Release(void)
{
	--m_lRef; 
	if (0==m_lRef)
	{
		delete this; 
		return 0;
	} 
	return m_lRef;
}

HRESULT STDMETHODCALLTYPE IEEmbeddingSite::GetWindow(HWND *phwnd)
{
	if (NULL == phwnd || NULL == m_pIEWrap)
		return E_FAIL;

	if (NULL == m_pIEWrap->GetPopupIEWindow())
		*phwnd = m_pIIEWrap->GetHWND();
	else
		*phwnd = m_pIEWrap->GetPopupIEWindow()->GetHWND();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE IEEmbeddingSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,  LPRECT lprcPosRect, LPRECT lprcClipRect,  LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	if (ppFrame)
		*ppFrame = NULL;
	if (ppDoc)
		*ppDoc = NULL;

	CRect rc;
	m_pIIEWrap->GetParentRect(&rc);

	if (lprcPosRect)
	{
		CopyRect(lprcPosRect, &rc);
	}
	if (lprcClipRect)
	{
		CopyRect(lprcClipRect, &rc);
	}
	if (lpFrameInfo)
	{
		lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
		lpFrameInfo->fMDIApp = FALSE;
		GetWindow(&lpFrameInfo->hwndFrame);
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;
	}
	return S_OK;
}

#pragma region
void CppCall()
{
	MessageBox(NULL, L"您调用了CppCall", L"提示(C++)", 0);
}
class ClientCall:public IDispatch
{
	long _refNum;
public:
	ClientCall()
	{
		_refNum = 1;
	}
	~ClientCall(void)
	{
	}
public:

	// IUnknown Methods

	STDMETHODIMP QueryInterface(REFIID iid,void**ppvObject)
	{
		*ppvObject = NULL;
		if (iid == IID_IUnknown)    *ppvObject = this;
		else if (iid == IID_IDispatch)    *ppvObject = (IDispatch*)this;
		if(*ppvObject)
		{
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return ::InterlockedIncrement(&_refNum);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		::InterlockedDecrement(&_refNum);
// 		if(_refNum == 0)
// 		{
// 			delete this;
// 		}
		return _refNum;
	}

	// IDispatch Methods

	HRESULT _stdcall GetTypeInfoCount(
		unsigned int * pctinfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetTypeInfo(
		unsigned int iTInfo,
		LCID lcid,
		ITypeInfo FAR* FAR* ppTInfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetIDsOfNames(
		REFIID riid, 
		OLECHAR FAR* FAR* rgszNames, 
		unsigned int cNames, 
		LCID lcid, 
		DISPID FAR* rgDispId 
		)
	{
		if(lstrcmp(rgszNames[0], L"CppCall")==0)
		{
			//网页调用window.external.CppCall时，会调用这个方法获取CppCall的ID
			*rgDispId = 100;
		}
		return S_OK;
	}

	HRESULT _stdcall Invoke(
		DISPID dispIdMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS* pDispParams,
		VARIANT* pVarResult,
		EXCEPINFO* pExcepInfo,
		unsigned int* puArgErr
		)
	{
		if(dispIdMember == 100)
		{
			//网页调用CppCall时，或根据获取到的ID调用Invoke方法
			CppCall();
		}
		return S_OK;
	}
};
#pragma endregion

HRESULT STDMETHODCALLTYPE IEEmbeddingSite::GetExternal(/* [out] */ IDispatch **ppDispatch)
{
	static ClientCall c;
	*ppDispatch = static_cast<IDispatch*>(&c);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
IEWrap::IEWrap()
{
    m_pIIEWrap = NULL;

	m_pWebBrowser = NULL;
	m_pOleObject = NULL;
	m_pViewObject = NULL;
	m_pSite = NULL;
//	m_hWndShellEmbedding = NULL;
	m_pPopupIEWnd = NULL;
}

IEWrap::~IEWrap()
{
	SAFE_RELEASE(m_pSite);
	SAFE_RELEASE(m_pWebBrowser);
	SAFE_RELEASE(m_pOleObject);
	SAFE_RELEASE(m_pViewObject);
	SAFE_DELETE(m_pPopupIEWnd);
}


void IEWrap::SetAttribute(IMapAttribute* pMatAttrib, bool bReload)
{
	DO_PARENT_PROCESS(IIEWrap, IControl);

	CreateControl();
}
void IEWrap::CreateControl()
{
	HRESULT hr = S_OK;
	
	do
	{
		// 创建站点对象
		m_pSite = new IEEmbeddingSite(this);
		m_pSite->AddRef();

		IOleClientSite* pControlSite = static_cast<IOleClientSite*>(m_pSite);
		IAdviseSink*    pAdviseSink = static_cast<IAdviseSink*>(m_pSite);

		hr = ::CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (void**)&m_pWebBrowser);

		if (FAILED(hr) || NULL == m_pWebBrowser)
		{
			UI_LOG_WARN(_T("%s Create WebBrowser ActiveX Control Failed."), FUNC_NAME);
			break;
		}

        IUIApplication* pUIApp = m_pIIEWrap->GetUIApplication();
		HWND hParentWnd = m_pIIEWrap->GetHWND();
		if (1 || Util::IsLayeredWindow(hParentWnd))
		{
			m_pIIEWrap->RemoveMeInTheTree();

            PopupIEWindow::CreateInstance(pUIApp, &m_pPopupIEWnd);
            m_pPopupIEWnd->m_pIEWrap = this;

 			m_pPopupIEWnd->Create(pUIApp, _T(""), hParentWnd);
			m_pPopupIEWnd->AddChild(m_pIIEWrap);
//            m_pPopupIEWnd->SetObjectPos(0, 0, m_pIIEWrap->GetConfigWidth(), m_pIIEWrap->GetConfigHeight(), SWP_NOREDRAW|SWP_NOMOVE);
			
            IMapAttribute* pEmpty = NULL;
            UICreateIMapAttribute(&pEmpty);

			SERIALIZEDATA sdata = {0};
			sdata.pMapAttrib = pEmpty;
			sdata.nFlag = SERIALIZEFLAG_LOAD;
			UISendMessage(m_pPopupIEWnd, UI_WM_SERIALIZE, (WPARAM)&sdata);
//			UISendMessage(m_pPopupIEWnd, UI_WM_SETATTRIBUTE, (WPARAM)pEmpty, (LPARAM)false);
            SAFE_RELEASE(pEmpty);

			SyncWindowData data;
			data.m_hWnd = m_pPopupIEWnd->GetHWND();
            data.m_nAnchorType = ANCHOR_CUSTOM;
// 
// 			if (m_pIIEWrap->GetConfigLeft() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_LEFT;
// 				data.m_rcAnchorData.xOffset = m_pIIEWrap->GetConfigLeft();
// 			}
// 			if (m_pIIEWrap->GetConfigRight() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_RIGHT;
// 				if (m_pIIEWrap->GetConfigLeft() == NDEF)
// 					data.m_rcAnchorData.xOffset = m_pIIEWrap->GetConfigRight();
// 				else
// 					data.m_rcAnchorData.xOffset2 = m_pIIEWrap->GetConfigRight();
// 			}
// 			if (m_pIIEWrap->GetConfigTop() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_TOP;
// 				data.m_rcAnchorData.yOffset = m_pIIEWrap->GetConfigTop();
// 			}
// 			if (m_pIIEWrap->GetConfigBottom() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_BOTTOM;
// 				if (m_pIIEWrap->GetConfigTop() == NDEF)
// 					data.m_rcAnchorData.yOffset = m_pIIEWrap->GetConfigBottom();
// 				else
// 					data.m_rcAnchorData.yOffset2 = m_pIIEWrap->GetConfigBottom();
// 			}
// 			if (m_pIIEWrap->GetConfigWidth() > 0)
// 				data.m_rcAnchorData.Width = m_pIIEWrap->GetConfigWidth();
// 			if (m_pIIEWrap->GetConfigHeight() > 0)
// 				data.m_rcAnchorData.Height = m_pIIEWrap->GetConfigHeight();
            
            UIASSERT(0 && _T("TODO: 未完成"));

			::SendMessage(hParentWnd, UI_WM_SYNC_WINDOW, (WPARAM)ADD_SYNC_WINDOW, (LPARAM)&data);
			::ShowWindow(m_pPopupIEWnd->GetHWND(), SW_SHOWNOACTIVATE);

			m_pPopupIEWnd->UpdateLayout(true);
		}

		m_pWebBrowser->QueryInterface(IID_IOleObject, (void**)&m_pOleObject);

		hr = m_pOleObject->QueryInterface(__uuidof(IViewObject2), (void**)&m_pViewObject);
		if (FAILED(hr) || NULL == m_pViewObject)	
			break;

		// 将站点对象与控件关联起来
		hr = m_pOleObject->SetClientSite(pControlSite);
		if (FAILED(hr))
			break;

		hr = m_pViewObject->SetAdvise(DVASPECT_CONTENT, 0, pAdviseSink);
		if (FAILED(hr))
			break;

		// 初始化控件属性
		// 注：只有在调用了InitNew之后，再修改WMode才有用
// 		CComQIPtr<IPersistStreamInit> spPSI = m_pOleObject;
// 		hr = spPSI->InitNew();
// 		UIASSERT(SUCCEEDED(hr));

//		this->SetWMode(m_eWMode);

		

		// 注：DoVerb OLEIVERB_INPLACEACTIVATE 不要放在put_Movie后面，否则flash会播放的很快。（为什么会这样?）
		// 此时会去请求IOleInPlaceSiteWindowless接口
		// 也会走到GetWindowContext接口中，貌似只在要该接口中指定Flash的位置即可，因此将DoVerb中的一些参数直接设置为NULL
		HRESULT hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pControlSite, 0, NULL, NULL);

		CComVariant v(L"http://www.baidu.com");
		m_pWebBrowser->Navigate2(&v, NULL,NULL,NULL,NULL);

// 		CComQIPtr<IOleInPlaceObject> p = m_pWebBrowser;
// 		CRect rc(0,0,130,130);
// 		p->SetObjectRects(&rc, &rc);

		// 获取用于承载IE的窗口
// 		CComQIPtr<IOleWindow> pOleWindow = m_pWebBrowser;
// 		if (pOleWindow)
// 			pOleWindow->GetWindow(&m_hWndShellEmbedding);

	}
	while (0);
}

void IEWrap::OnSize( UINT nType, int cx, int cy )
{
	if (NULL == m_pWebBrowser)
		return;

    CRect  rc;
    m_pIIEWrap->GetParentRect(&rc);
	SetIEPos(&rc);
}

HRESULT IEWrap::SetIEPos(LPRECT lprc)
{
	if (NULL == m_pWebBrowser || NULL == lprc)
		return E_INVALIDARG;

	CComQIPtr<IOleInPlaceObject> p = m_pWebBrowser;
	return p->SetObjectRects(lprc, lprc);
}
HRESULT IEWrap::SetIESize(int cx, int cy)
{
	if (NULL == m_pOleObject)
		return E_INVALIDARG;

	SIZEL pxsize = { cx, cy };
	SIZEL hmsize = { 0 };
	AtlPixelToHiMetric(&pxsize, &hmsize);
	return m_pOleObject->SetExtent(DVASPECT_CONTENT, &hmsize);
}

HRESULT IEWrap::InvokeJsFunction(BSTR bstrFuncName)
{
	if (NULL == m_pWebBrowser)
		return E_FAIL;

	CComPtr<IDispatch> pDispDoc = NULL;
	m_pWebBrowser->get_Document(&pDispDoc);
	if (NULL == pDispDoc)
		return E_FAIL;

	CComQIPtr<IHTMLDocument2> pDoc = pDispDoc;
	if (NULL == pDoc)
		return E_FAIL;

	CComPtr<IHTMLWindow2> pWnd = NULL;
	pDoc->get_parentWindow(&pWnd);
	if (NULL == pWnd)
		return E_FAIL;

	CComQIPtr<IDispatch>  pDispatch = pWnd;
	if (NULL == pDispatch)
		return E_FAIL;
	
	DISPID dispid = 0;
	if (FAILED (pDispatch->GetIDsOfNames(IID_NULL,&bstrFuncName,1,LOCALE_SYSTEM_DEFAULT,&dispid))) 
		dispid = -1;

	if(dispid == -1) 
		return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 0;
	ps.rgvarg = NULL;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	CComVariant varResult;
	return pDispatch->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &ps, &varResult, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////////

PopupIEWindow::PopupIEWindow()
{
	m_pIEWrap = NULL;
}
// BOOL PopupIEWindow::PreCreateWindow(CREATESTRUCT& cs)
// {
// 	__super::PreCreateWindow(cs);
// 	if (m_pIEWrap->GetConfigWidth() > 0)
// 	{
// 		cs.cx = m_pIEWrap->GetConfigWidth();
// 	}
// 	if (m_pIEWrap->GetConfigHeight() > 0)
// 	{
// 		cs.cy = m_pIEWrap->GetConfigHeight();
// 	}
// 
// 	return TRUE;
// }

}