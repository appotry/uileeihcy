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
#	define LOG_FALG_DBGVIEW           0x0004    // ����־ͬʱ��ӡ��dbgView

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
		bSwitch = true;
		uLogFileFlag = 0;
		uLogFlag = 0;
	}
	~LogItem()
	{
		this->_releaseConsole();

		if( f.is_open() )
			f.close();
	}
public:

	HRESULT __stdcall GetSwitch( LONG* pRetVal);
	HRESULT __stdcall GetName  ( BSTR* pRetVal);
	HRESULT __stdcall GetPath  ( BSTR* pRetVal);
	HRESULT __stdcall GetLevel ( LONG* pRetVal);
	HRESULT __stdcall GetLogFlag ( LONG* pRetVal);
	HRESULT __stdcall GetFileFlag( LONG* pRetVal);

public:
	bool           Init();
	void           Log( LOG_LEVEL level, const String& content );

	static 
	String&        LevelToString( const LOG_LEVEL& l, bool bAlign=true );
	static 
	long           StringToLevel( const String& level );

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
	bool               bSwitch;         // ��־�Ƿ��
	static  bool       bConsoleAlloc;   // ����̨�Ƿ��ʼ��(һ������ֻ��һ������̨)
	String             logName;         // ��־ģ������
	String             logFile;         // ��־���·��
	LOG_LEVEL          logLevel;        // ��־����ȼ�
	UINT               uLogFlag;        // ��־���Flag
	UINT               uLogFileFlag;    // ��־�ļ�Flag

#ifdef _UNICODE
	wfstream		   f;
#else
	fstream			   f;
#endif

};


class CPojo_Log
{
public:
	CPojo_Log(){};
	~CPojo_Log()
	{
		map<String, LogItem*>::iterator  iter = m_mapLogItems.begin();
		map<String, LogItem*>::iterator  iterEnd = m_mapLogItems.end();

		for( ; iter != iterEnd; iter++ )
		{
			delete iter->second;
		}

		m_mapLogItems.clear();
	}
	bool     IsSwitchOn(){ return m_bSwitch; }
	void     SetSwitch( bool bOnOrOff ) { m_bSwitch = bOnOrOff; }
	
	int      GetLogItemCount() { return m_mapLogItems.size(); }
	void     GetEnum(map<String, LogItem*>::iterator* iter, map<String, LogItem*>::iterator* iterEnd )
	{
		*iter = m_mapLogItems.begin();
		*iterEnd = m_mapLogItems.end();
	}
	ILogItem* GetLogItem( int nIndex ) // ���ص�ֵ��Ҫ����release�����ͷ�
	{
		map<String, LogItem*>::iterator iter = m_mapLogItems.begin();
		map<String, LogItem*>::iterator iterEnd = m_mapLogItems.end();

		for ( int i=0; iter!=iterEnd; iter++,i++ )
		{
			if( i== nIndex )
			{
				ILogItem* pRetVal = iter->second;
				pRetVal->AddRef();
				return pRetVal;
			}
		}
		return NULL;
	}
	LogItem* GetLogItem( const String& strModuleName )
	{
		if( 0 == m_mapLogItems.count(strModuleName) )
		{
			return NULL;
		}
		return m_mapLogItems[strModuleName];
	}
	bool AddLogItem( const String& strModule, bool bOnOrOff, LONG level, const String& strLogPath, LONG lLogFlag, LONG lFileLog )
	{
		if( NULL != GetLogItem( strModule) )
		{
			return false;
		}

		LogItem* pItem = new LogItem;
		pItem->logName = strModule;
		pItem->bSwitch = bOnOrOff;
		pItem->logLevel = (LOG_LEVEL)level;
		pItem->logFile = strLogPath;
		pItem->uLogFlag = lLogFlag;
		pItem->uLogFileFlag = lFileLog;

		if( false == pItem->Init() )
		{
			delete pItem;
			return false;
		}

		m_mapLogItems[strModule] = pItem;
		return true;
	}

	bool ModifyLogItem(const String& strOldModuleName, const String& strNewModuleName, bool bOnOrOff, LONG level, const String& strLogPath, LONG lLogFlag, LONG lFileLog )
	{
		LogItem* pItem = GetLogItem( strOldModuleName);
		if( NULL == pItem )
		{
			return false;
		}

		pItem->logName = strNewModuleName;
		pItem->bSwitch = bOnOrOff;
		pItem->logLevel = (LOG_LEVEL)level;
		pItem->logFile = strLogPath;
		pItem->uLogFlag = lLogFlag;
		pItem->uLogFileFlag = lFileLog;

		m_mapLogItems.erase(strOldModuleName);
		m_mapLogItems[strNewModuleName] = pItem;
		return true;
	}

private:
	bool      m_bSwitch;           // ��־�ܿ���

	map<String, LogItem*>  m_mapLogItems;   // ���е���־�����
};