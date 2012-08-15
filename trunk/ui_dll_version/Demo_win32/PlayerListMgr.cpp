#include "StdAfx.h"
#include "PlayerListMgr.h"

CPlayerListMgr::CPlayerListMgr(void)
{
}

CPlayerListMgr::~CPlayerListMgr(void)
{
}

bool CPlayerListMgr::Initialize()
{
	m_data.Load();
	return true;
}
bool CPlayerListMgr::Release()
{
	return true;
}