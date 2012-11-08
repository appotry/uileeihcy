#include <string>
using namespace std;

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


// 常用缩写定义
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

