#include "stdafx.h"
#include "h\util.h"
namespace UI { namespace Util
{
//////////////////////////////////////////////////////////////////////////
//	                       �͵�����صĲ���                             //
//////////////////////////////////////////////////////////////////////////



#ifdef UTIL_DEBUG
#include <time.h>
#include <DbgHelp.h>

LPTOP_LEVEL_EXCEPTION_FILTER   g_prevExceptionFilter = NULL;    // �����Լ��Ļص������ǣ�����֮ǰ�Ļص�����
String                         g_strDumpPath;
bool                           g_bAddTime;

LONG WINAPI MyUnhandledExceptionFilter(	__in struct _EXCEPTION_POINTERS* ExceptionInfo	);

// Ϊ�˲��þ�̬���ӵ� dbghelp.lib�����ö�̬���ӵķ���
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
//	�����������ʱ������һ��dump�ļ�����¼����ʱ����Ϣ
//
//	Parameter
//		strFilePath
//			[in]	���ɵ�dump�ļ���·������bAddTimeΪtrueʱ�������ļ�·�����������
//					��ǰʱ����Ϊ����ÿ�α����ı�ʶ
//		bAddTime
//			[in]	�Ƿ�Ҫ�ڱ�����·������ӱ�����ʱ�򡣲�������ӣ������ɵ�dump�ļ�
//					�Ὣ��һ�ε��ļ����ǵ�
//
//	Remark
//		���������ʱ������ûص��������ڻص������У����ǾͿ������κβ�����
//
void  /*UIUTILAPI*/ CreateDumpFileWhenCrash( String strFilePath, bool bAddTime )
{
	g_prevExceptionFilter = SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	g_strDumpPath         = strFilePath;
	g_bAddTime            = bAddTime;
}


//
//	����ʱ�Ļص�����
//
//	Return
//		EXCEPTION_EXECUTE_HANDLER = 1     ��ʾ���̽���
//		EXCEPTION_CONTINUE_EXECUTION = -1 ��ʾ�����쳣֮�����ִ��
//		EXCEPTION_CONTINUE_SEARCH = 0    ����ϵͳͨ�����쳣����������Ϣ�Ի��� 
//
LONG WINAPI MyUnhandledExceptionFilter(
									 __in          struct _EXCEPTION_POINTERS* pExceptionInfo
									 )
{
	do 
	{
		// 1. ��dbghelp.dll�ж�̬���س�dump��������

		HMODULE hModule = ::LoadLibrary( _T("dbghelp.dll") );
		if( NULL == hModule ) break;

		funcMiniDumpWriteDump  pMiniDumpWriteDump;
		pMiniDumpWriteDump = (funcMiniDumpWriteDump)::GetProcAddress( hModule, "MiniDumpWriteDump" );
		if( NULL == pMiniDumpWriteDump )	break;



		// 2. ����dump�ļ�

		if ( g_bAddTime )
		{
			String::size_type nPos = g_strDumpPath.rfind( _T('.') );
			if ( String::npos == nPos )	break;

			// ��ȡ��ǰʱ�䣬���뵽Ҫ���ɵ�dump�ļ�������
			time_t	 t; tm s_tm;
			TCHAR    szTime[128] = _T("");
			time(&t);localtime_s( &s_tm, &t );
			_stprintf( szTime, _T(" %d-%02d-%02d_%02d%02d%02d"),(s_tm.tm_year + 1900), s_tm.tm_mon+1, s_tm.tm_mday, s_tm.tm_hour, s_tm.tm_min,  s_tm.tm_sec );

			g_strDumpPath.insert(nPos,szTime);
		}

		HANDLE  hFile = ::CreateFile( g_strDumpPath.c_str(), FILE_WRITE_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if( NULL == hFile )	break;

		// 3. д��dump��Ϣ

		MINIDUMP_EXCEPTION_INFORMATION  mei;
		mei.ThreadId = GetCurrentThreadId();
		mei.ExceptionPointers = pExceptionInfo;
		mei.ClientPointers = FALSE;

		// TODO: ���Կ�����dump��Ϣ�м���汾��Ϣ�Ĳ���(PMINIDUMP_USER_STREAM_INFORMATION)
		BOOL bRet = pMiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mei, NULL, NULL );
		if ( FALSE == bRet )
		{
			DWORD dwErr = GetLastError();
		}
		
		// 4. ����

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