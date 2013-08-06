// Log.h : CLog ������

#pragma once
#include "resource.h"       // ������

#include "UILog_i.h"
#include "LogManager.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
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
