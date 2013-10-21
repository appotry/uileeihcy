#pragma once 

// ���Ҫ�ӱ�׼ C++ ���е���������������Ҫ������԰汾 (/MTd)�����ұ�����������Ϣ���� _Container_base������ Microsoft Visual C++ 2005 �п��Ժ��� C4251��---vector<Object*>�ᱨ��
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
#include <OleAuto.h>  // BSTR
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
//	release�汾�¶����������������debugview.exe�������Դ���
//  remark: __FILE__ ...��Щ����char*���͵�
//
#ifdef ASSERT
#undef ASSERT  // ����ʹ��ASSERT����ʹ��UIASSERT
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
//	����Ҫ�����ĺ����ӿڼ������Ҫ����ĳһutil�������ͣ���Ҫ#define����
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
	int       UIUTILAPI U2A(const wchar_t*  wcsUnicode, char* pAsciiBuf, int nBufferSize);
    int       UIUTILAPI U2UTF8(const wchar_t* unicode, char* pAsciiBuf, int nBufferSize);
    int       UIUTILAPI UTF82U(const char* utf8, wchar_t* pUnicodeBuf, int nBufferSize);
#if 0
	void      UIUTILAPI A2U( const char*  strAscii, CBufferT<wchar_t>& unicodeBuf );
	void      UIUTILAPI B2HS( const BYTE* bBin, int nBinLen, CBuffer& hexStrBuf );
	void      UIUTILAPI UTF82A( const char* utf8, CBufferT<char>& asciiBuf );
	void      UIUTILAPI A2UTF8( const char*  ascii, CBufferT<char>& asciiBuf );
#endif
	int       UIUTILAPI ISO8859_1ToUnicode(const BYTE* pSrc, int nSrcLen, wchar_t* pBuffer, int nBufLen);
    void      UIUTILAPI UnicodeEncode(const WCHAR*  szText, TCHAR* szEncode);  // szEncode�ĳ���Ϊ = new TCHAR[4*wcslen(szText) + 1]
    void      UIUTILAPI UnicodeDecode(const WCHAR*  szText, TCHAR* szDecode);

	int       UIUTILAPI CompareNoCase(const TCHAR* str1, const TCHAR* str2);

	void      UIUTILAPI FormatV( TCHAR* pszFormat, va_list argList, BSTR& bstr );

	bool      UIUTILAPI TranslateRECT(const TCHAR* strRect, RECT* pRect, TCHAR szSep = _T(',') );
	COLORREF  UIUTILAPI TranslateRGB(const TCHAR* strCol, TCHAR szSep = _T(',') );
    COLORREF  UIUTILAPI TranslateHexColor(const TCHAR* szColor);
	bool      UIUTILAPI TranslateImage9Region(const TCHAR* str, void* p9Region, TCHAR szSep = _T(',') );

    interface ISplitStringEnum
    {
        virtual void  Release() = 0;
        virtual const TCHAR* GetText(UINT nIndex) = 0;
        virtual int   GetCount() = 0;
    };
	int UIUTILAPI SplitString(const TCHAR* szText, TCHAR szSep, ISplitStringEnum** pEnum);
#endif

#ifdef UTIL_FILE
	//=================================================================
	// File
	//=================================================================
#if 0
	BOOL UIUTILAPI GetFileBuffer( TCHAR* szPath, CBufferT<char>& fileBuf );
#endif
	BOOL UIUTILAPI OpenFolderAndSelectFile( TCHAR* szFullPath );

	void UIUTILAPI ShowFileProp( TCHAR* szFile );
	void UIUTILAPI ExecFile( TCHAR* szFile, BOOL bWait = FALSE );
	
	BOOL UIUTILAPI GetPathDir( const TCHAR* szPath, TCHAR* szOutDir );
	BOOL UIUTILAPI GetPathFileName(const TCHAR* szPath, TCHAR* szOutFileName);
    BOOL UIUTILAPI GetPathFileExt(const TCHAR* szPath, TCHAR* szOutExt);
	BOOL UIUTILAPI IsFullPath( const TCHAR* szPath );
	BOOL UIUTILAPI CalcRelativePathToDir(const TCHAR* szDir, const TCHAR* szPath, TCHAR* szOutRelativePath );
	BOOL UIUTILAPI CalcRelativePathToFile(const TCHAR* szFile, const TCHAR* szPath, TCHAR* szOutRelativePath );
	BOOL UIUTILAPI CalcFullPathByRelative(const TCHAR* szDir, const TCHAR* szRelative, TCHAR* szOut ); 
	void UIUTILAPI _WtlAbbreviateName(_Inout_z_ LPTSTR lpszCanon, int cchMax, BOOL bAtLeastNam);
    void UIUTILAPI PathReplace2Backslashes(TCHAR* szPath);

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
	void UIUTILAPI DeflatRect( RECT* pfc, RECT* pDeflatRc );
	
	bool UIUTILAPI InstallAutoRun(BOOL bInstall, TCHAR* szName, TCHAR* szPath);
	bool UIUTILAPI QueryAutoRun(TCHAR* szName, TCHAR* szPath);
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

	LONG UIUTILAPI HimetricXtoDX(LONG xHimetric, LONG xPerInch=-1);
	LONG UIUTILAPI HimetricYtoDY(LONG yHimetric, LONG yPerInch=-1);
	LONG UIUTILAPI DXtoHimetricX(LONG dx, LONG xPerInch=-1);
	LONG UIUTILAPI DYtoHimetricY(LONG dy, LONG yPerInch=-1);

	BOOL UIUTILAPI IsLayeredWindow(HWND hWnd);
    void UIUTILAPI FixGdiAlpha(HDC hDC, RECT* lprc);
#endif

}}