#pragma once
#include "player_uip.h"
//////////////////////////////////////////////////////////////////////////
//
//  �߳�֪ͨ��Ϣ����

class DSMSG_PARAM
{
};


// ���õ�ǰ�Ľ���
#define  DSMSG_SET_CUR_POS    (WM_USER+1)    
class DSMSG_PARAM_SET_CUR_POS : public DSMSG_PARAM
{
public:
	double   dPercent;
};

#define  DSMSG_PLAY    (WM_USER+2)    
#define  DSMSG_PAUSE   (WM_USER+3)    
#define  DSMSG_STOP    (WM_USER+4) 
#define  DSMSG_QUIT    (WM_USER+5)

#define  DSMSG_SET_VISUALIZATION (WM_USER+6)
class DSMSG_PARAM_SET_VISUALIZATION : public DSMSG_PARAM
{
public:
	VisualizationInfo   info;
};

#define  DSMSG_CLEAR   (WM_USER+7)

inline DSMSG_PARAM* BuildSetCurPosParam(double dPercent)
{
	DSMSG_PARAM_SET_CUR_POS* p = new DSMSG_PARAM_SET_CUR_POS;
	p->dPercent = dPercent;
	return (DSMSG_PARAM*)p;
}

inline DSMSG_PARAM* BuildSetVisualizationParam(VisualizationInfo* pInfo)
{
	DSMSG_PARAM_SET_VISUALIZATION* p = new DSMSG_PARAM_SET_VISUALIZATION;
	p->info = *pInfo;
	return (DSMSG_PARAM*)p;
}