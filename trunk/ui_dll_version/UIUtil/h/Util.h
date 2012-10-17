#pragma once 
#include "BufferT.h"


// 如果要从标准 C++ 库中的类型派生，并且要编译调试版本 (/MTd)，而且编译器错误信息引用 _Container_base，则在 Microsoft Visual C++ 2005 中可以忽略 C4251。---vector<Object*>会报错
#pragma warning(disable:4251)
// This function or variable may be unsafe. Consider using freopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)


#include <windows.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <tchar.h>
#include <assert.h>

using namespace std;

#ifdef UIUTIL_EXPORTS
#define UIUTILAPI __declspec(dllexport)
#else
#define UIUTILAPI __declspec(dllimport)
#endif


#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
#endif

//
//	release版本下断言输出，可以利用debugview.exe看到断言错误
//  remark: __FILE__ ...这些都是char*类型的
//
#ifdef ASSERT
#undef ASSERT  // 避免使用ASSERT，请使用UIASSERT
#endif

#ifdef _DEBUG
#define UIASSERT(x)   assert(x)
#else
#define UIASSERT(x)                   \
	if(!(x))                          \
{                                 \
	char szInfo[2048] = "";       \
	sprintf(szInfo, "UIASSERT  expr(%s)  %s(L%d) ## %s", #x, __FILE__, __LINE__, __FUNCTION__ );  \
	::OutputDebugStringA(szInfo); \
}                                
#endif

typedef bool (CALLBACK* EnumFileInDirProc)(const TCHAR*, const TCHAR*, WPARAM);

//
//	定义要导出的函数接口集，如果要禁用某一util函数类型，则不要#define即可
//
#define UTIL_STRING
#define UTIL_APP
#define UTIL_FILE
#define UTIL_GDI

namespace UI { namespace Util {

#ifdef UTIL_CRYPT
	//=================================================================
	// Crypt
	//=================================================================
	BOOL UIUTILAPI MD5_Digest( BYTE* bOrign, int nOriLen, CBuffer *pDigestBuf );
	BOOL UIUTILAPI MD5_String_Digest( TCHAR* str, CBuffer* pDigestBuf );
	BOOL UIUTILAPI MD5_File_Digest( TCHAR* szPath, CBuffer* pDigestBuf );

	bool UIUTILAPI ImportPfxCert( TCHAR* szCertPath, TCHAR* szPassword, WCHAR* wcsImportToWhere, TCHAR* szErrInfo = NULL );
	bool UIUTILAPI RemoveCert( TCHAR* szSubject, TCHAR* szIssuer, WCHAR* wcsRemoveFromWhere, TCHAR* szErrInfo = NULL );
	bool UIUTILAPI ImportDerCert( TCHAR* szCertPath, WCHAR* wcsImportToWhere, TCHAR* szErrInfo = NULL );

	void UIUTILAPI Encrypt( BYTE* bOrign, int nOriLen, TCHAR* szPassword, CBuffer* pEncryptBuf );
#endif 

#ifdef UTIL_STRING
	//=================================================================
	// String
	//=================================================================
	void      UIUTILAPI U2A( const wchar_t*  wcsUnicode, CBufferT<char>& asciiBuf );
	void      UIUTILAPI A2U( const char*  strAscii, CBufferT<wchar_t>& unicodeBuf );
	void      UIUTILAPI B2HS( const BYTE* bBin, int nBinLen, CBuffer& hexStrBuf );
	void      UIUTILAPI UTF82U( const char* utf8, CBufferT<wchar_t>& unicodeBuf );
	void      UIUTILAPI UTF82A( const char* utf8, CBufferT<char>& asciiBuf );
	void      UIUTILAPI U2UTF8( const wchar_t* unicode, CBufferT<char>& asciiBuf );
	void      UIUTILAPI A2UTF8( const char*  ascii, CBufferT<char>& asciiBuf );

	int       UIUTILAPI CompareNoCase( const String& str1, const String& str2 );

	void      UIUTILAPI FormatV( TCHAR* pszFormat, va_list argList, BSTR& bstr );

	bool      UIUTILAPI TranslateRECT( const String& strRect, RECT* pRect, TCHAR szSep = _T(',') );
	COLORREF  UIUTILAPI TranslateRGB( const String& strCol, TCHAR szSep = _T(',') );
	bool      UIUTILAPI TranslateImage9Region( const String& str, void* p9Region, TCHAR szSep = _T(',') );
	
#endif

#ifdef UTIL_FILE
	//=================================================================
	// File
	//=================================================================
	BOOL UIUTILAPI GetFileBuffer( TCHAR* szPath, CBufferT<char>& fileBuf );
	BOOL UIUTILAPI OpenFolderAndSelectFile( TCHAR* szFullPath );

	void UIUTILAPI ShowFileProp( TCHAR* szFile );
	void UIUTILAPI ExecFile( TCHAR* szFile, BOOL bWait = FALSE );
	
	BOOL UIUTILAPI GetPathDir( const TCHAR* szPath, TCHAR* szOutDir );
	BOOL UIUTILAPI GetPathFileName(const TCHAR* szPath, TCHAR* szOutFileName);
	BOOL UIUTILAPI IsFullPath( const TCHAR* szPath );
	BOOL UIUTILAPI CalcRelativePathToDir(const TCHAR* szDir, const TCHAR* szPath, TCHAR* szOutRelativePath );
	BOOL UIUTILAPI CalcRelativePathToFile(const TCHAR* szFile, const TCHAR* szPath, TCHAR* szOutRelativePath );
	BOOL UIUTILAPI CalcFullPathByRelative(const TCHAR* szDir, const TCHAR* szRelative, TCHAR* szOut ); 
	

	//bool EnumFileInDirProc(TCHAR* szFileName);
	BOOL UIUTILAPI EnumFileInDirectory(const TCHAR* szDir, EnumFileInDirProc proc, WPARAM wParam);

	bool UIUTILAPI CreateEmptyXmlFileA( const char* szPath, const char* szRoot, const char* szNode = NULL );
	bool UIUTILAPI CreateEmptyXmlFileW( const wchar_t* szPath, const wchar_t* szRoot, const wchar_t* szNode = NULL );

#ifdef _UNICODE
#	define CreateEmptyXmlFile  CreateEmptyXmlFileW
#else
#	define CreateEmptyXmlFile  CreateEmptyXmlFileA
#endif
#endif

#ifdef UTIL_APP
	//=================================================================
	// Application
	//=================================================================
	void UIUTILAPI GetAppPath( HINSTANCE hInst, TCHAR* strPath );
	void UIUTILAPI GetAppPath_( HINSTANCE hInst, TCHAR* strPath );
	void UIUTILAPI GetAppPath( TCHAR* strPath );
	void UIUTILAPI GetAppPath_( TCHAR* strPath );
	bool UIUTILAPI IsKeyDown( UINT vk );
//	int  UIUTILAPI RectW( const RECT* prc );
//	int  UIUTILAPI RectH( const RECT* prc );
	void UIUTILAPI DeflatRect( RECT* pfc, RECT* pDeflatRc );
	
#endif

#ifdef UTIL_DEBUG

	void UIUTILAPI CreateDumpFileWhenCrash( String strFilePath, bool bAddTime );

#endif

#ifdef UTIL_OTHER
	//
	//	UnCheck
	//
	int UIUTILAPI base64_encode(PBYTE bin_data, LONG bin_size, PCHAR base64_data, PLONG base64_size, INT Mode);
	int UIUTILAPI base64_decode(PCHAR base64_data, LONG base64_size, PBYTE bin_data, PLONG bin_size);
#endif

#ifdef UTIL_GDI
	int  UIUTILAPI FontSize2Height(int nSize);
	int  UIUTILAPI FontHeight2Size(int nHeight);

	BOOL UIUTILAPI IsFontExist(const TCHAR* pszFaceName);
	BOOL UIUTILAPI GradientFillH( HDC hDC, const RECT* prc, COLORREF colFrom, COLORREF colTo );
	BOOL UIUTILAPI GradientFillV( HDC hDC, const RECT* prc, COLORREF colFrom, COLORREF colTo );

#endif

}}