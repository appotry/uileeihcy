#pragma once


/*
**	����
**		1. ֧�ֶ��ģ�����־���
**		2. ֧�������Console��ʵʱ�鿴��־
**      3. ֧��ѭ�������־
**      4. ֧����������������־
**		5. xml�Ĵ�Сд�޹�
**  
**  δʵ�ֹ���
**		1. ��־��������
**		2. ����ʱ�޸����ù���
**		3. �����
**
**	Remark
**		��˭����д��־�������߻���ʵ���ߣ�
**		==����ʵ���߸���д��־����Ϊֻ��ʵ���߲�֪�������������
**			������ֻ��Ҫ�����жϷ���ֵ����
**
*/




#if 0 // �����嵽idl�У������ⲿ��������
// ��־���flag
#	define LOG_FLAG_THREADID          0x0001    // ��ӡ���߳�ID
#	define LOG_FLAG_CONSOLE           0x0002    // ����־ͬʱ��ӡ������̨
#	define LOG_FLAG_DBGVIEW           0x0004    // ����־ͬʱ��ӡ��dbgView

// ����ļ����flag
// ���8λ�����ڱ�־LOG_FLAG_FILE_LOOP������
#	define LOG_FLAG_FILE_DISABLE      0x0001    // ��������ļ�
#	define LOG_FLAG_FILE_SIMPLE       0x0002    // ÿ��д֮ǰɾ���Ѿ����ڵ��ļ�����
#	define LOG_FLAG_FILE_TIME         0x0004    // ��־������ϵ�ǰʱ��
#	define LOG_FLAG_FILE_LOOP         0x0008    // ����ļ�ѭ��ʹ��
#endif

#define GET_FILE_LOOP_COUNT(n)     (n>>24)   // ����LOG_FLAG_FILE_LOOP��־��Ч����=3,���־ѭ��ʹ�� file0.txt, file1.txt, file2.txt, file0.txt ...

class ILogItemImpl : public ILogItem
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject) { return E_NOTIMPL; }
	virtual ULONG STDMETHODCALLTYPE AddRef( void) { return 1; }
	virtual ULONG STDMETHODCALLTYPE Release( void) { return 0; }

};

// Ҫ������־��������ݿ��ܲ�ֹһ���ÿһ��п��������һ��ר�ŵ��ļ��С�
class  LogItem : public ILogItemImpl
{
public:
	LogItem()
	{
		memset(&m_logitem, 0, sizeof(LOGITEM));

		m_logitem.bEnable= VARIANT_TRUE;
		m_logitem.uLogFileLoopCount = 10;
	}
	~LogItem()
	{
		if (m_logitem.logFile)
			SysFreeString(m_logitem.logFile);
		if (m_logitem.logName)
			SysFreeString(m_logitem.logName);

		this->_releaseConsole();

		if (f.is_open())
			f.close();
	}
public:

	HRESULT __stdcall IsEnable( VARIANT_BOOL* pbSwitch);
	HRESULT __stdcall GetName  ( BSTR* pRetVal);
	HRESULT __stdcall GetPath  ( BSTR* pRetVal);
	HRESULT __stdcall GetLevel ( LONG* pRetVal);
	HRESULT __stdcall GetLogFlag ( LONG* pRetVal);
	HRESULT __stdcall GetFileFlag( LONG* pRetVal);
	HRESULT __stdcall GetCookie( LONG* pRetVal);

public:
	bool           Init();
	void           Log(LOG_LEVEL level, BSTR bstrContent);

	static String&  LevelToString( const LOG_LEVEL& l, bool bAlign=true );
	static long     StringToLevel( const String& level );

private:
	template<class T>
	void           _logToStream( T& o, LOG_LEVEL level, const String& content );
	void           _logToDbgview( LOG_LEVEL level, const String& content );
	String&        _curTime();
	String&        _curTime2();
	void           _initConsole();
	void           _releaseConsole();
	void           _appendIndexToPath(String& strPath, byte index);

public:
	static  bool       bConsoleAlloc;   // ����̨�Ƿ��ʼ��(һ������ֻ��һ������̨)
	LOGITEM            m_logitem;

//	bool               bSwitch;         // ��־�Ƿ��
//	String             logName;         // ��־ģ������
//	String             logFile;         // ��־���·��
//	LOG_LEVEL          logLevel;        // ��־����ȼ�
//	UINT               uLogFlag;        // ��־���Flag
//	UINT               uLogFileFlag;    // ��־�ļ�Flag
//	UINT               uLogFileLoopCount; // �ļ�ѭ������
//	int                uCookie;         // log item����

#ifdef _UNICODE
	wfstream		   f;
#else
	fstream			   f;
#endif

};


class LogRes
{
public:
	LogRes()
	{
		m_nCookieIndex = 0;
	};
	~LogRes()
	{
		map<int, LogItem*>::iterator  iter = m_mapLogItems.begin();
		map<int, LogItem*>::iterator  iterEnd = m_mapLogItems.end();

		for (; iter != iterEnd; iter++)
		{
			delete iter->second;
		}

		m_mapLogItems.clear();
	}
	bool     IsEnable(){ return m_bEnable; }
	void     SetEnable( bool bEnable ) { m_bEnable = bEnable; }
	
	int      GetLogItemCount() { return m_mapLogItems.size(); }
	LogItem* GetLogItemByIndex(int nIndex) // ���ص�ֵ��Ҫ����release�����ͷ�
	{
		map<int, LogItem*>::iterator iter = m_mapLogItems.begin();
		map<int, LogItem*>::iterator iterEnd = m_mapLogItems.end();

		for (int i=0; iter!=iterEnd; iter++,i++)
		{
			if (i== nIndex)
			{
				return iter->second;
			}
		}
		return NULL;
	}
	LogItem*  GetLogItemByCookie(int nCookie) // ���ص�ֵ��Ҫ����release�����ͷ�
	{
		map<int, LogItem*>::iterator iter = m_mapLogItems.find(nCookie);
		if (iter == m_mapLogItems.end())
		{
			return NULL;
		}
		return iter->second;
	}
	LogItem* GetLogItemByName(BSTR bstrModuleName)
	{
		if (NULL == bstrModuleName)
			return NULL;

		map<int, LogItem*>::iterator iter = m_mapLogItems.begin();
		map<int, LogItem*>::iterator iterEnd = m_mapLogItems.end();

		for (; iter!=iterEnd; iter++)
		{
			if (0 == _tcscmp(bstrModuleName, iter->second->m_logitem.logName))
			{
				return iter->second;
			}
		}
		return NULL;
	}
	int AddLogItem(LOGITEM* pLogItem)
	{
		if (NULL != GetLogItemByName(pLogItem->logName))
			return -1;

		LogItem* pItem = new LogItem;
		pItem->m_logitem.logName = SysAllocString(pLogItem->logName);
		pItem->m_logitem.bEnable = pLogItem->bEnable;
		pItem->m_logitem.logLevel = pLogItem->logLevel;
		pItem->m_logitem.logFile = SysAllocString(pLogItem->logFile);
		pItem->m_logitem.uLogFlag = pLogItem->uLogFlag;
		pItem->m_logitem.uLogFileFlag = pLogItem->uLogFileFlag;
		pItem->m_logitem.uLogFileLoopCount = pLogItem->uLogFileLoopCount;
		pItem->m_logitem.uCookie = m_nCookieIndex+1;

		if (false == pItem->Init())
		{
			delete pItem;
			return -1;
		}

		m_nCookieIndex++;
		m_mapLogItems[m_nCookieIndex] = pItem;
		return m_nCookieIndex;
	}

	bool ModifyLogItem(long lCookie, LOGITEM* pLogItem)
	{
		LogItem* pItem = GetLogItemByCookie(lCookie);
		if (NULL == pItem)
			return false;

		if (pItem->m_logitem.logName)
			::SysFreeString(pItem->m_logitem.logName);
		if (pItem->m_logitem.logFile)
			::SysFreeString(pItem->m_logitem.logFile);

		pItem->m_logitem.logName =  SysAllocString(pLogItem->logName);
		pItem->m_logitem.bEnable = pLogItem->bEnable;
		pItem->m_logitem.logLevel = pLogItem->logLevel;
		pItem->m_logitem.logFile = SysAllocString(pLogItem->logFile);
		pItem->m_logitem.uLogFlag = pLogItem->uLogFlag;
		pItem->m_logitem.uLogFileFlag = pLogItem->uLogFileFlag;
		pItem->m_logitem.uLogFileLoopCount = pLogItem->uLogFileLoopCount;

//		pItem->uCookie = nCookie;
//		m_mapLogItems[nCookie] = pLogItem;
		return true;
	}

public:
	bool      m_bEnable;           // ��־�ܿ���
	int       m_nCookieIndex;

	map<int, LogItem*>  m_mapLogItems;   // ���е���־�����
};