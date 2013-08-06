#include "stdafx.h"
#include "h\util.h"


namespace UI { namespace Util
{

#ifdef UTIL_APP

//
//	����:	void GetAppPath( TCHAR* strPath )
//	
//	Ŀ��:	��ȡӦ�ó��������ļ��У������������\\
//
//	����:
//		strPath
//			[out]:	�����ļ���·��
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
//	����:	void GetAppPath_( TCHAR* strPath )
//	
//	Ŀ��:	��ȡӦ�ó��������ļ��У����������\\
//
//	����:
//		strPath
//			[out]:	�����ļ���·��
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

// ���һ�������Ƿ��£���CTRL,SHIFT,ALT
bool IsKeyDown( UINT vk )
{
	return ( ( GetKeyState( vk ) & 0x8000 ) != 0 );  // ���� ( GetKeyState( VK_CONTROL ) < 0 ) ??
}

int RectW( const RECT* prc )
{
	return prc->right - prc->left;
}
int RectH( const RECT* prc )
{
	return prc->bottom - prc->top;
}
// ���ڽ�һ��RECT��Сһ��PADDING/MARGIN�Ĵ�С
void DeflatRect( RECT* pfc, RECT* pDeflatRc )
{
	pfc->left   += pDeflatRc->left;
	pfc->top    += pDeflatRc->top;
	pfc->right  -= pDeflatRc->right;
	pfc->bottom -= pDeflatRc->bottom;
}

#endif

}}