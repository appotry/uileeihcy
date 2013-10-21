#include "stdafx.h"
#include "h\util.h"
#include "shobjidl.h"
#include "shlobj.h"
#include "shellapi.h"

/*
windows路径操作API函数
PathRemoveArgs 
去除路径的参数
PathRemoveBackslash 
去除路径最后的反斜杠"\"
PathAddBackslash 
在路径最后加上反斜杠"\"
PathRemoveBlanks 
去除路径前后的空格
PathAddExtension 
在文件路径后面加上扩展名
PathRemoveExtension 
去除文件路径扩展名
PathRenameExtension
更改文件路径扩展名
PathRemoveFileSpec 
去除文件名,得到目录
PathUnquoteSpaces
去除路径中的首尾空格
PathQuoteSpaces 
判断路径中是否有空格,有的话,就是用""引号把整个路径包含起来
PathAppend 
将一个路径追加到另一个路径后面
PathCombine 
合并两个路径
PathSkipRoot
去掉路径中的磁盘符或UNC部分
PathStripPath 
去掉路径中的目录部分,得到文件名
PathStripToRoot
去掉路径的文件部分,得到根目录
PathCompactPath
根据像素值生成符合长度的路径
如原始路径: C:\path1\path2\sample.txt
根据120像素截断后为: C:\pat...\sample.txt
根据25像素截断后为: ...\sample.txt
PathCompactPathEx 
根据字符个数来生成符合长度的路径
PathSetDlgItemPath 
将路径数据设置到对话框的子控件上
PathUndecorate 
去除路径中的修饰

PathUnExpandEnvStrings 
将路径中部分数据替换为系统环境变量格式 

PathFindOnPath 
从路径中查找路径
PathFindExtension 
查找路径的扩展名
PathFindFileName 
获取路径的文件名
PathFindNextComponent 
查找匹配路径
PathFindSuffixArray 
查找给定的文件名是否有给定的后缀

PathGetArgs 
获取路径参数
PathGetCharType
获取路径字符类型
PathGetDriveNumber
根据逻辑盘符返回驱动器序号 

PathRelativePathTo
创建一个路径到另一个路径的相对路径。

PathResolve 
将一个相对路径或绝对路径转换为一个合格的路径
PathCanonicalize 
规范化路径。将格式比较乱的路径整理成规范的路径格式
PathBuildRoot 
根据给定的磁盘序号创建根目录路径
CreateDirectory
创建目录
GetShortPathName 
将长路径转为8.3格式的短路径格式
GetLongPathName 
将短路径格式转为长路径。
PathGetShortPath 
将长路径转为短路径格式（8.3格式）
PathCreateFromUrl 
将URL路径转为MS-DOS格式
PathMakePretty 
把路径全部转为小写,增加可读性

PathMakeSystemFolder 
给路径增加系统属性
PathUnmakeSystemFolder 
去除路径中的系统属性
PathMakeUniqueName 
从模板创建统一的路径格式
PathProcessCommand 
生成一个可执行的路径,比如有参数的,会自动将路径用""包含
PathCleanupSpec 
去除路径中不合法的字符
PathCommonPrefix 
比较并提取两个路径相同的前缀
PathFileExists 
验证路径是否存在
PathMatchSpec 
判断路径是否匹配制定的扩展名
PathIsDirectory 
判断路径是否是一个有效的目录
PathIsFileSpec
验证路径是否一个文件名（有可能是一个路径）

PathIsExe
验证路径是否是可执行文件
注意:不仅仅是.exe,还有.bat、.com、.src等
PathIsRoot 
路径是否为根路径
PathIsRelative
判断路径是否是相对路径
PathIsContentType 
检测文件是否为制定类型
例如: 
PathIsContentType("hello.txt","text/plain") 返回TRUE
PathIsContentType("hello.txt","image/gif") 返回FALSE
PathIsHTMLFile 
判断路径是否是html文件类型——根据系统注册类型判断
PathIsLFNFileSpec 
判断路径是否是长路径格式
PathIsNetworkPath 
判断路径是否是一个网络路径。
PathIsPrefix 
判断路径是否含有指定前缀
PathIsSameRoot 
判断路径是否有相同根目录
PathIsSlow 
判断路径是否是一个高度延迟的网络连接
PathIsSystemFolder 
判断路径是否有系统属性（属性可以自己设定）
PathIsUNC 
路径是否是UNC格式（网络路径）
PathIsUNCServer 
路径是否是UNC服务器
PathIsUNCServerShare 
路径是否仅仅是UNC的共享路径格式
PathIsURL 
路径是否是http格式。
PathYetAnotherMakeUniqueName 
基于已存在的文件,自动创建一个唯一的文件名。比如存在"新建文件",此函数会创建文件名"新建文件(2)"

*/

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
#if 0
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
#endif

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
    // 去掉最前面的.\ 
    if (szOutRelativePath[0] == _T('.') && szOutRelativePath[1] == _T('\\'))
    {
        TCHAR szTemp[MAX_PATH] = _T("");
        _tcscpy(szTemp, szOutRelativePath);
        _tcscpy(szOutRelativePath, szTemp+2);
    }
	return bRet;
}

//
//	计算szPath相对于文件szFile的相对路径szOutRelative
//
BOOL CalcRelativePathToFile(const TCHAR* szFile, const TCHAR* szPath, TCHAR* szOutRelativePath)
{
	if (NULL == szFile || NULL == szPath || NULL == szOutRelativePath)
	{
		return FALSE;
	}

	BOOL bRet = ::PathRelativePathTo( szOutRelativePath,
		szFile, FILE_ATTRIBUTE_NORMAL, 
		szPath, FILE_ATTRIBUTE_NORMAL );

	if (FALSE == bRet)
	{
		_tcscpy( szOutRelativePath, szPath );
	}

    // 去掉最前面的.\ 
    if (szOutRelativePath[0] == _T('.') && szOutRelativePath[1] == _T('\\'))
    {
        TCHAR szTemp[MAX_PATH] = _T("");
        _tcscpy(szTemp, szOutRelativePath);
        _tcscpy(szOutRelativePath, szTemp+2);
    }
	return bRet;
}
//
//	将相对于szDir的路径szRelative扩展为全路径szOut
//
BOOL CalcFullPathByRelative(const TCHAR* _szDir, const TCHAR* szRelative, TCHAR* szOut )
{
	if( NULL == _szDir || NULL == szRelative || NULL == szOut )
		return FALSE;

    TCHAR szDir[MAX_PATH] = _T("");
    _tcscpy(szDir, _szDir);
    if (!PathIsDirectory(szDir))
    {
        PathRemoveFileSpec(szDir);
    }

	TCHAR szPath [2*MAX_PATH] = _T("");

	int nLenDir = _tcslen(szDir);
	if (szRelative[0] != _T('\\') &&
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
	if (NULL == szPath || NULL == szOutDir)
		return FALSE;

	const TCHAR* p = _tcsrchr(szPath, _T('\\'));
	if (NULL == p)
		return FALSE;

	_tcscpy(szOutDir, p+1);
	return TRUE;
}

BOOL GetPathFileExt(const TCHAR* szPath, TCHAR* szOutExt)
{
    if( NULL == szPath || NULL == szOutExt )
        return FALSE;

    const TCHAR* p = _tcsrchr(szPath, _T('.'));
    if (NULL == p)
        return FALSE;

    _tcscpy(szOutExt, p+1);
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


UINT WtlGetFileName(LPCTSTR lpszPathName, _Out_opt_cap_(nMax) LPTSTR lpszTitle, UINT nMax)
{
	assert(NULL !=lpszPathName);
	assert(lpszPathName != NULL);

	// always capture the complete file name including extension (if present)
	LPTSTR lpszTemp = ::PathFindFileName(lpszPathName);

	// lpszTitle can be NULL which just returns the number of bytes
	if (lpszTitle == NULL)
		return lstrlen(lpszTemp)+1;

	// otherwise copy it into the buffer provided
	Checked::tcsncpy_s(lpszTitle, nMax, lpszTemp, _TRUNCATE);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// lpszCanon = C:\MYAPP\DEBUGS\C\TESWIN.C
//
// cchMax   b   Result
// ------   -   ---------
//  1- 7    F   <empty>
//  1- 7    T   TESWIN.C
//  8-14    x   TESWIN.C
// 15-16    x   C:\...\TESWIN.C
// 17-23    x   C:\...\C\TESWIN.C
// 24-25    x   C:\...\DEBUGS\C\TESWIN.C
// 26+      x   C:\MYAPP\DEBUGS\C\TESWIN.C

void _WtlAbbreviateName(_Inout_z_ LPTSTR lpszCanon, int cchMax, BOOL bAtLeastName)
{
	int cchFullPath, cchFileName, cchVolName;
	const TCHAR* lpszCur;
	const TCHAR* lpszBase;
	const TCHAR* lpszFileName;

	lpszBase = lpszCanon;
	cchFullPath = lstrlen(lpszCanon);

	cchFileName = WtlGetFileName(lpszCanon, NULL, 0) - 1;
	lpszFileName = lpszBase + (cchFullPath-cchFileName);

	// If cchMax is more than enough to hold the full path name, we're done.
	// This is probably a pretty common case, so we'll put it first.
	if (cchMax >= cchFullPath)
		return;

	// If cchMax isn't enough to hold at least the basename, we're done
	if (cchMax < cchFileName)
	{
		if (!bAtLeastName)
			lpszCanon[0] = _T('\0');
		else
			Checked::tcscpy_s(lpszCanon, cchFullPath + 1, lpszFileName);
		return;
	}

	// Calculate the length of the volume name.  Normally, this is two characters
	// (e.g., "C:", "D:", etc.), but for a UNC name, it could be more (e.g.,
	// "\\server\share").
	//
	// If cchMax isn't enough to hold at least <volume_name>\...\<base_name>, the
	// result is the base filename.

	lpszCur = lpszBase + 2;                 // Skip "C:" or leading "\\"

	if (lpszBase[0] == '\\' && lpszBase[1] == '\\') // UNC pathname
	{
		// First skip to the '\' between the server name and the share name,
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			assert(*lpszCur != '\0');
		}
	}
	// if a UNC get the share name, if a drive get at least one directory
	assert(*lpszCur == '\\');
	// make sure there is another directory, not just c:\filename.ext
	if (cchFullPath - cchFileName > 3)
	{
		lpszCur = _tcsinc(lpszCur);
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			assert(*lpszCur != '\0');
		}
	}
	assert(*lpszCur == '\\');

	cchVolName = int(lpszCur - lpszBase);
	if (cchMax < cchVolName + 5 + cchFileName)
	{
		Checked::tcscpy_s(lpszCanon, cchFullPath + 1, lpszFileName);
		return;
	}

	// Now loop through the remaining directory components until something
	// of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
	//
	// assert that the whole filename doesn't fit -- this should have been
	// handled earlier.

	assert(cchVolName + (int)lstrlen(lpszCur) > cchMax);
	while (cchVolName + 4 + (int)lstrlen(lpszCur) > cchMax)
	{
		do
		{
			lpszCur = _tcsinc(lpszCur);
			assert(*lpszCur != '\0');
		}
		while (*lpszCur != '\\');
	}

	// Form the resultant string and we're done.
	int cch;
	if (cchVolName >= 0 && cchVolName < cchMax)
		cch = cchVolName;
	else cch = cchMax;
	Checked::memcpy_s(lpszCanon + cch, cchFullPath + 1 - cch, _T("\\..."), sizeof(_T("\\...")) );
	Checked::tcscat_s(lpszCanon, cchFullPath + 1, lpszCur);
}


// 将路径中的 / 替换为  \ 
void PathReplace2Backslashes(TCHAR* szPath)
{
    if (NULL == szPath)
        return;

    TCHAR* p = szPath;
    while (*p)
    {
        if (*p == _T('/'))
            *p = _T('\\');

        p++;
    }
}

#endif
}}


