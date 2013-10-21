#include "stdafx.h"
#include "h\util.h"
#include "shobjidl.h"
#include "shlobj.h"
#include "shellapi.h"

/*
windows·������API����
PathRemoveArgs 
ȥ��·���Ĳ���
PathRemoveBackslash 
ȥ��·�����ķ�б��"\"
PathAddBackslash 
��·�������Ϸ�б��"\"
PathRemoveBlanks 
ȥ��·��ǰ��Ŀո�
PathAddExtension 
���ļ�·�����������չ��
PathRemoveExtension 
ȥ���ļ�·����չ��
PathRenameExtension
�����ļ�·����չ��
PathRemoveFileSpec 
ȥ���ļ���,�õ�Ŀ¼
PathUnquoteSpaces
ȥ��·���е���β�ո�
PathQuoteSpaces 
�ж�·�����Ƿ��пո�,�еĻ�,������""���Ű�����·����������
PathAppend 
��һ��·��׷�ӵ���һ��·������
PathCombine 
�ϲ�����·��
PathSkipRoot
ȥ��·���еĴ��̷���UNC����
PathStripPath 
ȥ��·���е�Ŀ¼����,�õ��ļ���
PathStripToRoot
ȥ��·�����ļ�����,�õ���Ŀ¼
PathCompactPath
��������ֵ���ɷ��ϳ��ȵ�·��
��ԭʼ·��: C:\path1\path2\sample.txt
����120���ؽضϺ�Ϊ: C:\pat...\sample.txt
����25���ؽضϺ�Ϊ: ...\sample.txt
PathCompactPathEx 
�����ַ����������ɷ��ϳ��ȵ�·��
PathSetDlgItemPath 
��·���������õ��Ի�����ӿؼ���
PathUndecorate 
ȥ��·���е�����

PathUnExpandEnvStrings 
��·���в��������滻Ϊϵͳ����������ʽ 

PathFindOnPath 
��·���в���·��
PathFindExtension 
����·������չ��
PathFindFileName 
��ȡ·�����ļ���
PathFindNextComponent 
����ƥ��·��
PathFindSuffixArray 
���Ҹ������ļ����Ƿ��и����ĺ�׺

PathGetArgs 
��ȡ·������
PathGetCharType
��ȡ·���ַ�����
PathGetDriveNumber
�����߼��̷�������������� 

PathRelativePathTo
����һ��·������һ��·�������·����

PathResolve 
��һ�����·�������·��ת��Ϊһ���ϸ��·��
PathCanonicalize 
�淶��·��������ʽ�Ƚ��ҵ�·������ɹ淶��·����ʽ
PathBuildRoot 
���ݸ����Ĵ�����Ŵ�����Ŀ¼·��
CreateDirectory
����Ŀ¼
GetShortPathName 
����·��תΪ8.3��ʽ�Ķ�·����ʽ
GetLongPathName 
����·����ʽתΪ��·����
PathGetShortPath 
����·��תΪ��·����ʽ��8.3��ʽ��
PathCreateFromUrl 
��URL·��תΪMS-DOS��ʽ
PathMakePretty 
��·��ȫ��תΪСд,���ӿɶ���

PathMakeSystemFolder 
��·������ϵͳ����
PathUnmakeSystemFolder 
ȥ��·���е�ϵͳ����
PathMakeUniqueName 
��ģ�崴��ͳһ��·����ʽ
PathProcessCommand 
����һ����ִ�е�·��,�����в�����,���Զ���·����""����
PathCleanupSpec 
ȥ��·���в��Ϸ����ַ�
PathCommonPrefix 
�Ƚϲ���ȡ����·����ͬ��ǰ׺
PathFileExists 
��֤·���Ƿ����
PathMatchSpec 
�ж�·���Ƿ�ƥ���ƶ�����չ��
PathIsDirectory 
�ж�·���Ƿ���һ����Ч��Ŀ¼
PathIsFileSpec
��֤·���Ƿ�һ���ļ������п�����һ��·����

PathIsExe
��֤·���Ƿ��ǿ�ִ���ļ�
ע��:��������.exe,����.bat��.com��.src��
PathIsRoot 
·���Ƿ�Ϊ��·��
PathIsRelative
�ж�·���Ƿ������·��
PathIsContentType 
����ļ��Ƿ�Ϊ�ƶ�����
����: 
PathIsContentType("hello.txt","text/plain") ����TRUE
PathIsContentType("hello.txt","image/gif") ����FALSE
PathIsHTMLFile 
�ж�·���Ƿ���html�ļ����͡�������ϵͳע�������ж�
PathIsLFNFileSpec 
�ж�·���Ƿ��ǳ�·����ʽ
PathIsNetworkPath 
�ж�·���Ƿ���һ������·����
PathIsPrefix 
�ж�·���Ƿ���ָ��ǰ׺
PathIsSameRoot 
�ж�·���Ƿ�����ͬ��Ŀ¼
PathIsSlow 
�ж�·���Ƿ���һ���߶��ӳٵ���������
PathIsSystemFolder 
�ж�·���Ƿ���ϵͳ���ԣ����Կ����Լ��趨��
PathIsUNC 
·���Ƿ���UNC��ʽ������·����
PathIsUNCServer 
·���Ƿ���UNC������
PathIsUNCServerShare 
·���Ƿ������UNC�Ĺ���·����ʽ
PathIsURL 
·���Ƿ���http��ʽ��
PathYetAnotherMakeUniqueName 
�����Ѵ��ڵ��ļ�,�Զ�����һ��Ψһ���ļ������������"�½��ļ�",�˺����ᴴ���ļ���"�½��ļ�(2)"

*/

namespace UI { namespace Util
{
//	���ļ���صĲ���

#ifdef UTIL_FILE


//
//	����:	BOOL GetFileBuffer( TCHAR* szPath, CBufferT<char>&	fileBuf )
//
//	Ŀ��:	��ȡһ���ļ����ֽ�����
//
//	����:	
//		szPath
//			[in]:	Ҫ��ȡ���ļ�·��
//		fileBuf
//			[out]:	�����ļ��ֽ�����
//
//	��ע��
//		fileBuf.GetBufSize()�ɻ�ȡ���С
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
//	����:	BOOL OpenFolderAndSelectFile( TCHAR* szFullPath )
//	 
//	Ŀ�ģ�	���ҵĵ��ԣ�����λ��ָ�����ļ����ļ���
//
//	����:	
//		szFullPath
//			[in]:	Ҫ��λ�����ļ����ļ���
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
//	����:	void ShowFileProp( TCHAR* szFile )
//	 
//	Ŀ�ģ�	��һ���ļ������ԶԻ���
//
//	����:	
//		szFile
//			[in]:	Ҫ��ʾ�ĸ��ļ�������
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
//	����:	void ExecFile( TCHAR* szFile, BOOL bWait = FALSE ) 
//	 
//	Ŀ�ģ�	����һ������
//
//	����:	
//		szFile
//			[in]:	Ҫִ�е��ļ���·��
//		bWait:
//			[in]:	�Ƿ�Ҫ�ȴ��ó������н���
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
//	�ж�һ��·���Ƿ���ȫ·��
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
//	����szPath������ļ���szDir�����·��szOutRelative
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
    // ȥ����ǰ���.\ 
    if (szOutRelativePath[0] == _T('.') && szOutRelativePath[1] == _T('\\'))
    {
        TCHAR szTemp[MAX_PATH] = _T("");
        _tcscpy(szTemp, szOutRelativePath);
        _tcscpy(szOutRelativePath, szTemp+2);
    }
	return bRet;
}

//
//	����szPath������ļ�szFile�����·��szOutRelative
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

    // ȥ����ǰ���.\ 
    if (szOutRelativePath[0] == _T('.') && szOutRelativePath[1] == _T('\\'))
    {
        TCHAR szTemp[MAX_PATH] = _T("");
        _tcscpy(szTemp, szOutRelativePath);
        _tcscpy(szOutRelativePath, szTemp+2);
    }
	return bRet;
}
//
//	�������szDir��·��szRelative��չΪȫ·��szOut
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
//	��ȡһ��·��szPath���ڵ�Ŀ¼szOutDir
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
//	��ȡһ��·��szPath�е��ļ���
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
//	����һ���յ�xml�ļ����ļ����ݰ��� <? ?> ����rootԪ��<ui>,�����һ�����<node>
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

	// �������һ��rootԪ�أ�����markup�᷵��ʧ��
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


// �����ļ���
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


// ��·���е� / �滻Ϊ  \ 
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


