// Log.h : CLog 的声明

#pragma once
#include "resource.h"       // 主符号

#include "UILog_i.h"
#include "LogManager.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif



// CLog

class ATL_NO_VTABLE CLog :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLog, &CLSID_Log>,
	public IDispatchImpl<ILog, &IID_ILog, &LIBID_UILogLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CLog()
	{
		m_pLogManager = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LOG)

BEGIN_COM_MAP(CLog)
	COM_INTERFACE_ENTRY(ILog)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_pLogManager = new LogManager;
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_pLogManager)
		{
			delete m_pLogManager;
			m_pLogManager = NULL;
		}
	}

public:

	STDMETHOD(Create)(BSTR bstrXmlPath);
	STDMETHOD(Load)(BSTR bstrXmlPath);
	STDMETHOD(Log)(long lCookie, LOG_LEVEL level, BSTR bstrContent);
	STDMETHOD(EnableLog)(VARIANT_BOOL bEnable);
	STDMETHOD(Save)();
	STDMETHOD(IsDirty)(VARIANT_BOOL* pbDirty);
	STDMETHOD(IsEnable)(VARIANT_BOOL* pbEnable);
	STDMETHOD(TestCanLog)(long lCookie, LOG_LEVEL level, VARIANT_BOOL* pbRet);

	STDMETHOD(AddLogItem)(LOGITEM* pLogItem, long* pOutCookie);
	STDMETHOD(ModifyLogItem)(long lCookie, LOGITEM* pLogItem);
	STDMETHOD(DeleteLogItem)(long lCookie);

	STDMETHOD(GetLogItemCount)(LONG* pRetVal);
	STDMETHOD(GetLogItemByIndex)(LONG lIndex, ILogItem** ppRetVal);
	STDMETHOD(GetLogItemByName)(BSTR bstrLogItemName, ILogItem** ppRetVal);
	STDMETHOD(GetLogItemByCookie)(long lCookie, ILogItem **ppRetVal);

	
private:
	LogManager*   m_pLogManager;
};

OBJECT_ENTRY_AUTO(__uuidof(Log), CLog)
