#include "stdafx.h"
#include "PlayerListData.h"

CPlayerListData::~CPlayerListData()
{
	int nSize = (int)m_vecPlayerList.size();
	for (int i = 0; i < nSize; i++)
	{
		PlayerListItemInfo* pInfo = m_vecPlayerList[i];
		SAFE_DELETE(pInfo);
	}
	m_vecPlayerList.clear();
}

void  CPlayerListData::Load()
{
	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	_tcscat(szPath, _T("player.xml"));

	CMarkup  xml;
	if (false == xml.Load(szPath))
		return;

	do
	{
		if (false == xml.FindElem())    break;
		if (false == xml.IntoElem())    break;

		if (false == xml.FindElem(_T("playlist")))   break;
		if (false == xml.IntoElem())    break;

		for ( ;; )
		{
			if (false == xml.FindElem(_T("item")))   break;
			
			String strValue = xml.GetData();

			PlayerListItemInfo* pInfo = new PlayerListItemInfo;
			pInfo->m_strFilePath = strValue;

			m_vecPlayerList.push_back(pInfo);
		}
	}
	while(0);


	return;
}