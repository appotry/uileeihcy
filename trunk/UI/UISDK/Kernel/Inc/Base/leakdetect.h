#ifndef LEAKDETECT_H_DB346733_FCF0_4f4d_A7DE_DB09E9527BC4
#define LEAKDETECT_H_DB346733_FCF0_4f4d_A7DE_DB09E9527BC4

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


#endif // LEAKDETECT_H_DB346733_FCF0_4f4d_A7DE_DB09E9527BC4