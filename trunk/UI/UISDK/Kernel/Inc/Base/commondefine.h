#ifndef COMMONDEFINE_H_E5A89FDE_764D_4704_AD9B_F86D15A9AF38
#define COMMONDEFINE_H_E5A89FDE_764D_4704_AD9B_F86D15A9AF38

#ifndef interface
#define interface struct
#endif

#ifndef overwrite
#define overwrite virtual
#endif

#include <string>
#include <atltypes.h>
#include <assert.h>
using namespace std;

#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
#endif

typedef RECT  REGION4;
typedef CRect CRegion4;

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

#define RGBA(r,g,b,a)  (((BYTE)(r))|((WORD)(((BYTE)(g))<<8))|(DWORD(((BYTE)(b))<<16))|((DWORD)(((BYTE)(a))<<24)))

inline  int LPRECTW(RECT* prc) { return prc->right - prc->left; }
inline  int LPRECTH(RECT* prc) { return prc->bottom - prc->top; }
inline  int RECTW(RECT rc)     { return rc.right - rc.left; }
inline  int RECTH(RECT rc)     { return rc.bottom - rc.top; }

#define FUNC_NAME  _T(__FUNCTION__)

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
#define UIASSERT(x)               \
	if(!(x))                      \
{                                 \
	char szInfo[2048] = "";       \
	sprintf(szInfo, "UIASSERT  expr(%s)  %s(L%d) ## %s", #x, __FILE__, __LINE__, __FUNCTION__ );  \
	::OutputDebugStringA(szInfo); \
}                                
#endif

// ������д����
#define SAFE_DELETE(p) \
	if (p) \
{ \
	delete (p); \
	(p) = NULL; \
}

#define SAFE_ARRAY_DELETE(p) \
	if (p) \
{ \
	delete[] (p); \
	(p) = NULL; \
}

#define SAFE_RELEASE(p) \
	if (p) \
{ \
	(p)->Release(); \
	(p) = NULL; \
}

#define SAFE_RELEASE2(p) \
	if (p) \
{ \
	(p).Release(); \
	(p) = NULL; \
}

#define SAFE_DELETE_GDIOBJECT(h) \
{\
	if (h)\
	{\
		BOOL b = ::DeleteObject(h);\
		/*if (0 == b) UIASSERT(b);*/ \
		(h) = NULL;\
	}\
}

#define SAFE_DELETE_Ixxx(p) \
{\
	if (p)\
	{\
		(p)->delete_this(); \
		(p) = NULL;\
	}\
}

//
//	����һ��String���ͱ�����get/set����
//
//	����:
//		xxx
//			�����Ա��Ϊ m_strName����xxxȡֵΪ Name
//
//	ע�⣺
//		������dll/exeģ���в�Ҫ����Get����,��Ӧ����GetRef����,��������ʱ�����heap����
//
#define DECLARE_STRING_SETGET(xxx)    \
	const TCHAR*     Get##xxx()      { return m_str##xxx .c_str(); } \
	const String&    Get##xxx##Ref() { return m_str##xxx; } \
	void             Set##xxx( const String& str ) { m_str##xxx = str; };

#define DECLARE_bool_SETGET(xxx) \
	bool    Get##xxx() { return m_b##xxx; } \
	void    Set##xxx( bool b ) { m_b##xxx = b; }

#define DECLARE_byte_SETGET(xxx) \
	unsigned char  Get##xxx() { return m_byte##xxx; } \
	void    Set##xxx( byte b ) { m_byte##xxx = b; }

#define DECLARE_INT_SETGET(xxx) \
	int     Get##xxx() { return m_n##xxx; } \
	void    Set##xxx( int n ) { m_n##xxx = n; }

class IRootInterface
{
public:
	virtual ~IRootInterface() = 0 {};  // ȷ��deleteʱ�ܵ��õ����������������
};

template<class T>
class IUnknownImpl : public T
{
public:
	IUnknownImpl(){ m_lRef = 0;}
	virtual ~IUnknownImpl(){};  // ȷ���ͷ�ʱ�ܵ��õ����������������

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{ return E_NOTIMPL;	}
	virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {return ++m_lRef;}
	virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        --m_lRef; 
        if (0 == m_lRef)
        {
            delete this; return 0;
        } 
        return m_lRef;
    }

protected:
	ULONG  m_lRef;
};

template<class T>
class ISimpleUnknownImpl : public T
{
public:
	virtual ~ISimpleUnknownImpl(){};  // ȷ���ͷ�ʱ�ܵ��õ����������������

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{ return E_NOTIMPL;	}
	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{ return 1;	}
	virtual ULONG STDMETHODCALLTYPE Release(void)
	{ return 0;	}
};

#endif // COMMONDEFINE_H_E5A89FDE_764D_4704_AD9B_F86D15A9AF38