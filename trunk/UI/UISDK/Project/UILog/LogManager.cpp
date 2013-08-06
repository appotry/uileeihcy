#include "StdAfx.h"
#include "LogManager.h"

LogManager::LogManager(void)
{
	::InitializeCriticalSection(&m_CriticalSection);
	m_pLogParse = NULL;
	m_bDirty = false;
}

LogManager::~LogManager(void)
{
	::DeleteCriticalSection(&m_CriticalSection);
	if( NULL != m_pLogParse )
	{
		m_pLogParse->Release();
		m_pLogParse = NULL;
	}
}

//
//	新建一个日志配置文件
//
bool LogManager::Create(BSTR bstrXmlPath)
{
	ATLASSERT(0);  // 这个函数会被调用吗?
	if (m_pLogParse)
	{
		OutputDebugString( _T("LogManager::Create  log parse already exist") );
		return false;
	}

	m_pLogParse = IParseClassFactory::CreateLogParseInstance(bstrXmlPath);
	if (false == m_pLogParse->Create())
	{
		OutputDebugString(_T("LogManager::Create  create log parse failed.") );
		return false;
	}

	return true;
}

//
//	保存当前日志相关配置
//
bool LogManager::Save()
{
	if( NULL == m_pLogParse )
		return false;

	if( false == m_bDirty )
		return true;

	bool bRet = m_pLogParse->Save(&m_resLog);
	if( true == bRet )
	{
		m_bDirty = false;
	}

	return bRet;
}
//
//	[Parameter]
//		strXmlPath
//			[in]	log xml 描述文件路径(全路径)
//
//	Return
//		成功：true，失败：false
//
bool LogManager::Load(BSTR bstrXmlPath)
{
	if (m_pLogParse)
	{
		OutputDebugString( _T("LogManager::Load  log parse already exist") );
		return false;
	}

	m_pLogParse = IParseClassFactory::CreateLogParseInstance(bstrXmlPath);
	return m_pLogParse->Load(&m_resLog);

#if 0
 	vector<LogItem*>   v;
 	if( false == parse.ParseLogItems( hLogItem, strXmlPath, v ) )
 		return false;
 
	size_t  size = v.size();
	for ( size_t i = 0; i < size; i++ )
	{
		m_mapLogItems.insert( make_pair(v[i]->logName, v[i]) );
	}
#endif
	return true;
}


//
//	测试一条日志能否输出
//
//	[Parameter]
//		name
//			[in]	模块名称，一句模块名称查找相应的LogItem
//		logLevel
//			[in]	要输出的日志级别
//
//	[Return]
//		可以输出返回true，否则返回false
//	
bool LogManager::TestCanLog(long nCookie, LOG_LEVEL logLevel)
{
	if (false == this->m_resLog.IsEnable())
		return false;

	LogItem* pItem = this->m_resLog.GetLogItemByCookie(nCookie);
	if (NULL == pItem)
		return false;

	if (VARIANT_FALSE == pItem->m_logitem.bEnable)
		return false;

	if (pItem->m_logitem.logLevel > logLevel)
		return false;

	return true;
}

//
//	一个模块，输出某个级别的日志内容
//
//	[Parameter]
//		name
//			[in]	模块名称，一句模块名称查找相应的LogItem
//		logLevel
//			[in]	要输出的日志级别
//		content
//			[in]	日志内容
//	
void LogManager::Log(long lCookie, LOG_LEVEL level, BSTR bstrContent)
{
	if (false == this->m_resLog.IsEnable())
		return;

	LogItem* pItem = this->m_resLog.GetLogItemByCookie(lCookie);
	if (NULL == pItem)
		return;

	EnterCriticalSection( &m_CriticalSection ); 
	pItem->Log(level, bstrContent);
	LeaveCriticalSection( &m_CriticalSection );
}

void LogManager::EnableLog(bool bEnable)
{
	this->m_resLog.SetEnable(bEnable);

	m_bDirty = true;
}

bool LogManager::IsEnable()
{
	return this->m_resLog.IsEnable();
}

int LogManager::AddLogItem(LOGITEM* pLogItem)
{
	int nRet = m_resLog.AddLogItem(pLogItem);

	if (-1 != nRet)
	{
		m_bDirty = true;
	}

	return nRet;
}

bool LogManager::ModifyLogItem(long lCookie, LOGITEM* pLogItem)
{
	bool bRet = m_resLog.ModifyLogItem(lCookie, pLogItem);

	if (bRet)
	{
		m_bDirty = true;
		return true;
	}
	else
		return false;
}
bool LogManager::DeleteLogItem(long lCookie)
{
// 	bool bRet = m_resLog.DeleteLogItem(lCookie);
// 	if (bRet)
// 	{
// 		m_bDirty = true;
// 	}
// 
// 	return bRet;
	return false;
}

int LogManager::GetLogItemCount()
{
	return m_resLog.GetLogItemCount();
}

ILogItem* LogManager::GetLogItemByIndex(int nIndex)
{
	return m_resLog.GetLogItemByIndex(nIndex);
}

ILogItem* LogManager::GetLogItemByName(BSTR bstrLogItemName)
{
	return m_resLog.GetLogItemByName(bstrLogItemName);
}

ILogItem* LogManager::GetLogItemByCookie(long lCookie)
{
	return m_resLog.GetLogItemByCookie(lCookie);
}