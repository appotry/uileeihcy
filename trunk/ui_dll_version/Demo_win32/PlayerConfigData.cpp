#include "stdafx.h"
#include "PlayerConfigData.h"
#include "player.h"
#include "PlayerListMgr.h"


//////////////////////////////////////////////////////////////////////////

CConfigData::CConfigData()
{
	visual.m_nFps = 25;
	visual.m_nType = 0;
	visual.m_bDirty = false;
}

//////////////////////////////////////////////////////////////////////////


CPlayerConfigData::~CPlayerConfigData()
{
}

void CPlayerConfigData::GetConfigXmlPath(String& str)
{
	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	_tcscat(szPath, _T("player.xml"));

	str = szPath;
}


bool  CPlayerConfigData::Load(CConfigData* pData)
{
	String strXmlConfigPath;
	this->GetConfigXmlPath(strXmlConfigPath);

	if (false == m_xml.Load(strXmlConfigPath.c_str()))
		return false;

	do
	{
		if (false == m_xml.FindElem())    break;
		if (false == m_xml.IntoElem())    break;

		//////////////////////////////////////////////////////////////////////////
		// 可视化配置信息

		if (false == m_xml.FindElem(_T("Visual")))   break;

		String  str = m_xml.GetAttrib(_T("Type"));
		if (!str.empty())
		{
			pData->visual.m_nType = _ttoi(str.c_str());
		}

		str = m_xml.GetAttrib(_T("FramesPerSec"));
		if (!str.empty())
		{
			pData->visual.m_nFps = _ttoi(str.c_str());
		}
		pData->visual.m_bDirty = false;
	}
	while(0);

	return true;
}

bool CPlayerConfigData::Save(CConfigData* pData)
{
	TCHAR szText[64] = _T("");
	do
	{
		m_xml.ResetPos();
		if (false == m_xml.FindElem())    break;
		if (false == m_xml.IntoElem())     break;

		if (pData->visual.m_bDirty)
		{
			if (false == m_xml.FindElem(_T("Visual")))
			{
				m_xml.InsertElem(_T("Visual"));
			}
			_stprintf(szText, _T("%d"), pData->visual.m_nType);
			m_xml.SetAttrib(_T("Type"), szText);
			_stprintf(szText, _T("%d"), pData->visual.m_nFps);
			m_xml.SetAttrib(_T("FramesPerSec"), szText);
		}
	}
	while(0);

	String  strConfigXmlPath;
	this->GetConfigXmlPath(strConfigXmlPath);

	return m_xml.Save(strConfigXmlPath);
}		