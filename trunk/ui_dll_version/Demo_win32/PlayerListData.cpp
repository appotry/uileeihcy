#include "stdafx.h"
#include "PlayerListData.h"


void  CPlayerListData::Load()
{
	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	_tcscat(szPath, _T("player.xml"));

	CMarkup  xml;
	xml.Load(szPath);


	return;
}