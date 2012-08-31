#include "stdafx.h"
#include "h\util.h"
#include "shobjidl.h"
#include "shlobj.h"
#include "shellapi.h"

namespace UI { namespace Util
{
//	和文件相关的操作

#ifdef UTIL_FILE


//
//	函数:	BOOL GetFileBuffer( TCHAR* szPath, CBufferT<char>&	fileBuf )
//
//	目的:	获取一个文件的字节内容
//
//	参数:	
//		szPath
//			[in]:	要获取的文件路径
//		fileBuf
//			[out]:	返回文件字节内容
//
//	备注：
//		fileBuf.GetBufSize()可获取其大小
//
BOOL GetFileBuffer( TCHAR* szPath, CBufferT<char>&	fileBuf )
{
	fstream		file;
	int			nFileLen = 0;

#ifdef _UNICODE
	CBufferT<char> strPath;
	U2A( szPath, strPath );
	file.open( (char*)strPath, ios_base::in|ios_base::binary );
#else
	file.open( szPath, ios_base::in|ios_base::binary );
#endif

	if( file.fail() )
		return FALSE;

	file.seekg( 0, ios_base::end );
	nFileLen = file.tellg();
	file.seekg( 0, ios_base::beg );

	fileBuf.malloc( nFileLen );
	file.read( (char*)fileBuf, nFileLen );
	file.close();
	
	return TRUE;
}


//
//	函数:	BOOL OpenFolderAndSelectFile( TCHAR* szFullPath )
//	 
//	目的：	打开我的电脑，并定位到指定的文件或文件夹
//
//	参数:	
//		szFullPath
//			[in]:	要定位到得文件或文件夹
//
BOOL OpenFolderAndSelectFile( TCHAR* szFullPath )
{
	LPSHELLFOLDER		pDesktopFolder	= NULL;
	LPITEMIDLIST		pidl			= NULL;
	ULONG				chEaten			= 0 ;
	ULONG				dwAttributes	= 0;
	HRESULT				hr				= E_FAIL;

	if( FAILED(SHGetDesktopFolder( &pDesktopFolder)))
		return FALSE;

#ifdef _UNICODE
	hr = pDesktopFolder->ParseDisplayName( NULL, NULL, szFullPath, &chEaten, &pidl, &dwAttributes);
#else
	CBufferT<wchar_t>  wcsFullPath;
	A2U( szFullPath , wcsFullPath );
	hr = pDesktopFolder->ParseDisplayName( NULL, NULL, (wchar_t*)wcsFullPath, &chEaten, &pidl, &dwAttributes);
#endif
	if (FAILED(hr))
	{
		pDesktopFolder->Release();
		return FALSE;
	}
	LPCITEMIDLIST pidlFolder = pidl;
	
	hr = CoInitialize( NULL);
	hr = SHOpenFolderAndSelectItems( pidl, 0, NULL, 0);
	if( SUCCEEDED(hr) )
		::CoUninitialize();

	pDesktopFolder->Release();
	
	if ( hr == S_OK)
	{
		return TRUE;
	}
	return TRUE;
}



//
//	函数:	void ShowFileProp( TCHAR* szFile )
//	 
//	目的：	打开一个文件的属性对话框
//
//	参数:	
//		szFile
//			[in]:	要显示哪个文件的属性
//
void ShowFileProp( TCHAR* szFile )
{
	SHELLEXECUTEINFO ExecInfo;
	::ZeroMemory( &ExecInfo, sizeof( SHELLEXECUTEINFO ) );
	ExecInfo.fMask = SEE_MASK_INVOKEIDLIST ;
	ExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ExecInfo.hwnd = NULL;
	ExecInfo.lpVerb = _T("properties");
	ExecInfo.lpFile = szFile;
	ExecInfo.nShow = SW_SHOW;
	::ShellExecuteEx( &ExecInfo );	
}

//
//	函数:	void ExecFile( TCHAR* szFile, BOOL bWait = FALSE ) 
//	 
//	目的：	运行一个程序
//
//	参数:	
//		szFile
//			[in]:	要执行的文件的路径
//		bWait:
//			[in]:	是否要等待该程序运行结束
//
void ExecFile( TCHAR* szFile, BOOL bWait ) 
{
	SHELLEXECUTEINFO ExecInfo;
	::ZeroMemory( &ExecInfo, sizeof( SHELLEXECUTEINFO ) );
	ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ExecInfo.hwnd = NULL;
	ExecInfo.lpVerb = _T("open");
	ExecInfo.lpFile = szFile;
	ExecInfo.nShow = SW_SHOW;
	::ShellExecuteEx( &ExecInfo );	

	if(bWait)
		::WaitForSingleObject( ExecInfo.hProcess, INFINITE );

	::CloseHandle( ExecInfo.hProcess );
}

//
//	判断一个路径是否是全路径
//
BOOL IsFullPath( const TCHAR* szPath)
{
	
	UIASSERT( szPath != NULL );
	return !::PathIsRelative( szPath );
#if 0
	//Full path len >= 2
	if (szPath[0] == '\0' || szPath[1] == '\0')
	{
		return FALSE;
	}
	if (*szPath == '"') //"<path>" - skip the quote.
	{
		szPath++;
	}
	if (szPath[1]==':')		// drive: case
	{
		return TRUE;
	}
	if (szPath[0]=='\\' && szPath[1]=='\\' && 
		szPath[2] != '\0' ) // unc path name
	{		
		return TRUE;
	}
	return FALSE;
#endif
}

//
//	计算szPath相对于文件夹szDir的相对路径szOutRelative
//
BOOL CalcRelativePathToDir(const TCHAR* szDir, const TCHAR* szPath, TCHAR* szOutRelativePath )
{
	if ( NULL == szDir || NULL == szPath || NULL == szOutRelativePath )
	{
		return FALSE;
	}

	BOOL bRet = ::PathRelativePathTo( szOutRelativePath,
		 szDir, FILE_ATTRIBUTE_DIRECTORY, 
		 szPath, FILE_ATTRIBUTE_NORMAL );

	if( FALSE == bRet )
	{
		_tcscpy( szOutRelativePath, szPath );
	}
	return bRet;
}

//
//	计算szPath相对于文件szFile的相对路径szOutRelative
//
BOOL CalcRelativePathToFile(const TCHAR* szFile, const TCHAR* szPath, TCHAR* szOutRelativePath )
{
	if ( NULL == szFile || NULL == szPath || NULL == szOutRelativePath )
	{
		return FALSE;
	}

	BOOL bRet = ::PathRelativePathTo( szOutRelativePath,
		szFile, FILE_ATTRIBUTE_NORMAL, 
		szPath, FILE_ATTRIBUTE_NORMAL );

	if( FALSE == bRet )
	{
		_tcscpy( szOutRelativePath, szPath );
	}

	return bRet;
}
//
//	将相对于szDir的路径szRelative扩展为全路径szOut
//
BOOL CalcFullPathByRelative(const TCHAR* szDir, const TCHAR* szRelative, TCHAR* szOut )
{
	if( NULL == szDir || NULL == szRelative || NULL == szOut )
		return FALSE;

	TCHAR szPath [2*MAX_PATH] = _T("");

	int nLenDir = _tcslen(szDir);
	if( szRelative[0] != _T('\\') ||
		szDir[nLenDir-1] != _T('\\') )
	{
		_stprintf( szPath, _T("%s\\%s"), szDir,szRelative );
	}
	else
	{
		_stprintf( szPath, _T("%s%s"), szDir,szRelative );
	}

	PathCanonicalize( szOut, szPath );

	return TRUE;
}

//
//	获取一个路径szPath所在的目录szOutDir
//
BOOL GetPathDir( const TCHAR* szPath, TCHAR* szOutDir )
{
	if( NULL == szPath || NULL == szOutDir )
		return FALSE;

	TCHAR szCopy[MAX_PATH] = _T("");
	_tcsncpy( szCopy, szPath, MAX_PATH-1);

	TCHAR* p = _tcsrchr( szCopy, _T('\\') );
	if( NULL == p )
		return FALSE;

#ifdef _UNICODE
	*p = _T('');
#else
	*p = 0;
#endif
	_tcscpy( szOutDir, szCopy );
	return TRUE;
}


//
//	获取一个路径szPath中的文件名
//
BOOL GetPathFileName( const TCHAR* szPath, TCHAR* szOutDir )
{
	if( NULL == szPath || NULL == szOutDir )
		return FALSE;

	const TCHAR* p = _tcsrchr( szPath, _T('\\') );
	if( NULL == p )
		return FALSE;

	_tcscpy( szOutDir, p+1 );
	return TRUE;
}

//
//	创建一个空的xml文件，文件内容包含 <? ?> 加上root元素<ui>,再添加一个结点<node>
//
bool CreateEmptyXmlFile( const TCHAR* szPath, const TCHAR* szRoot, const TCHAR* szNode )
{
#ifdef _UNICODE
	wfstream f;
#else
	fstream  f;
#endif
	f.open( szPath, ios_base::out );
	if( f.fail() )
	{
		return false;
	}

	// 必须填充一个root元素，否则markup会返回失败
	f << _T("<?xml version=\"1.0\" encoding=\"utf-8\" ?>") << endl;
	f << _T("<") << szRoot << _T(">") << endl;
	if( NULL != szNode )
	{
		f << _T("<") << szNode << _T("/>") << endl;   
	}
	f << _T("</") << szRoot << _T(">");   

	f.close();

	return true;
}


// 遍历文件夹
BOOL EnumFileInDirectory(const TCHAR* szDir, EnumFileInDirProc proc, WPARAM wParam)
{
	WIN32_FIND_DATA  finddata;

	String strBaseDir = szDir;
	if (strBaseDir[strBaseDir.length()-1] != _T('\\') &&
		strBaseDir[strBaseDir.length()-1] != _T('/'))
	{
		strBaseDir += _T("\\");
	}
	String strFind = strBaseDir;
	strFind += _T("*.*");

	HANDLE hFind=::FindFirstFile(strFind.c_str(),&finddata);
	if(INVALID_HANDLE_VALUE == hFind)
		return FALSE;

	while(1)
	{
		if(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (finddata.cFileName[0] != '.')
			{
				String str = strBaseDir;
				str += finddata.cFileName;
				str += _T("\\");
				EnumFileInDirectory(str.c_str(), proc, wParam);
			}
		}
		else
		{
			proc(strBaseDir.c_str(), finddata.cFileName, wParam);
		}
		if(!FindNextFile(hFind,&finddata))
			break;
	}
	FindClose(hFind);

	return TRUE;
}

#endif
}}

