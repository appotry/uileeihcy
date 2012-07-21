#include "StdAfx.h"
#include <time.h>
#include <wincon.h>
#include "LogItem.h"

bool LogItem::bConsoleAlloc = false;

HRESULT LogItem::GetSwitch( LONG* pRetVal)
{
	if( NULL == pRetVal )
		return E_POINTER;

	if(bSwitch)
		*pRetVal = 1;
	else
		*pRetVal = 0;
	return S_OK;
}
HRESULT LogItem::GetName ( BSTR* pRetVal)
{
	if( NULL == pRetVal )
		return E_POINTER;

	*pRetVal = SysAllocString(logName.c_str());
	return S_OK;
}
HRESULT LogItem::GetPath  ( BSTR* pRetVal)
{
	if( NULL == pRetVal )
		return E_POINTER;

	*pRetVal = SysAllocString(logFile.c_str());
	return S_OK;
}
HRESULT LogItem::GetLevel ( LONG* pRetVal)
{
	if( NULL == pRetVal )
		return E_POINTER;

	*pRetVal = logLevel;
	return S_OK;
}
HRESULT LogItem::GetLogFlag ( LONG* pRetVal)
{
	if( NULL == pRetVal )
		return E_POINTER;

	*pRetVal = uLogFlag;
	return S_OK;
}
HRESULT LogItem::GetFileFlag( LONG* pRetVal)
{
	if( NULL == pRetVal )
		return E_POINTER;

	*pRetVal = uLogFileFlag;
	return S_OK;
}

//
//	初始化日志模块，成功返回true，失败返回false
//
bool  LogItem::Init( )
{
	if (logFile.empty())
		return false;
	
	// 既不输出到文件，也不输出到控制台，直接退出
	if ( (uLogFileFlag & LOG_FLAG_FILE_DISABLE) &&
		 !(uLogFlag & LOG_FLAG_CONSOLE) )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 创建输出文件

	if( PathIsDirectory(logFile.c_str()) ) // 传递进来的路径是一个文件夹，返回失败( 注意，这里不能用 TRUE == 来判断，因为返回的可能是16 )
	{
		return false;
	}


	if ( uLogFileFlag & LOG_FLAG_FILE_DISABLE )
	{
		// Do Nothing
	}
	else if( uLogFileFlag & LOG_FLAG_FILE_SIMPLE )   // 重复写
	{
		f.open( logFile.c_str(), ios_base::out );
	}
	else if( uLogFileFlag & LOG_FLAG_FILE_TIME )     // 新建一个以当前时间为后缀名的文件
	{
		int nPos = logFile.rfind( _T('\\') );
		if( -1 == nPos )
			return false;

		String strInsert = _T("_") + this->_curTime2();
		logFile.insert( nPos,  strInsert );

		f.open( logFile.c_str(), ios_base::out );
	}
	else if( uLogFileFlag & LOG_FLAG_FILE_LOOP )     // 循环使用文件
	{
		// (取出修改时间最早的那个文件，或者还不存在的那个)
		int       nMax = GET_FILE_LOOP_COUNT(uLogFileFlag);
		String    strLastFilePath = logFile;
		FILETIME  lastFT = {0,0};   // 记录/比较 各个文件的修改时间

		for ( byte i = 0; i < nMax; i++ )
		{
			String  strFilePath = logFile;
			this->_appendIndexToPath( strFilePath, i );
			if ( !PathFileExists(strFilePath.c_str()) )       // 这个序号的文件还没被创建，那么将日志写到该文件中
			{
				strLastFilePath = strFilePath;
				break;
			}

			WIN32_FIND_DATA  wfd;
			HANDLE hHandle = FindFirstFile( strFilePath.c_str(), &wfd );

			if( lastFT.dwHighDateTime == 0 && lastFT.dwLowDateTime == 0 )
			{
				// 初始化
				lastFT.dwHighDateTime = wfd.ftLastWriteTime.dwHighDateTime;
				lastFT.dwLowDateTime  = wfd.ftLastWriteTime.dwLowDateTime;

				strLastFilePath = strFilePath;
			}
			else if( -1 == ::CompareFileTime( &wfd.ftLastWriteTime, &lastFT ) )     // 该文件的修改修改更早
			{
				lastFT.dwHighDateTime = wfd.ftLastWriteTime.dwHighDateTime;
				lastFT.dwLowDateTime  =	wfd.ftLastWriteTime.dwLowDateTime;

				strLastFilePath = strFilePath;
			}

			FindClose(hHandle);
		}

		f.open( strLastFilePath.c_str(), ios_base::out );
	}

	if (f.is_open())
	{
		locale loc("chs");
		f.imbue(loc);
	}


	//////////////////////////////////////////////////////////////////////////
	// 初始化控制台

	if ( uLogFlag & LOG_FLAG_CONSOLE && bSwitch)
	{
		this->_initConsole();
	}

	return true;
}


/*
**	初始化控制台输出，显示控制台
*/
void LogItem::_initConsole()
{
	if ( true == bConsoleAlloc )
	{
		return;
	}

	bConsoleAlloc = true;

	AllocConsole();
	freopen("CONIN$",  "r+t",  stdin );
	freopen("CONOUT$", "w+t",  stdout); 
	std::ios_base::sync_with_stdio ( );

	// 输出unicode中文
	locale loc("chs");
	wcout.imbue(loc);
}

/*
**	释放控制台
*/
void LogItem::_releaseConsole()
{
	if( bConsoleAlloc )
	{
		::FreeConsole();
		bConsoleAlloc = false;
	}
}

void LogItem::Log( LOG_LEVEL level, const String& content )
{
	if( false == bSwitch )
		return;

	if ( level < this->logLevel)
		return;

	if ( uLogFlag & LOG_FLAG_CONSOLE)
	{
		static HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE); 
		switch(level)
		{
		case LOG_LEVEL_DEBUG:
			SetConsoleTextAttribute(hCon,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;

		case LOG_LEVEL_INFO:
			SetConsoleTextAttribute(hCon,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;

		case LOG_LEVEL_WARN:
			SetConsoleTextAttribute(hCon,FOREGROUND_GREEN);
			break;

		case LOG_LEVEL_ERROR:
			SetConsoleTextAttribute(hCon,FOREGROUND_RED);
			break;

		case LOG_LEVEL_FATAL:
			SetConsoleTextAttribute(hCon,FOREGROUND_GREEN|FOREGROUND_RED);
			break;
		}

		
		

		_logToStream( wcout, level, content);
	}
	
	if( f.is_open() )
		_logToStream( f, level, content);
}


template<class T>
void   LogItem::_logToStream( T& o, LOG_LEVEL level, const String& content )
{
	o << logName << _T(" ");
	o << LogItem::LevelToString(level)<< _T(" ");

	if( uLogFlag & LOG_FLAG_THREADID )
	{
		TCHAR szTheadID[16];
		_stprintf(szTheadID, _T("%d"), GetCurrentThreadId() );   // 防止被 1234 被写成了 1,234
		o <<  szTheadID << _T(" ");
	}

	o << _curTime()<< _T(" ");
	o << content;
	o << endl;

	o.flush();
}

// 输出到dbgview中查看
void  LogItem::_logToDbgview( LOG_LEVEL level, const String& content )
{
	String strinfo;

	strinfo += logName + _T(" ");
	strinfo += LogItem::LevelToString(level) + _T(" ");

	if( uLogFlag & LOG_FLAG_THREADID )
	{
		TCHAR szTheadID[16];
		_stprintf(szTheadID, _T("%d"), GetCurrentThreadId() );   // 防止被 1234 被写成了 1,234
		strinfo += szTheadID;
		strinfo += _T(" ");
	}

	strinfo +=  content;
	strinfo +=  _T("\r\n");

	::OutputDebugString(strinfo.c_str());
}

// 获取当前时间，时间格式：%d%02d%02d %02d%02d%02d，用于文件名后缀
String&  LogItem::_curTime2( )
{
	time_t	t;
	tm      s_tm;
	TCHAR   szTime[128] = _T("");
	static  String  strRet;

	time( &t );
	localtime_s( &s_tm, &t );

	_stprintf( szTime, _T("%d%02d%02d %02d%02d%02d"),(s_tm.tm_year + 1900), s_tm.tm_mon+1, s_tm.tm_mday, 
		s_tm.tm_hour, s_tm.tm_min,  s_tm.tm_sec );

	strRet = szTime;
	return strRet;
}

// 获取当前时间，时间格式：%d-%02d-%02d %02d:%02d:%02d 星期x
String&  LogItem::_curTime( )
{
	time_t	t;
	tm      s_tm;
	TCHAR   szTime[128] = _T("");
	static  String  strRet;

	time( &t );
	localtime_s( &s_tm, &t );

	_stprintf( szTime, _T("%d-%02d-%02d %02d:%02d:%02d "),(s_tm.tm_year + 1900), s_tm.tm_mon+1, s_tm.tm_mday, 
		s_tm.tm_hour, s_tm.tm_min,  s_tm.tm_sec );

// 
// 	switch( s_tm.tm_wday )
// 	{
// 	case 0:
// 		_tcscat( szTime, _T("Sunday") );
// 		break;
// 	case 1:
// 		_tcscat( szTime, _T("Monday") );
// 		break;
// 	case 2:
// 		_tcscat( szTime, _T("Tuesday") );
// 		break;
// 	case 3:
// 		_tcscat( szTime, _T("Wendesday") );
// 		break;
// 	case 4:
// 		_tcscat( szTime, _T("Thusday") );
// 		break;
// 	case 5:
// 		_tcscat( szTime, _T("Friday") );
// 		break;
// 	case 6:
// 		_tcscat( szTime, _T("Satursday") );
// 		break;
// 	}

	strRet = szTime;
	return strRet;
}

//
//	Parameter
//		l
//			[in]	要进行转换的级别
//
//		bAlign
//			[in]	是否要进行对齐处理
//
/*static*/
String& LogItem::LevelToString( const LOG_LEVEL& l, bool bAlign )
{
	static String strRet;

	switch(l)
	{
	case LOG_LEVEL_DEBUG:
		if( bAlign )
			strRet =  _T(" [DEBUG]   ");
		else
			strRet = _T("DEBUG");
		break;
	case LOG_LEVEL_INFO:
		if( bAlign )
			strRet =  _T(" [INFO]    ");
		else
			strRet = _T("INFO");
		break;
	case LOG_LEVEL_WARN:
		if( bAlign )
			strRet =  _T(" [WARN]    ");
		else
			strRet = _T("WARN");
		break;
	case LOG_LEVEL_ERROR:
		if( bAlign )
			strRet =  _T(" [ERROR]   ");
		else
			strRet = _T("ERROR");
		break;
	case LOG_LEVEL_FATAL:
		if( bAlign )
			strRet =  _T(" [FATAL]   ");
		else
			strRet = _T("FATAL");
		break;
	default:
		if( bAlign )
			strRet =  _T(" [UNKNOWN] ");
		else
			strRet = _T("UNKNOWN");
		break;
	}

	return strRet;
}
long LogItem::StringToLevel( const String& level )
{
	if( level == _T("DEBUG") )
		return LOG_LEVEL_DEBUG;
	if( level == _T("INFO") )
		return LOG_LEVEL_INFO;
	if( level == _T("WARN") )
		return LOG_LEVEL_WARN;
	if( level == _T("ERROR") )
		return LOG_LEVEL_ERROR;
	if( level == _T("FATAL") )
		return LOG_LEVEL_FATAL;

	return LOG_LEVEL_DEBUG;
}

//
//	在给定的路径后面加上序号，如  ( abc.log, 1 ) ==> abc_1.log
//
void LogItem::_appendIndexToPath(String& strPath, byte index)
{
	String::size_type nPos = strPath.rfind( _T('.') );

	if( String::npos == nPos )
		return;

	TCHAR szTemp[16] = _T("");
	_stprintf( szTemp, _T("_%d"), int(index) );

	strPath.insert(nPos,szTemp);
}