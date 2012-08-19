#include "stdafx.h"
#include "h\util.h"


namespace UI { namespace Util
{

#ifdef UTIL_APP

//
//	函数:	void GetAppPath( TCHAR* strPath )
//	
//	目的:	获取应用程序所在文件夹，不包括后面的\\
//
//	参数:
//		strPath
//			[out]:	返回文件夹路径
//
void GetAppPath( TCHAR* strPath )
{
	int		dwLength;
	TCHAR	*p = NULL;
	TCHAR	szFilePath[MAX_PATH];

	dwLength = GetModuleFileName(GetModuleHandle(NULL), szFilePath, MAX_PATH);
	p = _tcsrchr(szFilePath, '\\');
	if(p != NULL)
		*p = 0;

	_tcscpy( strPath, _T("") );
	_tcscpy( strPath, szFilePath );
}

//
//	函数:	void GetAppPath_( TCHAR* strPath )
//	
//	目的:	获取应用程序所在文件夹，包括后面的\\
//
//	参数:
//		strPath
//			[out]:	返回文件夹路径
//
void GetAppPath_( TCHAR* strPath )
{
	int		dwLength;
	TCHAR	*p = NULL;
	TCHAR	szFilePath[MAX_PATH];

	dwLength = GetModuleFileName(GetModuleHandle(NULL), szFilePath, MAX_PATH);
	p = _tcsrchr(szFilePath, '\\');
	if(p != NULL)
		*(p+1) = 0;

	_tcscpy( strPath, _T("") );
	_tcscpy( strPath, szFilePath );
}

void GetAppPath( HINSTANCE hInst, TCHAR* strPath )
{
	int		dwLength;
	TCHAR	*p = NULL;
	TCHAR	szFilePath[MAX_PATH];

	dwLength = GetModuleFileName(hInst, szFilePath, MAX_PATH);
	p = _tcsrchr(szFilePath, '\\');
	if(p != NULL)
		*p = 0;

	_tcscpy( strPath, _T("") );
	_tcscpy( strPath, szFilePath );
}
void GetAppPath_( HINSTANCE hInst, TCHAR* strPath )
{
	int		dwLength;
	TCHAR	*p = NULL;
	TCHAR	szFilePath[MAX_PATH];

	dwLength = GetModuleFileName(hInst, szFilePath, MAX_PATH);
	p = _tcsrchr(szFilePath, '\\');
	if(p != NULL)
		*(p+1) = 0;

	_tcscpy( strPath, _T("") );
	_tcscpy( strPath, szFilePath );
}

// 检测一个按键是否按下，如CTRL,SHIFT,ALT
bool IsKeyDown( UINT vk )
{
	return ( ( GetKeyState( vk ) & 0x8000 ) != 0 );  // 或者 ( GetKeyState( VK_CONTROL ) < 0 ) ??
}

int RectW( const RECT* prc )
{
	return prc->right - prc->left;
}
int RectH( const RECT* prc )
{
	return prc->bottom - prc->top;
}
// 用于将一个RECT缩小一个PADDING/MARGIN的大小
void DeflatRect( RECT* pfc, RECT* pDeflatRc )
{
	pfc->left   += pDeflatRc->left;
	pfc->top    += pDeflatRc->top;
	pfc->right  -= pDeflatRc->right;
	pfc->bottom -= pDeflatRc->bottom;
}

#endif

}}