#pragma once


/*
**	功能
**		1. 支持多个模块的日志输出
**		2. 支持输出到Console上实时查看日志
**      3. 支持循环输出日志
**      4. 支持添加日期名输出日志
**		5. xml的大小写无关
**  
**  未实现功能
**		1. 日志分析工具
**		2. 运行时修改配置功能
**		3. 宏调用
**
**	Remark
**		由谁负责写日志？调用者还是实现者？
**		==》由实现者负责写日志，因为只有实现者才知道具体错在哪里
**			调用者只需要负责判断返回值即可
**
*/




#if 0 // 被定义到idl中，便于外部程序引用
// 日志相关flag
#	define LOG_FLAG_THREADID          0x0001    // 打印本线程ID
#	define LOG_FLAG_CONSOLE           0x0002    // 将日志同时打印到控制台
#	define LOG_FLAG_DBGVIEW           0x0004    // 将日志同时打印到dbgView

// 输出文件相关flag
// 最高8位被用于标志LOG_FLAG_FILE_LOOP的数量
#	define LOG_FLAG_FILE_DISABLE      0x0001    // 不输出到文件
#	define LOG_FLAG_FILE_SIMPLE       0x0002    // 每次写之前删除已经存在的文件内容
#	define LOG_FLAG_FILE_TIME         0x0004    // 日志名后加上当前时间
#	define LOG_FLAG_FILE_LOOP         0x0008    // 输出文件循环使用
#endif

#define GET_FILE_LOOP_COUNT(n)     (n>>24)   // 仅当LOG_FLAG_FILE_LOOP标志有效，如=3,则标志循环使用 file0.txt, file1.txt, file2.txt, file0.txt ...

class ILogItemImpl : public ILogItem
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject) { return E_NOTIMPL; }
	virtual ULONG STDMETHODCALLTYPE AddRef( void) { return 1; }
	virtual ULONG STDMETHODCALLTYPE Release( void) { return 0; }

};

// 要进行日志输出的内容可能不止一项，而每一项都有可能输出到一个专门的文件中。
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
	static  bool       bConsoleAlloc;   // 控制台是否初始化(一个程序只有一个控制台)
	LOGITEM            m_logitem;

//	bool               bSwitch;         // 日志是否打开
//	String             logName;         // 日志模块名称
//	String             logFile;         // 日志输出路径
//	LOG_LEVEL          logLevel;        // 日志输出等级
//	UINT               uLogFlag;        // 日志输出Flag
//	UINT               uLogFileFlag;    // 日志文件Flag
//	UINT               uLogFileLoopCount; // 文件循环次数
//	int                uCookie;         // log item索引

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
	LogItem* GetLogItemByIndex(int nIndex) // 返回的值需要调用release进行释放
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
	LogItem*  GetLogItemByCookie(int nCookie) // 返回的值需要调用release进行释放
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
	bool      m_bEnable;           // 日志总开关
	int       m_nCookieIndex;

	map<int, LogItem*>  m_mapLogItems;   // 所有的日志输出项
};