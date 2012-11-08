#include <string>
using namespace std;

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


// ������д����
#define SAFE_DELETE(p) \
	if( NULL != p ) \
{ \
	delete p; \
	p = NULL; \
}

#define SAFE_ARRAY_DELETE(p) \
	if( NULL != p ) \
{ \
	delete[] p; \
	p = NULL; \
}

#define SAFE_RELEASE(p) \
	if( NULL != p ) \
{ \
	p->Release(); \
	p = NULL; \
}

#define SAFE_RELEASE2(p) \
	if( NULL != p ) \
{ \
	p.Release(); \
	p = NULL; \
}

#define SAFE_DELETE_GDIOBJECT(h) \
{\
	if( NULL != h )\
	{\
		BOOL b = ::DeleteObject(h);\
		if (0 == b) UIASSERT(b); \
		h = NULL;\
	}\
}

