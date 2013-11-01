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
	int		dwLength = 0;
	TCHAR	*p = NULL;
	TCHAR	szFilePath[MAX_PATH] = {0};

	dwLength = GetModuleFileName(GetModuleHandle(NULL), szFilePath, MAX_PATH);
	p = _tcsrchr(szFilePath, '\\');
	if(p != NULL)
		*(p+1) = 0;

	_tcscpy( strPath, _T("") );
	_tcscpy( strPath, szFilePath );
}

void GetAppPath( HINSTANCE hInst, TCHAR* strPath )
{
	int		dwLength = 0;
	TCHAR	*p = NULL;
	TCHAR	szFilePath[MAX_PATH] = {0};

	dwLength = GetModuleFileName(hInst, szFilePath, MAX_PATH);
	p = _tcsrchr(szFilePath, '\\');
	if(p != NULL)
		*p = 0;

	_tcscpy( strPath, _T("") );
	_tcscpy( strPath, szFilePath );
}
void GetAppPath_( HINSTANCE hInst, TCHAR* strPath )
{
	int		dwLength = 0;
	TCHAR	*p = NULL;
	TCHAR	szFilePath[MAX_PATH] = {0};

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

// ����������
// Software\\Microsoft\\Windows\\CurrentVersion\\Run
bool InstallAutoRun(BOOL bInstall, TCHAR* szName, TCHAR* szPath)
{
	if (!szName)
		return false;

	HKEY hKey = NULL;
	RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &hKey); 
	if (!hKey)
		return false;

	if (bInstall)
	{
		if (!szPath)
		{
			RegCloseKey(hKey);
			return false;
		}
		RegSetValueEx(hKey, szName, 0, REG_SZ, (byte*)szPath, (_tcslen(szPath)+1) * sizeof(TCHAR));
	}
	else
	{
		RegDeleteValue(hKey, szName);
	}
	RegCloseKey(hKey);

	return true;
}

// RegGetValue Requires Windows Vista or Windows XP Professional x64 Edition.
bool QueryAutoRun(TCHAR* szName, TCHAR* szPath)
{
	if (!szName)
		return false;

	HKEY hKey = NULL;
	RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),&hKey); 
	if (!hKey)
		return false;

	long lSize = 0;
	//RegGetValue(hKey, NULL, szName, RRF_RT_REG_SZ, NULL, NULL, (LPDWORD)&lSize);
	RegQueryValueEx(hKey, szName, 0, NULL, NULL, (LPDWORD)&lSize);
	if (0 == lSize)
	{
		RegCloseKey(hKey);
		return false;
	}

	if (szPath)
	{
		int nMax = MAX_PATH*sizeof(TCHAR);
		if (lSize > nMax)
			lSize = nMax;

		//RegGetValue(hKey, NULL, szName, RRF_RT_REG_SZ, NULL, (LPVOID)szPath, (LPDWORD)&lSize);
		RegQueryValueEx(hKey, szName, 0, NULL, (LPBYTE)szPath, (LPDWORD)&lSize);
	}
	RegCloseKey(hKey);
	return true;
}
#endif

}}