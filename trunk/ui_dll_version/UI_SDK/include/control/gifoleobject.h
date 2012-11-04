#pragma 
#include "../util/GifImage.h"

//////////////////////////////////////////////////////////////////////////
//
//                      用于richedit中插入的ole对象
//
//////////////////////////////////////////////////////////////////////////


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


class CGifOleObject : public IGifOleObject, public IViewObject2
{
protected:
	CGifOleObject();
	~CGifOleObject();

public:
	static  HRESULT CreateInstance(REFIID riid, void** ppv);
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void  **ppvObject);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);

	//////////////////////////////////////////////////////////////////////////
#pragma region // ole object 
	virtual HRESULT STDMETHODCALLTYPE SetHostNames(LPCOLESTR szContainerApp,LPCOLESTR szContainerObj) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Close(DWORD dwSaveOption) 
	{
		this->Release();
		return S_OK;	
	}

	virtual HRESULT STDMETHODCALLTYPE SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite,LONG lindex, HWND hwndParent, LPCRECT lprcPosRect) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumVerbs(IEnumOLEVERB **ppEnumOleVerb) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Update( void) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE IsUpToDate( void) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetUserClassID(CLSID *pClsid) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE SetExtent(DWORD dwDrawAspect, SIZEL *psizel) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwConnection) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE EnumAdvise(IEnumSTATDATA **ppenumAdvise) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus) 
	{ return E_NOTIMPL;	}
	virtual HRESULT STDMETHODCALLTYPE SetColorScheme(LOGPALETTE *pLogpal)
	{ return E_NOTIMPL;	}

	virtual HRESULT STDMETHODCALLTYPE SetClientSite(IOleClientSite *pClientSite);
	virtual HRESULT STDMETHODCALLTYPE GetClientSite(IOleClientSite **ppClientSite);
	virtual HRESULT STDMETHODCALLTYPE GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
#pragma  endregion
	//////////////////////////////////////////////////////////////////////////

#pragma region // iviewobject
	virtual HRESULT STDMETHODCALLTYPE GetColorSet( DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet) 
	{ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Freeze( DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze) 
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE Unfreeze(DWORD dwFreeze) 
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE SetAdvise( DWORD aspects, DWORD advf, IAdviseSink *pAdvSink);
	virtual HRESULT STDMETHODCALLTYPE GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink);
	virtual HRESULT STDMETHODCALLTYPE Draw(DWORD dwDrawAspect, LONG lindex,  void *pvAspect,  DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds, BOOL ( STDMETHODCALLTYPE *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
	virtual HRESULT STDMETHODCALLTYPE GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);
#pragma endregion

#pragma  region // igifoleobject
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath);
	virtual HRESULT __stdcall Refresh();
#pragma endregion

protected:
	LONG             m_dwRef;
	IAdviseSink*     m_pViewAdviseSink;
	IOleClientSite*  m_pClientSite;

	GifImage*        m_pGif;
};
