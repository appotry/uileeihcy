#include "stdafx.h"
#include "h\util.h"
#include <algorithm> // transform

#define SAFE_RELEASE(p) \
    if (p) \
{ \
    (p)->Release(); \
    (p) = NULL; \
}

namespace UI { namespace Util
{

#ifdef UTIL_STRING

// ����DLL֮��ʹ��stl��Ϊ�������д��ݣ��ᵼ���ڴ��ͷ�ʱ�ı�����
// ����޸�Ϊ���ⲿ�ṩһ���ӿڣ��ⲿ������֮���ٽ��ӿ��ͷš�

class CSplitStringEnum : public ISplitStringEnum
{
public:
    virtual void  Release()
    {
        delete this;
    }
    virtual const TCHAR*  GetText(UINT nIndex)
    {   
        if (nIndex >= (int)m_vecString.size())
            return NULL;

        return m_vecString[nIndex].c_str();
    }
    virtual int  GetCount() 
    {
        return m_vecString.size();
    }

public:
    vector<String>  m_vecString;
};
int  SplitString(const TCHAR* szText, TCHAR szSep, ISplitStringEnum** ppEnum)
{
    if (NULL == szText || NULL == ppEnum)
        return 0;

    String str = szText;
    CSplitStringEnum* pEnum = new CSplitStringEnum;

    int nIndex = 0;
    while (true)
    {
        int nResult = (int)str.find(szSep, nIndex );
        if( -1 == nResult )
        {
            pEnum->m_vecString.push_back( str.substr( nIndex, str.length()-nIndex ) );
            break;
        }
        else
        {
            pEnum->m_vecString.push_back( str.substr( nIndex, nResult-nIndex ) );
            nIndex = ++nResult;
        }
    }

    *ppEnum = pEnum;
    return pEnum->m_vecString.size();
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
int U2A( const wchar_t*  wcsUnicode, char* pAsciiBuf, int nBufferSize )
{
	return ::WideCharToMultiByte( CP_ACP, 0, wcsUnicode, wcslen( wcsUnicode )+1, (char*)pAsciiBuf, nBufferSize, NULL, NULL  );
}

int U2UTF8(const wchar_t* unicode, char* pAsciiBuf, int nBufferSize)
{
    return ::WideCharToMultiByte(CP_UTF8, NULL, unicode, wcslen(unicode)+1, pAsciiBuf, nBufferSize, NULL, NULL);
}

int UTF82U(const char* utf8, wchar_t* pUnicodeBuf, int nBufferSize)
{
    return ::MultiByteToWideChar(CP_UTF8, NULL, utf8, strlen(utf8)+1, pUnicodeBuf, nBufferSize);
}

#if 0
void  A2U( const char*  strAscii, CBufferT<wchar_t>& unicodeBuf )
{
	int nLen = ::MultiByteToWideChar( CP_ACP, 0, strAscii, strlen( strAscii )+1, NULL, 0 );
	unicodeBuf.malloc( nLen );
	::MultiByteToWideChar( CP_ACP, 0, strAscii, strlen( strAscii )+1, (wchar_t*)unicodeBuf, nLen );
	unicodeBuf[nLen-1] = L'';
}


void UTF82A( const char* utf8, CBufferT<char>& asciiBuf )
{
	CBufferT<wchar_t> unicodebuffer;

	UTF82U(utf8, unicodebuffer);
	U2A( unicodebuffer,asciiBuf );
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

#endif

int ISO8859_1ToUnicode(const BYTE* pSrc, int nSrcLen, wchar_t* pBuffer, int nBufLen)
{
	#define CP_ISO_8859_1 28591
	return ::MultiByteToWideChar(CP_ISO_8859_1, 0, (LPCSTR)pSrc, nSrcLen, pBuffer, nBufLen);
}

// �����ַ�ת����16���Ƹ�ʽ����ʽ���д��䣬���� "��" ->"4E67"
void  UnicodeEncode(const WCHAR*  szText, TCHAR* szEncode)
{
    if (NULL == szText || NULL == szEncode)
        return;

    int  nByteSize = wcslen(szText)*2;
    byte*  pbyte = (byte*)szText;

    int high = 0;
    int low = 0;

    const int digit9 = '9';
    const int A = 'A' - 10;  // ��14��E����ô��Ӧ���� A + 4
    const int dight0 = '0';

    for (int i = 0; i < nByteSize; i++ )
    {
       // _itow(pbyte[i], szEncode+(2*i), 16);  <--  ����ʹ�ø÷���������a��Ӧ���� 0x3100������ú�����ת����3 1 0

       high = pbyte[i] & 0xF0;
       high >>= 4;
       low = pbyte[i] & 0x0F;

       szEncode[2*i]   = high>9 ? (high+A):(high+dight0);
       szEncode[2*i+1] = low>9  ? (low+A):(low+dight0);
    }
}

void  UnicodeDecode(const WCHAR* szText, TCHAR* szDecode)
{
    if (NULL == szText || NULL == szDecode)
        return;

    int  nSrcLen = wcslen(szText);
    nSrcLen --;  // ��������Խ��

    WCHAR* p = (WCHAR*)szText;
    byte* pDst = (byte*)szDecode;

    int high = 0;
    int low = 0;

    const int digit9 = '9';
    const int digit0 = '0';

    const int z = 'z';   // 122, 7a
    const int a = 'a';   // 122, 7a

    const int Z = 'Z';   // 90,  5a
    const int A = 'A';   // 90,  5a
    
    for (int i = 0, j = 0; i < nSrcLen; i+=2, j++ )
    {
        int p1 = p[i];
        int p2 = p[i+1];
        
        if (p1 <= digit9 && p1 >= digit0)
            high = p1 - digit0;

        else if (p1 <= Z && p1 >= A)
            high = p1 - A + 10;

        else if (p1 <= z && p1 >= a)
            high = p1 - a + 10;

        else
            continue;


        if (p2 <= digit9 && p2 >= digit0)
            low = p2 - digit0;

        else if (p2 <= Z && p2 >= A)
            low = p2 - A + 10;

        else if (p2 <= z && p2 >= a)
            low = p2 - a + 10;

        else
            continue;

        pDst[j] = (high << 4) + low;
    }
}

//
// �Ƚ������ַ����������ִ�Сд
//
int  CompareNoCase(const TCHAR* str1, const TCHAR* str2)
{
// 
// 	transform(str1.begin(), str1.end(), str1.begin(), _totlower );
// 	transform(str2.begin(), str2.end(), str2.begin(), _totlower );
//	return str1.compare( str2 );

	return _tcsicmp(str1, str2);
}

COLORREF  TranslateRGB(const TCHAR* szCol, TCHAR szSep)
{
    if (NULL== szCol)
        return 0;

    COLORREF col = 0;

    ISplitStringEnum* pEnum = NULL;
    int nSize = SplitString(szCol, szSep, &pEnum);
    if (NULL == pEnum)
        return col;

	if (3 == nSize)
	{
        col =  RGB( 
            _ttoi(pEnum->GetText(0)),
            _ttoi(pEnum->GetText(1)),
            _ttoi(pEnum->GetText(2)) );

    //    col |= 0xFF000000;  // a Ĭ��Ϊ255;
    }

    // r g b a
    else if (4 == nSize)
    {
        col =  RGB( 
            _ttoi(pEnum->GetText(0)),
            _ttoi(pEnum->GetText(1)),
            _ttoi(pEnum->GetText(2)) );

        col |= _ttoi(pEnum->GetText(3)) << 24;
    }

    SAFE_RELEASE(pEnum);
    return col;
}

COLORREF  TranslateHexColor(const TCHAR* szColor)
{
    if (NULL == szColor)
        return 0;

    long l = 0;
    _stscanf(szColor, _T("%x"), &l);
    return l;
}

bool  TranslateRECT(const TCHAR* szRect, RECT* pRect, TCHAR szSep)
{
    if (NULL == szRect)
        return false;

    ISplitStringEnum* pEnum = NULL;
	int nSize = SplitString(szRect, szSep, &pEnum);
	if (NULL == pEnum)
        return false;

    if (nSize >= 4)
	{
		pRect->left   = _ttoi(pEnum->GetText(0));
		pRect->top    = _ttoi(pEnum->GetText(1));
		pRect->right  = _ttoi(pEnum->GetText(2));
		pRect->bottom = _ttoi(pEnum->GetText(3));
	}
	else if (1 == nSize)
	{
		pRect->bottom = pRect->right = pRect->top = pRect->left = _ttoi(pEnum->GetText(0));
	}
	else if (2 == nSize || 3 == nSize)
	{
		pRect->right = pRect->left = _ttoi(pEnum->GetText(0));
		pRect->bottom = pRect->top =  _ttoi(pEnum->GetText(1));
	}

    SAFE_RELEASE(pEnum);
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

bool TranslateImage9Region(const TCHAR* szText, void* p9Region, TCHAR szSep )
{
	if (NULL == szText || NULL == p9Region)
		return false;

	Image9Region* pReal9Region = (Image9Region*)p9Region;

    ISplitStringEnum* pEnum = NULL;
    int nSize = SplitString(szText, szSep, &pEnum);
    if (0 == nSize || NULL == pEnum)
        return false;

	switch(nSize)
	{
	case 1:
		{
			int n1 = _ttoi(pEnum->GetText(0));
			pReal9Region->Set(n1);
		}
		break;

	case 2:
		{
			int n1 = _ttoi(pEnum->GetText(0));
			int n2 = _ttoi(pEnum->GetText(1));
			pReal9Region->Set(n1,n2);
		}
		break;

	case 4:
		{
			int n1 = _ttoi(pEnum->GetText(0));
			int n2 = _ttoi(pEnum->GetText(1));
			int n3 = _ttoi(pEnum->GetText(2));
			int n4 = _ttoi(pEnum->GetText(3));
			pReal9Region->Set(n1,n2,n3,n4);
		}
		break;

	case 8:
		{
			int n1 = _ttoi(pEnum->GetText(0));
			int n2 = _ttoi(pEnum->GetText(1));
			int n3 = _ttoi(pEnum->GetText(2));
			int n4 = _ttoi(pEnum->GetText(3));
			int n5 = _ttoi(pEnum->GetText(4));
			int n6 = _ttoi(pEnum->GetText(5));
			int n7 = _ttoi(pEnum->GetText(6));
			int n8 = _ttoi(pEnum->GetText(7));
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
        SAFE_RELEASE(pEnum);
		return false;
	}

    SAFE_RELEASE(pEnum);
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

#ifdef _UNICODE
	bstr = ::SysAllocString(szBuffer);
#else
	bstr = ::SysAllocString(CA2W(szBuffer));
#endif
	delete [] szBuffer;
}

#endif
}}