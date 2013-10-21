#include "StdAfx.h"
#include "3rd/markup/markup.h"
#include "LogXmlParse.h"
void Split(const String& str, TCHAR szSep, vector<String>& vRet);

//
// log.xml
//
#define XML_LOG                   _T("log")
#define XML_LOG_ITEM              _T("item")
#define XML_LOG_ENABLE            _T("enable")
#define XML_LOG_ENABLE_ON         _T("true")
#define XML_LOG_ENABLE_OFF        _T("false")
#define XML_LOG_NAME              _T("name")
#define XML_LOG_PATH              _T("path")
#define XML_LOG_LEVEL             _T("level")
#define XML_LOG_FLAG              _T("flag")
#define XML_LOG_FILEFLAG          _T("fileflag")
#define XML_FLAG_SEPARATOR        _T('|')            // xml�еķָ��������ı����뷽ʽ: left|top|vcenter
#define XML_LOGFLAG_THREADID      _T("threadid")
#define XML_LOGFLAG_CONSOLE       _T("console")
#define XML_LOGFLAG_DBGVIEW       _T("dbgview")

#define XML_LOGFILEFLAG_DISABLE   _T("disable")
#define XML_LOGFILEFLAG_SIMPLE    _T("simple")
#define XML_LOGFILEFLAG_TIME      _T("time")
#define XML_LOGFILEFLAG_LOOP      _T("loop")
#define XML_LOG_FILE_LOOPCOUNT    _T("loopcount")

/*
<root>                                                    // �����ƣ������������ơ�����ȥ������µ�<log>��Ϣ 
    <log  switch="on/off">                                // log��switch������������ģ��Ŀ���״̬
	        <item                                         //
			 name="moduleName"                            // һ����־����ģ�������
			 switch="on/off"                              // ��ģ����־�������(Ĭ��on)
             level="debug/info/warn/error/fator"          // ����־�������������(Ĭ��warn)  
             path="log.log"                               // ��־�ļ�·�������ڶ����ַ�Ϊ:ʱ��Ĭ��Ϊ����·��������Ϊ���·��
             flag="0x0"                                   // ��־flag��������0x��ͷ (��logitem.h)
             fileflag="0x0"                               // ����ļ�flag��������0x��ͷ (��logitem.h)
        />        
    </log>
</root>

*/



CLogXmlParse::CLogXmlParse(const TCHAR* szDataSource) : ILogParse(szDataSource)
{
	
}

CLogXmlParse::~CLogXmlParse(void)
{
}

#define  XML_UI _T("UI")
bool CLogXmlParse::Create()
{
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		return Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI );
	}
	return true;
}
bool CLogXmlParse::Save( LogRes*  pLog )
{
	if( NULL == pLog )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		::OutputDebugString( _T("CLogXmlParse::Save  load xml failed") );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == xml.FindElem( ) )                      break;    // ����root��������
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_LOG) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_LOG ))             break;

		// ��־�ܿ���
		if( false == xml.SetAttrib( XML_LOG_ENABLE, pLog->IsEnable()?XML_LOG_ENABLE_ON:XML_LOG_ENABLE_OFF ) )  break;
		if( false == xml.IntoElem() )                       break;

		bool bLoopRet = true;

		map<int, LogItem*>::iterator  iter = pLog->m_mapLogItems.begin();
		map<int, LogItem*>::iterator  iterEnd = pLog->m_mapLogItems.end();
		for ( ; iter != iterEnd; iter++ )
		{
			TCHAR szFlag[32] = _T("");

			String strModuleName = iter->second->m_logitem.logName;
			String strLogLevel   = LogItem::LevelToString(iter->second->m_logitem.logLevel, false);
			String strEnable     = iter->second->m_logitem.bEnable? XML_LOG_ENABLE_ON : XML_LOG_ENABLE_OFF;
			String strPath       = iter->second->m_logitem.logFile;
			
			_stprintf( szFlag, _T("0x%x"), iter->second->m_logitem.uLogFlag );
			String strLogFlag = szFlag;

			_stprintf( szFlag, _T("0x%x"), iter->second->m_logitem.uLogFileFlag );
			String strLogFileFlag = szFlag;

			if( false == xml.InsertElem( XML_LOG_ITEM ))       { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_NAME, strModuleName.c_str()) ) { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_ENABLE, strEnable.c_str()) )  { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_LEVEL, strLogLevel.c_str()) )  { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_PATH, strPath.c_str()) )  { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_FLAG, strLogFlag.c_str()) ) { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_FILEFLAG, strLogFileFlag.c_str()) ) { bLoopRet = false;  break; }
		}

		if( !bLoopRet )
			break;

		// �����ļ�
		if( false == xml.Save(m_strDataSource))              break;

		bRet = true;
	}
	while(0);
	
	if( false == bRet )
	{
		::OutputDebugString(_T("CLogXmlParse::Save  failed!"));
	}

	return true;
}

bool CLogXmlParse::Load(LogRes*  pLog)
{
	if (NULL == pLog)
		return false;

	CMarkup  xml;
	if (false == xml.Load(m_strDataSource))
	{
        TCHAR szText[256] = _T("");
        _stprintf(szText, _T("%s load xml failed: %s\n"), _T(__FUNCTION__), m_strDataSource.c_str());
		::OutputDebugString(szText);
		return false;
	}

	// ͳһ�л�ΪСд��ĸ����ֹ���ô�Сд����
	String strXmlDoc = xml.GetDoc();
//	transform(strXmlDoc.begin(),strXmlDoc.end(),strXmlDoc.begin(),tolower);
	xml.SetDoc(strXmlDoc);

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == xml.FindElem())                        break;    // ����root��������
		if (false == xml.IntoElem())                        break;

		if (false == xml.FindElem(XML_LOG))                 break;

		String strGlobalSwitch = xml.GetAttrib(XML_LOG_ENABLE);
		if (strGlobalSwitch.empty() || strGlobalSwitch == _T("1") || strGlobalSwitch == _T("true") )
			pLog->SetEnable(true);
		else
			pLog->SetEnable(false);

		if (false == xml.IntoElem())                        break;
		while (1)
		{
			if (false == xml.FindElem(XML_LOG_ITEM))        break;
			
			String logName = xml.GetAttrib(XML_LOG_NAME);
			String logFile = xml.GetAttrib(XML_LOG_PATH);
			
			if (Util::IsFullPath( logFile.c_str()))
			{
			}
			else if (logFile.length() > 0)
			{
				TCHAR szModulePath[MAX_PATH] = _T("");

				GetModuleFileName( g_hInstance, szModulePath, MAX_PATH );
				TCHAR* szTemp = _tcsrchr(szModulePath, _T('\\'));
				if( NULL != szTemp )
					*(szTemp+1) = 0;

				TCHAR szFullPath[MAX_PATH] = _T("");
				Util::CalcFullPathByRelative(szModulePath, logFile.c_str(), szFullPath );
				logFile = szFullPath;
			}

			String strLogFlag = xml.GetAttrib(XML_LOG_FLAG);
			long uLogFlag = 0;
//			_stscanf( strLogFlag.c_str(), _T("0x%x"), &uLogFlag);
			vector<String>  vecFlags;
			Split(strLogFlag, XML_FLAG_SEPARATOR, vecFlags);
			int nSize = (int)vecFlags.size();
			for (int i = 0; i < nSize; i++)
			{
				if (vecFlags[i] == XML_LOGFLAG_CONSOLE)
				{
					uLogFlag |= LOG_FLAG_CONSOLE;
				}
				else if (vecFlags[i] == XML_LOGFLAG_THREADID)
				{
					uLogFlag |= LOG_FLAG_THREADID;
				}
				else if (vecFlags[i] == XML_LOGFLAG_DBGVIEW)
				{
					uLogFlag |= LOG_FLAG_DBGVIEW;
				}
			}

			String strLogFileFlag = xml.GetAttrib(XML_LOG_FILEFLAG);
			long uLogFileFlag = 0;
			long uLogFileLoopCount = 0;
//			_stscanf( strLogFileFlag.c_str(), _T("0x%x"), &uLogFileFlag );
			
			vector<String>  vecFileFlags;
			Split(strLogFileFlag, XML_FLAG_SEPARATOR, vecFileFlags);
			nSize = (int)vecFileFlags.size();
			for (int i = 0; i < nSize; i++)
			{
				if (vecFileFlags[i] == XML_LOGFILEFLAG_DISABLE)
				{
					uLogFileFlag |= LOG_FLAG_FILE_DISABLE;
				}
				else if (vecFileFlags[i] == XML_LOGFILEFLAG_SIMPLE)
				{
					uLogFileFlag |= LOG_FLAG_FILE_SIMPLE;
				}
				else if (vecFileFlags[i] == XML_LOGFILEFLAG_TIME)
				{
					uLogFileFlag |= LOG_FLAG_FILE_TIME;
				}
				else if (vecFileFlags[i] == XML_LOGFILEFLAG_LOOP)
				{
					uLogFileFlag |= LOG_FLAG_FILE_LOOP;
					uLogFileLoopCount = _ttoi(xml.GetAttrib(XML_LOG_FILE_LOOPCOUNT).c_str());
				}
			}

			String strLevel = xml.GetAttrib(XML_LOG_LEVEL);
			long logLevel = LogItem::StringToLevel(strLevel);

			String strEnable = xml.GetAttrib( XML_LOG_ENABLE );
			bool bEnable = false;
			if (strEnable.empty() || strEnable == _T("1") || strEnable == _T("true") )
				bEnable = true;

			LOGITEM  logItem;
			memset(&logItem, 0, sizeof(LOGITEM));
			logItem.bEnable = bEnable;
			logItem.logName = SysAllocString(logName.c_str());
			logItem.logFile = SysAllocString(logFile.c_str());
			logItem.logLevel = (LOG_LEVEL)logLevel;
			logItem.uLogFlag = uLogFlag;
			logItem.uLogFileFlag = uLogFileFlag;
			logItem.uLogFileLoopCount = uLogFileLoopCount;
			if (false == pLog->AddLogItem(&logItem))
			{
				TCHAR szInfo[256] = _T("");
				_stprintf(szInfo, _T("CLogXmlParse::Load AddLogItem failed. name=%s, path=%s\n"), logName.c_str(), logFile.c_str() );
				::OutputDebugString(szInfo);
			}
		}
		bRet = true;
	}
	while(0);
	if (false == bRet)
	{
		::OutputDebugString(_T("CLogXmlParse::Load failed!"));
	}

	return bRet;
}


#if  0
// 
//	��XML�м���<log><item>��
//
//	[Parameter]
//		hModule
//			[in]	Ӧ�ó������������ڻ�ȡӦ�ó�������·����������չ��־����ļ������·��
//		strXmlPath
//			[in]	log xml �����ļ�·��(ȫ·��)
//		vLogItem
//			[out]	���ؽ������
//
//	Return
//		�ɹ���true��ʧ�ܣ�false
//
bool CLogXmlParse::ParseLogItems( HMODULE hModule, const String&  strXmlPath, vector<LogItem*>& vLogItems )
{
	bool bRet = false;
#if 0
	bRet = this->m_xml.Load(strXmlPath);

	if( false == bRet )
		return false;

	//////////////////////////////////////////////////////////////////////////
	// ͳһ�л�ΪСд��ĸ����ֹ���ô�Сд����

	String strXmlDoc = m_xml.GetDoc();
	transform(strXmlDoc.begin(),strXmlDoc.end(),strXmlDoc.begin(),tolower);
	this->m_xml.SetDoc(strXmlDoc);

	do
	{
		if( false == m_xml.FindElem() )                       break;    // ����root��������
		if( false == m_xml.IntoElem() )                       break;
		if( false == m_xml.FindElem( XML_LOG ) )              break;
		if( false == m_xml.IntoElem() )                       break;
		
		do 
		{
			if ( false == m_xml.FindElem( XML_LOG_ITEM ) )    break;

			String strName   = m_xml.GetAttrib( XML_LOG_NAME );
			String strSwitch = m_xml.GetAttrib( XML_LOG_SWITCH );
			String strPath   = m_xml.GetAttrib( XML_LOG_PATH );
			String strLevel  = m_xml.GetAttrib( XML_LOG_LEVEL );
			String strFlag   = m_xml.GetAttrib( XML_LOG_FLAG );
			String strFileFlag = m_xml.GetAttrib( XML_LOG_FILEFLAG );


			LogItem*  pItem = new LogItem;
			this->_makeLogItem(hModule, strName, strSwitch, strPath, strLevel, strFlag, strFileFlag, pItem );

			vLogItems.push_back( pItem );
		} 
		while(true);

		bRet = true;
	}
	while(0);

	if( false == bRet )
	{
		::OutputDebugString( _T("CLogXmlParse::GetLogItems failed") );
	}
#endif
	return bRet;
}

/*
**  ����xml��ȡ�����ַ�������Ϊ��Ӧ���ֶ�
**
**	Parameters
**		hModule
			[in]	Ӧ�ó������������ڻ�ȡӦ�ó�������·����������չ���·��

**		strName
**			[in]	��־����
**
**		strSwitch
**			[in]	����־�����Ƿ��
**
**		strPath
**			[in]	��־���λ��
**
**		strLevel
**			[in]	��־�������
**
**		strFlag
**			[in]	��־flag
**	
**		strFileFlag
**			[in]	����ļ�flag
**
**		pItem
			[out]	����LogItem
*/
bool CLogXmlParse::_makeLogItem( HMODULE hModule, const String& strName, const String& strSwitch, const String& strPath, const String& strLevel, const String& strFlag, const String& strFileFlag, LogItem* pItem )
{
	//////////////////////////////////////////////////////////////////////////
	// ģ������

	pItem->logName = strName;

	//////////////////////////////////////////////////////////////////////////
	// ����

	if ( _T("on") == strSwitch) 
		pItem->bSwitch = true;
	else if( _T("off") == strSwitch )
		pItem->bSwitch = false;
	else
		pItem->bSwitch = true;

	//////////////////////////////////////////////////////////////////////////
	// ���λ�� ����·��/���·��

	if( Util::IsFullPath( strPath.c_str()) )
	{
		pItem->logFile = strPath;
	}
	else if (strPath.length() > 0)
	{
		TCHAR szModulePath[MAX_PATH] = _T("");

		GetModuleFileName( hModule, szModulePath, MAX_PATH );
		TCHAR* szTemp = _tcsrchr(szModulePath, _T('\\'));
		if( NULL != szTemp )
			*(szTemp+1) = 0;
		
		TCHAR szFullPath[MAX_PATH] = _T("");
		Util::CalcFullPathByRelative(szModulePath, strPath.c_str(), szFullPath );
		pItem->logFile = szFullPath;
	}

	//////////////////////////////////////////////////////////////////////////
	// �������

	if ( _T("debug") == strLevel )
		pItem->logLevel = LOG_LEVEL_DEBUG;
	else if( _T("info") == strLevel )
		pItem->logLevel = LOG_LEVEL_INFO;
	else if( _T("warn") == strLevel )
		pItem->logLevel = LOG_LEVEL_WARN;
	else if( _T("error") == strLevel )
		pItem->logLevel = LOG_LEVEL_ERROR;
	else if( _T("fatal") == strLevel )
		pItem->logLevel = LOG_LEVEL_FATAL;
	else
		pItem->logLevel = LOG_LEVEL_WARN;


	//////////////////////////////////////////////////////////////////////////
	//  flag

	_stscanf( strFlag.c_str(),      L"0x%x", &pItem->uLogFlag );
	_stscanf( strFileFlag.c_str(),  L"0x%x", &pItem->uLogFileFlag );


	//////////////////////////////////////////////////////////////////////////
	// ��ʼ��

	if( false == pItem->Init() )
		return false;

	return true;
}

#endif

void Split(const String& str, TCHAR szSep, vector<String>& vRet)
{
	int nIndex = 0;

	while( true )
	{
		int nResult = (int)str.find( szSep, nIndex );
		if( -1 == nResult )
		{
			vRet.push_back( str.substr( nIndex, str.length()-nIndex ) );
			break;
		}
		else
		{
			vRet.push_back( str.substr( nIndex, nResult-nIndex ) );
			nIndex = ++nResult;
		}
	}
}