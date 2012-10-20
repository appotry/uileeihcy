#include "stdafx.h"
#include "PlayerListData.h"
#include "player.h"
#include "PlayerListMgr.h"

CPlayerListData::~CPlayerListData()
{
}

void  CPlayerListData::Load()
{
// 	TCHAR szPath[MAX_PATH] = _T("");
// 	Util::GetAppPath_(szPath);
// 	_tcscat(szPath, _T("playerlist.xml"));

	String strConfigXmlPath;
	this->GetConfigXmlPath(strConfigXmlPath);

	CMarkup  xml;
	if (false == xml.Load(strConfigXmlPath.c_str()))
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

		if (false == xml.FindElem(_T("playlist")))
		{
			xml.AddElem(_T("playlist"));
		}
		if (false == xml.IntoElem())    break;

		if (false == xml.AddElem(_T("item"), strPath))  break;   // 不要调用InsertElem,应该添加到最后面，而不是插入到第一个位置
		if (false == xml.Save(strConfigXmlPath)) break;

		bRet = true;

	}while(0);

	return bRet;
}

bool CPlayerListData::Remove(const String& strPath)
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

		bool bLoopRet = false;
		while (xml.FindElem())
		{
			if (xml.GetData() == strPath)
			{
				xml.RemoveElem();
				bLoopRet = true;
				break;
			}
		}
		if (false == bLoopRet)          break;
		if (false == xml.Save(strConfigXmlPath))     break;

		bRet = true;

	}while(0);

	return bRet;
}
bool CPlayerListData::RemoveAll()
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
		
		if (false == xml.RemoveElem())  break;
		xml.AddElem(_T("playlist"));

		if (false == xml.Save(strConfigXmlPath))     break;

		bRet = true;

	}while(0);

	return bRet;
}

void CPlayerListData::GetConfigXmlPath(String& str)
{
	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	_tcscat(szPath, _T("playerlist.xml"));

	str = szPath;
}

