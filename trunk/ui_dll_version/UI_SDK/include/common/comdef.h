#include <string>
using namespace std;

#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
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
		::DeleteObject(h);\
		h = NULL;\
	}\
}

