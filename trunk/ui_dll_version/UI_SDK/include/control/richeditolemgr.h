#pragma once
class WindowlessRichEdit;

namespace UI
{
	

	// �洢ole�������ص���Ϣ
	class RichEditOleObjectItem : public IOleObject, public IViewObject2
	{
	public:
		RichEditOleObjectItem();
		virtual ~RichEditOleObjectItem();

		virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void  **ppvObject);
		virtual ULONG   STDMETHODCALLTYPE AddRef(void);
		virtual ULONG   STDMETHODCALLTYPE Release(void);

#pragma region // ole object 
		virtual HRESULT STDMETHODCALLTYPE SetHostNames(LPCOLESTR szContainerApp,LPCOLESTR szContainerObj) 
		{ return S_OK; }

		// ��һ��OLE������delete����ɾ����ʱ������á�Ȼ�󽫶�����ctrl+z�ٻ�ԭ��Ȼ����ɾ��ʱ���ֻ����һ��
		// �ú����Ƿ��ǿ�������ֹͣ����һ��GIF�ļ����� ctrl+z ��ԭʱ�������ʲô�ӿں�����
		virtual HRESULT STDMETHODCALLTYPE Close(DWORD dwSaveOption) 
		{
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
		virtual HRESULT STDMETHODCALLTYPE EnumAdvise(IEnumSTATDATA **ppenumAdvise) 
		{ return E_NOTIMPL; }
		virtual HRESULT STDMETHODCALLTYPE GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus) 
		{ return E_NOTIMPL;	}
		virtual HRESULT STDMETHODCALLTYPE SetColorScheme(LOGPALETTE *pLogpal)
		{ return E_NOTIMPL;	}

		virtual HRESULT STDMETHODCALLTYPE Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection);
		virtual HRESULT STDMETHODCALLTYPE Unadvise(DWORD dwConnection);
		virtual HRESULT STDMETHODCALLTYPE SetClientSite(IOleClientSite *pClientSite);
		virtual HRESULT STDMETHODCALLTYPE GetClientSite(IOleClientSite **ppClientSite);
		virtual HRESULT STDMETHODCALLTYPE GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
#pragma  endregion

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

	protected:
		LONG               m_dwRef;
//		IDataAdviseHolder* m_pDataAdviseHolder;
		IOleAdviseHolder*  m_pOleAdviseHolder;
		IAdviseSink*       m_pViewAdviseSink;
		IOleClientSite*    m_pClientSite;

	};

typedef map<DWORD, RichEditOleObjectItem*> OLEOITEMMAP;

	// ����richedit�е� ole�����Ӧ�Ľṹ���б�
	class RichEditOleObjectManager
	{
	public:
		RichEditOleObjectManager(WindowlessRichEdit* pRichEdit);
		~RichEditOleObjectManager();

		bool    AddOleItem(RichEditOleObjectItem* pItem);
		RichEditOleObjectItem*  GetOleItem(int dwUser);

	protected:
		WindowlessRichEdit*   m_pRichEdit;
		OLEOITEMMAP           m_mapOleObject;
		DWORD                 m_dwIndex;    // ��һ��item����������
	};
}