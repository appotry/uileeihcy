#pragma once
#include "LogItem.h"
#include "LogXmlParse.h"

class LogManager
{
public:
	LogManager(void);
	~LogManager(void);

public:
	bool    Create( String strXmlPath );
	bool    Load( String  strXmlPath );
	bool    TestCanLog(const String& name, LOG_LEVEL logLevel);
	void    Log( const String& name, LOG_LEVEL logLevel, const String& content );
	void    EnableLog( bool bOnOrOff );
	bool    IsEnable();
	bool    Save();
	bool    IsDirty() { return m_bDirty; }

	bool    AddLogItem(const String& strLogItem, bool bOnOrOff, LONG level, const String& bstrLogPath, LONG lLogFlag, LONG lFileLog );
	bool    ModifyLogItem(const String& strOldLogItemName, const String& strNewLogItemName, bool bOnOrOff, LONG level, const String& strLogPath, LONG lLogFlag, LONG lFileLog );
	bool    RemoveLogItem( const String& bstrLogItemName );

	int     GetLogItemCount();
	void    GetLogItem( int nIndex, ILogItem** ppRetVal );
	void    GetLogItem( const String& strLogItemName, ILogItem** ppRetVal );

private:
	

private:
	CRITICAL_SECTION   m_CriticalSection;   // 各个日志模块同步，用于当两个模块共同输出到同一个文件的情况
	ILogParse*         m_pLogParse;
	CPojo_Log          m_pojoLog;
	
	bool               m_bDirty;    // 是否需要保存
};
