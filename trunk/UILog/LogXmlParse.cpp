#include "StdAfx.h"
#include "LogXmlParse.h"

//
// log.xml
//
#define XML_LOG                   _T("log")
#define XML_LOG_ITEM              _T("item")
#define XML_LOG_SWITCH            _T("switch")
#define XML_LOG_SWITCH_ON         _T("on")
#define XML_LOG_SWITCH_OFF        _T("off")
#define XML_LOG_NAME              _T("name")
#define XML_LOG_PATH              _T("path")
#define XML_LOG_LEVEL             _T("level")
#define XML_LOG_FLAG              _T("flag")
#define XML_LOG_FILEFLAG          _T("fileflag")



/*
<root>                                                    // 根名称，可以任意名称。程序将去根结点下的<log>信息 
    <log  switch="on/off">                                // log，switch用来控制整个模块的开关状态
	        <item                                         //
			 name="moduleName"                            // 一个日志输出项，模块的名称
			 switch="on/off"                              // 该模块日志输出开关(默认on)
             level="debug/info/warn/error/fator"          // 该日志输出项的输出级别(默认warn)  
             path="log.log"                               // 日志文件路径，当第二个字符为:时，默认为绝对路径，否则为相对路径
             flag="0x0"                                   // 日志flag，必须以0x开头 (见logitem.h)
             fileflag="0x0"                               // 输出文件flag，必须以0x开头 (见logitem.h)
        />        
    </log>
</root>

*/



CLogXmlParse::CLogXmlParse(const String& strDataSource) : ILogParse(strDataSource)
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
bool CLogXmlParse::Save( CPojo_Log*  pLog )
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
		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_LOG) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_LOG ))             break;

		// 日志总开关
		if( false == xml.SetAttrib( XML_LOG_SWITCH, pLog->IsSwitchOn()?XML_LOG_SWITCH_ON:XML_LOG_SWITCH_OFF ) )  break;
		if( false == xml.IntoElem() )                       break;

		bool bLoopRet = true;

		map<String, LogItem*>::iterator  iter;
		map<String, LogItem*>::iterator  iterEnd;
		pLog->GetEnum( &iter, &iterEnd );
		for ( ; iter != iterEnd; iter++ )
		{
			TCHAR szFlag[32] = _T("");

			String strModuleName = iter->first;
			String strLogLevel   = LogItem::LevelToString(iter->second->logLevel, false);
			String strSwitch     = iter->second->bSwitch? XML_LOG_SWITCH_ON : XML_LOG_SWITCH_OFF;
			String strPath       = iter->second->logFile;
			
			_stprintf( szFlag, _T("0x%x"), iter->second->uLogFlag );
			String strLogFlag = szFlag;

			_stprintf( szFlag, _T("0x%x"), iter->second->uLogFileFlag );
			String strLogFileFlag = szFlag;

			if( false == xml.InsertElem( XML_LOG_ITEM ))       { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_NAME, strModuleName.c_str()) ) { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_SWITCH, strSwitch.c_str()) )  { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_LEVEL, strLogLevel.c_str()) )  { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_PATH, strPath.c_str()) )  { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_FLAG, strLogFlag.c_str()) ) { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_LOG_FILEFLAG, strLogFileFlag.c_str()) ) { bLoopRet = false;  break; }
		}

		if( !bLoopRet )
			break;

		// 保存文件
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

bool CLogXmlParse::Load( CPojo_Log*  pLog )
{
	if( NULL == pLog )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		::OutputDebugString( _T("CLogXmlParse::Save  load xml failed") );
		return false;
	}

	// 统一切换为小写字母，防止配置大小写错误
	String strXmlDoc = xml.GetDoc();
	transform(strXmlDoc.begin(),strXmlDoc.end(),strXmlDoc.begin(),tolower);
	xml.SetDoc(strXmlDoc);

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( false == xml.FindElem(XML_LOG) )                break;

		String strGlobalSwitch = xml.GetAttrib(XML_LOG_SWITCH);
		if( strGlobalSwitch == XML_LOG_SWITCH_ON )
			pLog->SetSwitch(true);
		else
			pLog->SetSwitch(false);

		if( false == xml.IntoElem() )                       break;
		while(1)
		{
			if (false == xml.FindElem(XML_LOG_ITEM))        break;
			
			String logName = xml.GetAttrib(XML_LOG_NAME);
			String logFile = xml.GetAttrib(XML_LOG_PATH);
			
			if( Util::IsFullPath( logFile.c_str()) )
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
			_stscanf( strLogFlag.c_str(), _T("0x%x"), &uLogFlag );

			String strLogFileFlag = xml.GetAttrib(XML_LOG_FILEFLAG);
			long uLogFileFlag = 0;
			_stscanf( strLogFileFlag.c_str(), _T("0x%x"), &uLogFileFlag );

			String strLevel = xml.GetAttrib(XML_LOG_LEVEL);
			long logLevel = LogItem::StringToLevel(strLevel);

			String strSwtich = xml.GetAttrib( XML_LOG_SWITCH );
			bool bSwitch = false;
			if( strSwtich == XML_LOG_SWITCH_ON )
				bSwitch = true;

			if( false == pLog->AddLogItem( logName, bSwitch, logLevel, logFile, uLogFlag, uLogFileFlag ) )
			{
				TCHAR szInfo[256] = _T("");
				_stprintf(szInfo, _T("CLogXmlParse::Load AddLogItem failed. name=%s, path=%s\n"), logName.c_str(), logFile.c_str() );
				::OutputDebugString(szInfo);
			}
		}
		bRet = true;
	}
	while(0);
	if( false == bRet )
	{
		::OutputDebugString(_T("CLogXmlParse::Save  failed!"));
	}

	return bRet;
}


#if  0
// 
//	从XML中加载<log><item>项
//
//	[Parameter]
//		hModule
//			[in]	应用程序句柄，可用于获取应用程序所在路径，进而扩展日志输出文件的相对路径
//		strXmlPath
//			[in]	log xml 描述文件路径(全路径)
//		vLogItem
//			[out]	返回解析结果
//
//	Return
//		成功：true，失败：false
//
bool CLogXmlParse::ParseLogItems( HMODULE hModule, const String&  strXmlPath, vector<LogItem*>& vLogItems )
{
	bool bRet = false;
#if 0
	bRet = this->m_xml.Load(strXmlPath);

	if( false == bRet )
		return false;

	//////////////////////////////////////////////////////////////////////////
	// 统一切换为小写字母，防止配置大小写错误

	String strXmlDoc = m_xml.GetDoc();
	transform(strXmlDoc.begin(),strXmlDoc.end(),strXmlDoc.begin(),tolower);
	this->m_xml.SetDoc(strXmlDoc);

	do
	{
		if( false == m_xml.FindElem() )                       break;    // 忽略root结点的名称
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
**  将从xml获取到的字符串解析为相应的字段
**
**	Parameters
**		hModule
			[in]	应用程序句柄，可用于获取应用程序所在路径，进而扩展相对路径

**		strName
**			[in]	日志名称
**
**		strSwitch
**			[in]	该日志功能是否打开
**
**		strPath
**			[in]	日志输出位置
**
**		strLevel
**			[in]	日志输出级别
**
**		strFlag
**			[in]	日志flag
**	
**		strFileFlag
**			[in]	输出文件flag
**
**		pItem
			[out]	返回LogItem
*/
bool CLogXmlParse::_makeLogItem( HMODULE hModule, const String& strName, const String& strSwitch, const String& strPath, const String& strLevel, const String& strFlag, const String& strFileFlag, LogItem* pItem )
{
	//////////////////////////////////////////////////////////////////////////
	// 模块名称

	pItem->logName = strName;

	//////////////////////////////////////////////////////////////////////////
	// 开关

	if ( _T("on") == strSwitch) 
		pItem->bSwitch = true;
	else if( _T("off") == strSwitch )
		pItem->bSwitch = false;
	else
		pItem->bSwitch = true;

	//////////////////////////////////////////////////////////////////////////
	// 输出位置 绝对路径/相对路径

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
	// 输出级别

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
	// 初始化

	if( false == pItem->Init() )
		return false;

	return true;
}

#endif