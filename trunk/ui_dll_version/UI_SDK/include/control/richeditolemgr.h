#pragma once
#include "oledataobject.h"

//////////////////////////////////////////////////////////////////////////
//
//                      用于richedit中插入的ole对象
//
//////////////////////////////////////////////////////////////////////////


//  Q1. 调用InsertObject时，一定要传递一个Storage指针吗？
//  A1. 如果传递一个NULL，则在复制该对象后，将无法粘贴。内部的原因也还没有搞清楚
//      ILockBytes，它是一个Storage后存储介质之间的桥梁，StgCreateDocfileOnILockBytes
//      代表在内存中创建一个存储对象。相应的StgCreateDocFile表示使用文件创建一个
//      存储对象
//
//   Q2. IDataObject怎么被其它进程访问的？
//   A2. 从目前查到的资料中显示，应该是Marshal的结果
//
//   Q3. 进程退出后，为什么还能进行拷贝粘贴?
//   A3. 其实这全靠OleFlushClipboard。它会将当前剪贴板中的IDataObject进行复制，创建一个
//       临时的IDataObject，并释放之前的IDataObject对象。因此在程序退出前应该调用一下
//       这个函数
//
//
// 更多的实现细节可以参考atl的源代码：CComControlBase  IOleObjectImpl
//
//

namespace UI
{
	class WindowlessRichEdit;

	class IRichEditOleObjectItem
	{
	public:
		virtual ~IRichEditOleObjectItem() = 0{};
		virtual HRESULT  GetOleObject(IOleObject** ppOleObject, bool bAddRef=true) = 0;	
		virtual HRESULT  GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj) = 0;
	};

	// 存储外部自使用COM实现的OLE对象
	class RichEditOleObjectItem_Com : public IRichEditOleObjectItem
	{
	public:
		RichEditOleObjectItem_Com();
		~RichEditOleObjectItem_Com();

	public:
		virtual HRESULT  GetOleObject(IOleObject** ppOleObject, bool bAddRef=true);
		virtual HRESULT  GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj);
		HRESULT  Attach(CLSID  clsid);
	private:
		IOleObject*   m_pOleObject;
	};

	// 存储内部自己实现的ole对象的相关的信息
	class RichEditOleObjectItem_Inner : public IOleObject, public IViewObject2, public IRichEditOleObjectItem
	{
	public:
		RichEditOleObjectItem_Inner();
		virtual ~RichEditOleObjectItem_Inner();

#pragma region // IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject);
		virtual ULONG   STDMETHODCALLTYPE AddRef(void);
		virtual ULONG   STDMETHODCALLTYPE Release(void);
#pragma endregion

#pragma region // ole object 
		virtual HRESULT STDMETHODCALLTYPE SetHostNames(LPCOLESTR szContainerApp,LPCOLESTR szContainerObj) 
		{ return S_OK; }

		// 将一个OLE对象用delete键给删除掉时，会调用。然后将对象用ctrl+z再还原，然后再删除时，又会调用一次
		// 该函数是否是可以用于停止播放一个GIF文件？那 ctrl+z 还原时，会调用什么接口函数？
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

#pragma region  // 实现父类的虚函数
		virtual HRESULT  GetOleObject(IOleObject** ppOleObject, bool bAddRef=true);
#pragma  endregion

#pragma region  // 子类扩展时需要实现的函数
		virtual HRESULT OnDraw(HDC hDC, RECT* prc) = 0;
		virtual HRESULT OnGetSize(SIZE* pSize) = 0;
#pragma endregion

	protected:
		LONG               m_dwRef;
//		IDataAdviseHolder* m_pDataAdviseHolder;
		IOleAdviseHolder*  m_pOleAdviseHolder;
		IAdviseSink*       m_pViewAdviseSink;
		IOleClientSite*    m_pClientSite;
	};

typedef map<DWORD, IRichEditOleObjectItem*> OLEOITEMMAP;
//class GifImageItemMgr;

	// 管理richedit中的 ole对象对应的结构体列表
	class RichEditOleObjectManager
	{
	public:
		RichEditOleObjectManager(WindowlessRichEdit* pRichEdit);
		~RichEditOleObjectManager();

		bool    AddOleItem(IRichEditOleObjectItem* pItem);
		IRichEditOleObjectItem*  GetOleItem(int dwUser);
//		GifImageItemMgr*  GetGifImageItemMgr() { return m_pGifImageItemMgr; }
		CPojo_Gif*    GetGifManager() { return &m_gifMgr; }

	protected:
		WindowlessRichEdit*   m_pRichEdit;

		OLEOITEMMAP           m_mapOleObject;
		DWORD                 m_dwIndex;    // 下一个item的索引计数

//		GifImageItemMgr*      m_pGifImageItemMgr;
		CPojo_Gif             m_gifMgr;
	};

}