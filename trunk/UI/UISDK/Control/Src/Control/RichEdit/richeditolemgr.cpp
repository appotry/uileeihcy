#include "stdafx.h"
#include "richeditolemgr.h"
#include "gifoleobject.h"
#include <algorithm>

IRichEditOleObjectItem::~IRichEditOleObjectItem()
{
    if (m_pOleObjectMgr)
    {
        m_pOleObjectMgr->OnOleObjDelete(this);
    }
}

#pragma region
RichEditOleObjectItem_Com::RichEditOleObjectItem_Com()
{
	m_pOleObject = NULL;
    memset(&m_clsid, 0, sizeof(m_clsid));
}
RichEditOleObjectItem_Com::~RichEditOleObjectItem_Com()
{  
	SAFE_RELEASE(m_pOleObject);
}

HRESULT  RichEditOleObjectItem_Com::GetOleObject(IOleObject** ppOleObject, bool bAddRef)
{
	if (NULL == ppOleObject)
		return E_INVALIDARG;

	if (NULL == m_pOleObject)
		return E_FAIL;

	*ppOleObject = m_pOleObject;

	if (bAddRef)
		m_pOleObject->AddRef();

	return S_OK;
}
HRESULT  RichEditOleObjectItem_Com::GetEncodeText(BSTR* pbstr)
{
    if (!pbstr)
        return E_INVALIDARG;

    WCHAR szText[128] = {0};
    WCHAR szGUID[64] = {0};

    StringFromCLSID(m_clsid, (LPOLESTR*)&szGUID);
    wsprintf(szText, L"<com version=\"1.0\" clsid=\"%s\"/>", szGUID);
    *pbstr = SysAllocString(szText);
    return S_OK;
}

HRESULT  RichEditOleObjectItem_Com::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
{
	return E_NOTIMPL;
}


HRESULT  RichEditOleObjectItem_Com::Attach(CLSID  clsid)
{
	SAFE_RELEASE(m_pOleObject);
    m_clsid = clsid;
	return ::CoCreateInstance(clsid, NULL, CLSCTX_INPROC, IID_IOleObject, (void**)&m_pOleObject); 
}
#pragma endregion

#pragma region
RichEditOleObjectItem_Inner::RichEditOleObjectItem_Inner()
{
	m_pViewAdviseSink = NULL;
	m_pClientSite = NULL;
	m_dwRef = 0;

	m_pOleAdviseHolder = NULL;
//	m_pDataAdviseHolder = NULL;
}
RichEditOleObjectItem_Inner::~RichEditOleObjectItem_Inner()
{
	SAFE_RELEASE(m_pViewAdviseSink);
	SAFE_RELEASE(m_pClientSite);
	SAFE_RELEASE(m_pOleAdviseHolder);
//	SAFE_RELEASE(m_pDataAdviseHolder);
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::QueryInterface(REFIID riid, void** ppvObject)
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

ULONG STDMETHODCALLTYPE RichEditOleObjectItem_Inner::AddRef( void) 
{
	return ++m_dwRef;
}

ULONG STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Release( void) 
{
	if (0 == --m_dwRef)
	{
		delete this;
		return 0;
	}
	return m_dwRef;
}

HRESULT RichEditOleObjectItem_Inner::GetOleObject(IOleObject** ppOleObject, bool bAddRef)
{
	if (NULL == ppOleObject)
		return E_INVALIDARG;

	*ppOleObject = static_cast<IOleObject*>(this);

	if (bAddRef)
		this->AddRef();

	return S_OK;
}
HRESULT RichEditOleObjectItem_Inner::GetEncodeText(BSTR* pbstr)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::SetClientSite(IOleClientSite *pClientSite)
{
	SAFE_RELEASE(m_pClientSite);
	m_pClientSite = pClientSite;
	if (m_pClientSite)
		m_pClientSite->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetClientSite(IOleClientSite **ppClientSite) 
{
	*ppClientSite = m_pClientSite;
	if (m_pClientSite)
		m_pClientSite->AddRef();

	return S_OK;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetExtent(DWORD dwDrawAspect, SIZEL *psizel) 
{
//	psizel->cx = psizel->cy = 2000;
	SIZE  size = {0,0};
	this->OnGetSize(&size);

	psizel->cx = Util::DXtoHimetricX(size.cx);
	psizel->cy = Util::DYtoHimetricY(size.cx);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Draw( 
												DWORD dwDrawAspect,
												LONG lindex,
												void *pvAspect,
												DVTARGETDEVICE *ptd,
												HDC hdcTargetDev,
												HDC hdcDraw,
												LPCRECTL lprcBounds,
												LPCRECTL lprcWBounds,
												BOOL ( STDMETHODCALLTYPE *pfnContinue )(ULONG_PTR dwContinue),
												ULONG_PTR dwContinue) 
{

	RECT  rc = {lprcBounds->left, lprcBounds->top, lprcBounds->right, lprcBounds->bottom};
	return this->OnDraw(hdcDraw, &rc);
// 	HBRUSH hBrush = CreateSolidBrush(rand());
// 	::FillRect(hdcDraw, (RECT*)lprcBounds, hBrush);
// 	DeleteObject(hBrush);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel) 
{
	return this->GetExtent(dwDrawAspect, lpsizel);
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection)
{
	HRESULT hr = S_OK;
	if (m_pOleAdviseHolder == NULL)
		hr = CreateOleAdviseHolder(&m_pOleAdviseHolder);
	if (SUCCEEDED(hr))
		hr = m_pOleAdviseHolder->Advise(pAdvSink, pdwConnection);
	return hr;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::Unadvise(DWORD dwConnection)
{
	HRESULT hRes = E_FAIL;
	if (m_pOleAdviseHolder != NULL)
		hRes = m_pOleAdviseHolder->Unadvise(dwConnection);
	return hRes;
}
HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink) 
{
	SAFE_RELEASE(m_pViewAdviseSink);
	m_pViewAdviseSink = pAdvSink;
	if (m_pViewAdviseSink)
		m_pViewAdviseSink->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE RichEditOleObjectItem_Inner::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	*ppAdvSink = m_pViewAdviseSink;
	if (m_pViewAdviseSink)
		m_pViewAdviseSink->AddRef();
	return S_OK;
}
#pragma endregion

#pragma region
RichEditOleObjectManager::RichEditOleObjectManager(WindowlessRichEdit* pRichEdit)
{
	m_pRichEdit = pRichEdit;
	m_pUIApp = NULL;
}
RichEditOleObjectManager::~RichEditOleObjectManager()
{
	// 判断一下
	// OleFlushClipboard();

	OLELIST::iterator iter = m_listOleObj.begin();
	for (; iter != m_listOleObj.end(); iter = m_listOleObj.begin())
	{
        IRichEditOleObjectItem* pItem = *iter;
	    SAFE_DELETE(pItem);  // 会触发OnOleObjDelete，删除iter
	}
}

bool RichEditOleObjectManager::AddOleItem(IRichEditOleObjectItem* pItem)
{
	if (NULL == pItem)
		return false;

	pItem->SetOleObjectManager(this);
	m_listOleObj.push_back(pItem);

	return true;
}
void  RichEditOleObjectManager::OnOleObjDelete(IRichEditOleObjectItem* pItem)
{
    if (!pItem)
        return;

    OLELIST::iterator iter = std::find(m_listOleObj.begin(), m_listOleObj.end(), pItem);
    if (iter != m_listOleObj.end())
    {
        m_listOleObj.erase(iter);
    }
}

void RichEditOleObjectManager::SetUIApplication(IUIApplication* p)
{
	m_pUIApp = p;
#if 0 // -- 架构改造
	m_gifMgr.SetUIApplication(p);
#endif
}
// QQ gif 剪贴格式
// 		<QQRichEditFormat>
// 			<Info version="1001">
// 			</Info>
// 			<EditElement 
// 				type="1" 
// 				filepath="C:\Users\libopc\AppData\Roaming\Tencent\QQ\QQFace\OperationPath\1022.gif" 
// 				shortcut="">
// 			</EditElement>
// 		</QQRichEditFormat>

// QQ 表情剪贴格式
// 		<QQRichEditFormat>
// 			<Info version="1001">
// 			</Info>
// 			<EditElement 
// 				type="2" 
// 				sysfaceindex="2" 
// 				filepath="" 
// 				shortcut="">
// 			</EditElement>
// 		</QQRichEditFormat>
HGLOBAL RichEditOleObjectManager::CreateGifFileClipboardData(const TCHAR* szFilePath, bool bUnicode)
{
	UIASSERT(bUnicode); // 暂不支持非UNICODE，懒得去实现
#if 0 // -- 架构改造
	CMarkup  xml;
	xml.SetDoc(_T("<UIRichEditFormat/>"));
	if (false == xml.FindElem())
		return NULL;
	xml.AddAttrib(_T("version"), _T("1.0"));
	xml.AddAttrib(_T("type"), RICHEDIT_OLE_GIF_FILE);
	xml.AddAttrib(_T("filepath"), szFilePath);
	
	String str = xml.GetDoc();

	HGLOBAL hGlobal = GlobalAlloc(0, sizeof(WCHAR)*(str.length()+1));
	LPWSTR lpstr = (LPWSTR)::GlobalLock(hGlobal);
	wcscpy(lpstr, str.c_str());
	::GlobalUnlock(hGlobal);
	return hGlobal;
#endif

    return NULL;
}
HGLOBAL RichEditOleObjectManager::CreateEmotionClipboardData(const TCHAR* szEmotionName, bool bUnicode)
{
	return NULL;
}

bool RichEditOleObjectManager::ParseOleFormatXml(const LPWSTR wszXmlData)
{
#if 0 // -- 架构改造
	CMarkup xml;
	if (false == xml.SetDoc(wszXmlData))
		return false;

	if (false == xml.FindElem())
		return false;

	String strVersion = xml.GetAttrib(_T("version"));

	int nType = _ttoi(xml.GetAttrib(_T("type")).c_str());
	switch(nType)
	{
	case RICHEDIT_OLE_GIF_FILE:
		{
			String strFilePath = xml.GetAttrib(_T("filepath"));
			if (strFilePath.empty())
				return false;

			m_pRichEdit->InsertGif(strFilePath.c_str());
			return true;
		}
		break;
	default:
		return false;
	}
#endif
	return true;
}
#pragma endregion