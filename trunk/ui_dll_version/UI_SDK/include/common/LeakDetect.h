//////////////////////////////////////////////////////////////////////////
//
//   �ڴ�й¶���Ľ��汾������Ҫ��ÿ��ģ�����ڴ���ӣ�
//    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//
//	 ע�����������new�����ͻ������Ҫ�ڸ�cpp����� #undef newȡ���ù���
//
//////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG) && !defined(_IOSTREAM_)
#include <iostream>  // �������include���ᵼ��������ڴ�й¶�����뱨����������Ϊ��stl��ͻ��?
#endif

#ifdef _DEBUG
#	define DEBUG_CLIENTBLOCK new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
#	define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#	define new DEBUG_CLIENTBLOCK
#endif


// #ifdef _DEBUG
// #include "..\ui_sdk\include\other\vld.h"	// �ڴ�й¶��� <-- ��win7������Ч
// #endif
