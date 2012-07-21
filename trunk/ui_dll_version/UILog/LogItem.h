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
#	define LOG_FALG_DBGVIEW           0x0004    // 将日志同时打印到dbgView

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
	bool               bSwitch;         // 日志是否打开
	static  bool       bConsoleAlloc;   // 控制台是否初始化(一个程序只有一个控制台)
	String             logName;         // 日志模块名称
	String             logFile;         // 日志输出路径
	LOG_LEVEL          logLevel;        // 日志输出等级
	UINT               uLogFlag;        // 日志输出Flag
	UINT               uLogFileFlag;    // 日志文件Flag

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
	ILogItem* GetLogItem( int nIndex ) // 返回的值需要调用release进行释放
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
	bool      m_bSwitch;           // 日志总开关

	map<String, LogItem*>  m_mapLogItems;   // 所有的日志输出项
};