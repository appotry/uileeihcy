// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// UIBuilder.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������



#define UIBUILDER_LOG_LEVEL(l,c)     LOG_LEVEL( g_pLog, _T("ui_builder"),l,c )

void UIBUILDER_LOG_DEBUG( TCHAR* szContent, ... )
{
	UIBUILDER_LOG_LEVEL(LOG_LEVEL_DEBUG, szContent );
}
void UIBUILDER_LOG_INFO( TCHAR* szContent, ... )
{
	UIBUILDER_LOG_LEVEL(LOG_LEVEL_INFO, szContent );
}
void UIBUILDER_LOG_WARN( TCHAR* szContent, ... )
{
	UIBUILDER_LOG_LEVEL(LOG_LEVEL_WARN, szContent );
}
void UIBUILDER_LOG_ERROR( TCHAR* szContent, ... )
{
	UIBUILDER_LOG_LEVEL(LOG_LEVEL_ERROR, szContent );
}
void UIBUILDER_LOG_FATAL( TCHAR* szContent, ...)
{
	UIBUILDER_LOG_LEVEL(LOG_LEVEL_FATAL, szContent );
}

