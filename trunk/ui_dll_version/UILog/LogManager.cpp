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
//	�½�һ����־�����ļ�
//
bool LogManager::Create( String strXmlPath )
{
	if( NULL != m_pLogParse )
	{
		OutputDebugString( _T("LogManager::Create  log parse already exist") );
		return false;
	}

	m_pLogParse = IParseClassFactory::CreateLogParseInstance( strXmlPath );
	if( false == m_pLogParse->Create() )
	{
		OutputDebugString(_T("LogManager::Create  create log parse failed.") );
		return false;
	}

	return true;
}

//
//	���浱ǰ��־�������
//
bool LogManager::Save()
{
	if( NULL == m_pLogParse )
		return false;

	if( false == m_bDirty )
		return true;

	bool bRet = m_pLogParse->Save(&m_pojoLog);
	if( true == bRet )
	{
		m_bDirty = false;
	}

	return bRet;
}
//
//	[Parameter]
//		strXmlPath
//			[in]	log xml �����ļ�·��(ȫ·��)
//
//	Return
//		�ɹ���true��ʧ�ܣ�false
//
bool LogManager::Load( String  strXmlPath )
{
	if( NULL != m_pLogParse )
	{
		OutputDebugString( _T("LogManager::Load  log parse already exist") );
		return false;
	}

	m_pLogParse = IParseClassFactory::CreateLogParseInstance( strXmlPath );
	return m_pLogParse->Load( &m_pojoLog );

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
//	����һ����־�ܷ����
//
//	[Parameter]
//		name
//			[in]	ģ�����ƣ�һ��ģ�����Ʋ�����Ӧ��LogItem
//		logLevel
//			[in]	Ҫ�������־����
//
//	[Return]
//		�����������true�����򷵻�false
//	
bool LogManager::TestCanLog(const String& name, LOG_LEVEL logLevel)
{
	if ( false == this->m_pojoLog.IsSwitchOn() )
		return false;

	LogItem* pItem = this->m_pojoLog.GetLogItem(name);
	if ( NULL == pItem )
		return false;

	if( false == pItem->bSwitch )
		return false;

	if( pItem->logLevel > logLevel )
		return false;

	return true;
}

//
//	һ��ģ�飬���ĳ���������־����
//
//	[Parameter]
//		name
//			[in]	ģ�����ƣ�һ��ģ�����Ʋ�����Ӧ��LogItem
//		logLevel
//			[in]	Ҫ�������־����
//		content
//			[in]	��־����
//	
void LogManager::Log( const String& name, LOG_LEVEL logLevel, const String& content)
{
	if ( false == this->m_pojoLog.IsSwitchOn() )
		return;

	LogItem* pItem = this->m_pojoLog.GetLogItem(name);
	if ( NULL == pItem )
		return;

	EnterCriticalSection( &m_CriticalSection ); 
	pItem->Log( logLevel, content );
	LeaveCriticalSection( &m_CriticalSection );
}

void LogManager::EnableLog( bool bOnOrOff )
{
	this->m_pojoLog.SetSwitch(bOnOrOff);

	m_bDirty = true;
}

bool LogManager::IsEnable()
{
	return this->m_pojoLog.IsSwitchOn();
}

bool LogManager::AddLogItem(const String& strLogItem, bool bOnOrOff, LONG level, const String& strLogPath, LONG lLogFlag, LONG lFileLog )
{
	bool bRet = m_pojoLog.AddLogItem(strLogItem, bOnOrOff, level, strLogPath, lLogFlag, lFileLog );

	if( bRet )
	{
		m_bDirty = true;
		return true;
	}
	else
		return false;
}

bool LogManager::ModifyLogItem(const String& strOldLogItemName, const String& strNewLogItemName, bool bOnOrOff, LONG level, const String& strLogPath, LONG lLogFlag, LONG lFileLog )
{
	bool bRet = m_pojoLog.ModifyLogItem(strOldLogItemName, strNewLogItemName, bOnOrOff, level, strLogPath, lLogFlag, lFileLog );

	if( bRet )
	{
		m_bDirty = true;
		return true;
	}
	else
		return false;
}
bool LogManager::RemoveLogItem( const String& bstrLogItemName )
{
	m_bDirty = true;
	return false;
}

int LogManager::GetLogItemCount()
{
	return m_pojoLog.GetLogItemCount();
}

void LogManager::GetLogItem( int nIndex, ILogItem** ppRetVal )
{
	*ppRetVal = m_pojoLog.GetLogItem( nIndex );
}

void LogManager::GetLogItem( const String& strLogItemName, ILogItem** ppRetVal )
{
	*ppRetVal = (ILogItem*) m_pojoLog.GetLogItem(strLogItemName);
}