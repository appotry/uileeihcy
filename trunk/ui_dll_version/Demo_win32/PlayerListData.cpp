#include "stdafx.h"
#include "PlayerListData.h"
#include "player.h"
#include "PlayerListMgr.h"

CPlayerListData::~CPlayerListData()
{
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
			::GetPlayerListMgr()->OnLoadItem(strValue);
		}
	}
	while(0);


	
	return;
}




bool  CPlayerListData::Add(const String& strPath)
{
	String  strConfigXmlPath;
	this->GetConfigXmlPath(strConfigXmlPath);

	CMarkup xml;
	if (false == xml.Load(strConfigXmlPath))
		return false;

	bool bRet = false;
	do
	{
		if (false == xml.FindElem())    break;
		if (false == xml.IntoElem())    break;

		if (false == xml.FindElem(_T("playlist")))   break;
		if (false == xml.IntoElem())    break;

		if (false == xml.InsertElem(_T("item"), strPath))  break;
		if (false == xml.Save(strConfigXmlPath)) break;

		bRet = true;

	}while(0);

	return bRet;
}

void CPlayerListData::GetConfigXmlPath(String& str)
{
	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	_tcscat(szPath, _T("player.xml"));

	str = szPath;
}

