#include "stdafx.h"
#include "PlayerListData.h"
#include "App\Player\Player_UI\Mgr\player.h"
#include "App\Player\Player_UI\Mgr\PlayerListMgr.h"
#include "3rd/markup/markup.h"

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
			
			//	加载所有属性
            UI::IMapAttribute*  pMapAttrib = NULL;
            UI::UICreateIMapAttribute(&pMapAttrib);

			for (int j = 0; ; j++)
			{
				String key = xml.GetAttribName(j);
				if (_T("") == key)
					break;

				String value = xml.GetAttrib( key );
                pMapAttrib->AddAttr(key.c_str(), value.c_str());
			}

			String strValue = xml.GetData();
			::GetPlayerListMgr()->OnLoadItem(strValue, pMapAttrib);
            SAFE_RELEASE(pMapAttrib);
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
    UI::Util::GetAppPath_(szPath);
	_tcscat(szPath, _T("TTPlayer\\playerlist.xml"));

	str = szPath;
}

