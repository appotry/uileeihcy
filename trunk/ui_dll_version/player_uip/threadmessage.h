#pragma once
#include "player_uip.h"
//////////////////////////////////////////////////////////////////////////
//
//  线程通知消息定义

class DSMSG_PARAM
{
};

#define DSMSG_DIRECTSHOW_NOTIFY  (WM_USER)

// 设置当前的进度
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

#define  DSMSG_CLEAR     (WM_USER+7)
#define  DSMSG_PLAYFILE  (WM_USER+8)

class DSMSG_PARAM_PLAYFILE : public DSMSG_PARAM
{
public:
	DSMSG_PARAM_PLAYFILE() 
	{ 
		int nSize = sizeof(szFilePath);
		memset(szFilePath, 0, nSize); 
		nSize = sizeof(szFileExt);
		memset(szFileExt, 0, nSize); 
	}
	TCHAR  szFileExt[MAX_PATH];
	TCHAR  szFilePath[MAX_PATH];

	bool   SetPath(const TCHAR* sz, const TCHAR* szExt)
	{
		if (NULL == sz || NULL == szExt)
			return false;

		_tcsncpy(szFilePath, sz, MAX_PATH-1);
		_tcsncpy(szFileExt, szExt, MAX_PATH-1);
		return true;
	}
};

#define  DSMSG_PLAYFILE_RSP  (WM_USER+9)
class DSMSG_PARAM_PLAYFILE_RSP : public DSMSG_PARAM_PLAYFILE
{
public:
	DSMSG_PARAM_PLAYFILE_RSP() { hr = E_FAIL; }
	HRESULT  hr;
};

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

inline DSMSG_PARAM* BuildPlayFileParam(const TCHAR* szFilePath, const TCHAR* szExt)
{
	DSMSG_PARAM_PLAYFILE* p = new DSMSG_PARAM_PLAYFILE;
	if (false == p->SetPath(szFilePath, szExt))
	{
		delete p;
		return NULL;
	}
	return (DSMSG_PARAM*)p;
}

inline DSMSG_PARAM* BuildPlayFileRspParam(const TCHAR* szFilePath, const TCHAR* szExt, HRESULT hr)
{
	DSMSG_PARAM_PLAYFILE_RSP* p = new DSMSG_PARAM_PLAYFILE_RSP;
	if (false == p->SetPath(szFilePath, szExt))
	{
		delete p;
		return NULL;
	}
	p->hr = hr;
	return (DSMSG_PARAM*)p;
}

//////////////////////////////////////////////////////////////////////////


class CAutoCriticalSection
{
public:
	CAutoCriticalSection(CRITICAL_SECTION* pCS)
	{
		m_pCS = pCS;
		::EnterCriticalSection(pCS);
	}
	~CAutoCriticalSection()
	{
		if (NULL != m_pCS)
		{
			LeaveCriticalSection(m_pCS);
		}
	}

private:
	CRITICAL_SECTION*  m_pCS;
};
#define DECLARE_CS_PROTECT   CAutoCriticalSection __cs__(&this->m_cs);