#include "stdafx.h"
#include "h\util.h"
#include <algorithm> // transform

namespace UI { namespace Util
{

#ifdef UTIL_STRING

//		在向DLL之间使用stl作为参数进行传递，会导致内存释放时的崩溃。因此这个Split函数仅能在模块内部使用
void  Split( const String& str, TCHAR szSep, vector<String>& vRet )
{
	int nIndex = 0;

	while( true )
	{
		int nResult = (int)str.find( szSep, nIndex );
		if( -1 == nResult )
		{
			vRet.push_back( str.substr( nIndex, str.length()-nIndex ) );
			break;
		}
		else
		{
			vRet.push_back( str.substr( nIndex, nResult-nIndex ) );
			nIndex = ++nResult;
		}
	}
}

//
//	函数:	void U2A( wchar_t*  wcsUnicode, CBufferT<char>& buf )
//
//	目的:	将一个Unicode字符转换成Ascill字符
//
//	参数:	
//		wcsUnicode
//			[in]:	要进行转换成Unicode字符
//		asciiBuf
//			[out]:	返回的Ascill字符
//
void U2A( const wchar_t*  wcsUnicode, CBufferT<char>& asciiBuf )
{
	int nLen = ::WideCharToMultiByte( CP_ACP, 0, wcsUnicode, wcslen( wcsUnicode )+1, NULL, 0, NULL, NULL );
	asciiBuf.malloc( nLen );
	::WideCharToMultiByte( CP_ACP, 0, wcsUnicode, wcslen( wcsUnicode )+1, (char*)asciiBuf, nLen, NULL, NULL  );
	asciiBuf[nLen-1] = L'';
}

void  A2U( const char*  strAscii, CBufferT<wchar_t>& unicodeBuf )
{
	int nLen = ::MultiByteToWideChar( CP_ACP, 0, strAscii, strlen( strAscii )+1, NULL, 0 );
	unicodeBuf.malloc( nLen );
	::MultiByteToWideChar( CP_ACP, 0, strAscii, strlen( strAscii )+1, (wchar_t*)unicodeBuf, nLen );
	unicodeBuf[nLen-1] = L'';
}

void UTF82U( const char* utf8, CBufferT<wchar_t>& unicodeBuf )
{
	int nLen = ::MultiByteToWideChar(CP_UTF8, NULL, utf8, strlen(utf8)+1, NULL, 0) ;
	unicodeBuf.malloc( nLen );
	::MultiByteToWideChar(CP_UTF8, NULL, utf8, strlen(utf8)+1, unicodeBuf, nLen);
	unicodeBuf[nLen-1] = L'';
}
void UTF82A( const char* utf8, CBufferT<char>& asciiBuf )
{
	CBufferT<wchar_t> unicodebuffer;

	UTF82U(utf8, unicodebuffer);
	U2A( unicodebuffer,asciiBuf );
}
void U2UTF8( const wchar_t* unicode, CBufferT<char>& asciiBuf )
{
	int nLen = ::WideCharToMultiByte(CP_UTF8, NULL, unicode, wcslen(unicode)+1, NULL, 0, NULL, NULL);
	asciiBuf.malloc(nLen);
	::WideCharToMultiByte(CP_UTF8, NULL, unicode, wcslen(unicode)+1, asciiBuf, nLen, NULL, NULL);
	asciiBuf[nLen-1] = L'';
}
void A2UTF8( const char* ascii, CBufferT<char>& asciiBuf )
{
	CBufferT<wchar_t> unicodebuffer;

	A2U(ascii, unicodebuffer);
	U2UTF8(unicodebuffer,asciiBuf);
}
//
//	函数:	void B2HS( const BYTE* bBin, int nBinLen, CBuffer& hexStrBuf )
//
//	目的:	将一个2进制字节流转换成16进制的字符串
//
//	参数:	
//		bBin
//			[in]:	需要进行转换的2进制数据
//		nBinLen
//			[in]:	2进制数据的长度
//		hexStrBuf
//			[out]:	返回的16进制字符串
//
void  B2HS( const BYTE* bBin, int nBinLen, CBuffer& hexStrBuf )
{
	hexStrBuf.malloc( nBinLen*2+1 );
	for(int i = 0; i < nBinLen; i++ )
	{
		TCHAR szTemp[3];
		_stprintf( szTemp, _T("%02X"), bBin[i] );
		_tcscat( (TCHAR*)hexStrBuf, szTemp );
	}
}



//
// 比较两个字符串，不区分大小写
//
int  CompareNoCase( const String& str1, const String& str2 )
{
// 
// 	transform(str1.begin(), str1.end(), str1.begin(), _totlower );
// 	transform(str2.begin(), str2.end(), str2.begin(), _totlower );
//	return str1.compare( str2 );

	return _tcsicmp( str1.c_str(), str2.c_str() );

}

void TestStr( int* p )
{
	p = new int[5];
}


COLORREF   TranslateRGB( const String& strCol, TCHAR szSep )
{
	vector<String>  vRGB;
	Split( strCol, szSep, vRGB );

	int nSize = (int)vRGB.size();
	if( 3 != nSize )
	{
		return 0;
	}

	COLORREF col =  RGB( 
		_ttoi( vRGB[0].c_str() ),
		_ttoi( vRGB[1].c_str() ),
		_ttoi( vRGB[2].c_str() ) );

	return col;
}

bool  TranslateRECT( const String& strRect, RECT* pRect, TCHAR szSep )
{
	vector<String> vsplit;
	Split( strRect, szSep, vsplit );

	int nSize = (int)vsplit.size();
	if( 4 <= nSize )
	{
		pRect->left   = _ttoi(vsplit[0].c_str());
		pRect->top    = _ttoi(vsplit[1].c_str());
		pRect->right  = _ttoi(vsplit[2].c_str());
		pRect->bottom = _ttoi(vsplit[3].c_str());
	}
	else if( 1 == nSize )
	{
		pRect->bottom = pRect->right = pRect->top = pRect->left = _ttoi(vsplit[0].c_str());
	}
	else if( 2 == nSize || 3 == nSize )
	{
		pRect->right = pRect->left = _ttoi(vsplit[0].c_str());
		pRect->bottom = pRect->top =  _ttoi(vsplit[1].c_str());
	}
	return true;
}





// Util中有使用到一些其它DLL中的临时数据，但仅仅为些而#include一个头文件不值得，因此在这个DLL内部再定义一次

//
// 9宫拉伸区域定义
//
class  Image9Region
{
public:
	Image9Region(){ Set(0); }
	void Set( WORD w );
	void Set( WORD wLeftRight, WORD wTopBottom );
	void Set( WORD wLeft, WORD wTop, WORD wRight, WORD wBottom );

	WORD topleft;
	WORD top;
	WORD topright;
	WORD left;
	WORD right;
	WORD bottomleft;
	WORD bottom;
	WORD bottomright;
};

inline void Image9Region::Set( WORD w )
{
	topleft = top = topright = left = right = bottomleft = bottom = bottomright = w;
}

inline void Image9Region::Set( WORD wLeftRight, WORD wTopBottom )
{
	topleft = topright = left = right = bottomleft = bottomright = wLeftRight;
	top = bottom = wTopBottom;
}

inline void Image9Region::Set( WORD wLeft, WORD wTop, WORD wRight, WORD wBottom )
{
	topleft = left = bottomleft = wLeft;
	top = wTop;
	topright = right = bottomright = wRight;
	bottom = wBottom;
}

bool TranslateImage9Region( const String& str, void* p9Region, TCHAR szSep )
{
	if( NULL == p9Region )
		return false;

	Image9Region* pReal9Region = (Image9Region*)p9Region;

	vector<String> vsplit;
	Split( str, szSep, vsplit );

	int nSize = (int)vsplit.size();
	switch(nSize)
	{
	case 1:
		{
			int n1 = _ttoi(vsplit[0].c_str());
			pReal9Region->Set(n1);
		}
		break;

	case 2:
		{
			int n1 = _ttoi(vsplit[0].c_str());
			int n2 = _ttoi(vsplit[1].c_str());
			pReal9Region->Set(n1,n2);
		}
		break;

	case 4:
		{
			int n1 = _ttoi(vsplit[0].c_str());
			int n2 = _ttoi(vsplit[1].c_str());
			int n3 = _ttoi(vsplit[2].c_str());
			int n4 = _ttoi(vsplit[3].c_str());
			pReal9Region->Set(n1,n2,n3,n4);
		}
		break;

	case 8:
		{
			int n1 = _ttoi(vsplit[0].c_str());
			int n2 = _ttoi(vsplit[1].c_str());
			int n3 = _ttoi(vsplit[2].c_str());
			int n4 = _ttoi(vsplit[3].c_str());
			int n5 = _ttoi(vsplit[4].c_str());
			int n6 = _ttoi(vsplit[5].c_str());
			int n7 = _ttoi(vsplit[6].c_str());
			int n8 = _ttoi(vsplit[7].c_str());
			pReal9Region->topleft  = n1;
			pReal9Region->top      = n2;
			pReal9Region->topright = n3;
			pReal9Region->left     = n4;
			pReal9Region->right    = n5;
			pReal9Region->bottomleft  = n6;
			pReal9Region->bottom      = n7;
			pReal9Region->bottomright = n8;
		}
		break;

	default:
		return false;
	}
	
	return true;
}

/*
** 不固定参数形式的函数,返回strRet，注意要释放 SysFreeString(bstr);
**
** 调用方法：
**		String _cdecl I18N( TCHAR* szKey, ...)
		{
			assert( szKey != NULL );

			va_list argList;
			va_start(argList, szKey);
			String str;
			Util::FormatV(szKey, argList, str);
			va_end(argList);

			return str;
		}
**	
*/
void  FormatV( TCHAR* pszFormat, va_list argList, BSTR& bstr )
{
	int nLength = _vsctprintf( pszFormat, argList );

	TCHAR*  szBuffer = new TCHAR[ nLength + 1];
	::ZeroMemory( szBuffer, nLength + 1);

	/* 为了兼容VS2003编译，不使用_s版本 */
	_vstprintf_s( szBuffer, nLength+1, pszFormat, argList );
	//_vstprintf( szBuffer, pszFormat, argList );

	bstr = ::SysAllocString(szBuffer);
	delete [] szBuffer;
}

#endif
}}