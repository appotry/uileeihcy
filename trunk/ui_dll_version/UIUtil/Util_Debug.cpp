#include "stdafx.h"
#include "h\util.h"
namespace UI { namespace Util
{
//////////////////////////////////////////////////////////////////////////
//	                       和调试相关的操作                             //
//////////////////////////////////////////////////////////////////////////



#ifdef UTIL_DEBUG
#include <time.h>
#include <DbgHelp.h>

LPTOP_LEVEL_EXCEPTION_FILTER   g_prevExceptionFilter = NULL;    // 设置自己的回调函数是，保存之前的回调函数
String                         g_strDumpPath;
bool                           g_bAddTime;

LONG WINAPI MyUnhandledExceptionFilter(	__in struct _EXCEPTION_POINTERS* ExceptionInfo	);

// 为了不用静态链接到 dbghelp.lib，采用动态链接的方法
typedef BOOL (WINAPI *funcMiniDumpWriteDump)(\
									  __in          HANDLE hProcess,\
									  __in          DWORD ProcessId,\
									  __in          HANDLE hFile,\
									  __in          MINIDUMP_TYPE DumpType,\
									  __in          PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,\
									  __in          PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,\
									  __in          PMINIDUMP_CALLBACK_INFORMATION CallbackParam\
									  );


//
//	当程序崩溃的时候生成一个dump文件，记录崩溃时的信息
//
//	Parameter
//		strFilePath
//			[in]	生成的dump文件的路径，当bAddTime为true时，会在文件路径后面添加上
//					当前时间作为区分每次崩溃的标识
//		bAddTime
//			[in]	是否要在崩溃的路径中添加崩溃的时候。不过不添加，新生成的dump文件
//					会将上一次的文件覆盖掉
//
//	Remark
//		当程序崩溃时，会调用回调函数，在回调函数中，我们就可以作任何操作了
//
void  /*UIUTILAPI*/ CreateDumpFileWhenCrash( String strFilePath, bool bAddTime )
{
	g_prevExceptionFilter = SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	g_strDumpPath         = strFilePath;
	g_bAddTime            = bAddTime;
}


//
//	崩溃时的回调函数
//
//	Return
//		EXCEPTION_EXECUTE_HANDLER = 1     表示进程结束
//		EXCEPTION_CONTINUE_EXECUTION = -1 表示处理异常之后继续执行
//		EXCEPTION_CONTINUE_SEARCH = 0    进行系统通常的异常处理（错误消息对话框） 
//
LONG WINAPI MyUnhandledExceptionFilter(
									 __in          struct _EXCEPTION_POINTERS* pExceptionInfo
									 )
{
	do 
	{
		// 1. 从dbghelp.dll中动态加载出dump导出函数

		HMODULE hModule = ::LoadLibrary( _T("dbghelp.dll") );
		if( NULL == hModule ) break;

		funcMiniDumpWriteDump  pMiniDumpWriteDump;
		pMiniDumpWriteDump = (funcMiniDumpWriteDump)::GetProcAddress( hModule, "MiniDumpWriteDump" );
		if( NULL == pMiniDumpWriteDump )	break;



		// 2. 生成dump文件

		if ( g_bAddTime )
		{
			String::size_type nPos = g_strDumpPath.rfind( _T('.') );
			if ( String::npos == nPos )	break;

			// 获取当前时间，插入到要生成的dump文件名当中
			time_t	 t; tm s_tm;
			TCHAR    szTime[128] = _T("");
			time(&t);localtime_s( &s_tm, &t );
			_stprintf( szTime, _T(" %d-%02d-%02d_%02d%02d%02d"),(s_tm.tm_year + 1900), s_tm.tm_mon+1, s_tm.tm_mday, s_tm.tm_hour, s_tm.tm_min,  s_tm.tm_sec );

			g_strDumpPath.insert(nPos,szTime);
		}

		HANDLE  hFile = ::CreateFile( g_strDumpPath.c_str(), FILE_WRITE_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( NULL == hFile )	break;

		// 3. 写入dump信息

		MINIDUMP_EXCEPTION_INFORMATION  mei;
		mei.ThreadId = GetCurrentThreadId();
		mei.ExceptionPointers = pExceptionInfo;
		mei.ClientPointers = FALSE;

		// TODO: 可以考虑在dump信息中加入版本信息的参数(PMINIDUMP_USER_STREAM_INFORMATION)
		BOOL bRet = pMiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mei, NULL, NULL );
		if ( FALSE == bRet )
		{
			DWORD dwErr = GetLastError();
		}
		
		// 4. 结束

		CloseHandle( hFile );
		if ( hModule )
		{
			::FreeLibrary( hModule );
			hModule = NULL;
		}

	} 
	while (false);

	if( NULL != g_prevExceptionFilter )
		SetUnhandledExceptionFilter( g_prevExceptionFilter );
	
	return EXCEPTION_CONTINUE_SEARCH;
}



#endif
}}