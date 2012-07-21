#include "stdafx.h"
#include "h\util.h"
#include <algorithm> // transform

namespace UI { namespace Util
{

#ifdef UTIL_STRING

//		����DLL֮��ʹ��stl��Ϊ�������д��ݣ��ᵼ���ڴ��ͷ�ʱ�ı�����������Split����������ģ���ڲ�ʹ��
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
//	����:	void U2A( wchar_t*  wcsUnicode, CBufferT<char>& buf )
//
//	Ŀ��:	��һ��Unicode�ַ�ת����Ascill�ַ�
//
//	����:	
//		wcsUnicode
//			[in]:	Ҫ����ת����Unicode�ַ�
//		asciiBuf
//			[out]:	���ص�Ascill�ַ�
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
//	����:	void B2HS( const BYTE* bBin, int nBinLen, CBuffer& hexStrBuf )
//
//	Ŀ��:	��һ��2�����ֽ���ת����16���Ƶ��ַ���
//
//	����:	
//		bBin
//			[in]:	��Ҫ����ת����2��������
//		nBinLen
//			[in]:	2�������ݵĳ���
//		hexStrBuf
//			[out]:	���ص�16�����ַ���
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
// �Ƚ������ַ����������ִ�Сд
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





// Util����ʹ�õ�һЩ����DLL�е���ʱ���ݣ�������ΪЩ��#includeһ��ͷ�ļ���ֵ�ã���������DLL�ڲ��ٶ���һ��

//
// 9������������
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
** ���̶�������ʽ�ĺ���,����strRet��ע��Ҫ�ͷ� SysFreeString(bstr);
**
** ���÷�����
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

	/* Ϊ�˼���VS2003���룬��ʹ��_s�汾 */
	_vstprintf_s( szBuffer, nLength+1, pszFormat, argList );
	//_vstprintf( szBuffer, pszFormat, argList );

	bstr = ::SysAllocString(szBuffer);
	delete [] szBuffer;
}

#endif
}}