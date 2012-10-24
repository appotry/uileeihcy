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

	player.m_bMute = false;
	player.m_byteVolumn = 100;
	player.m_byteBalance = 100;
	player.m_bytePlayMode = (byte)ALL_LOOP;
	player.m_bDirty = false;
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
		// 播放器配置信息

		if (m_xml.FindElem(_T("Player")))
		{
			String  str = m_xml.GetAttrib(_T("Mute"));
			if (_T("1")==str || _T("true")==str)
			{
				pData->player.m_bMute = true;
			}
			else
			{
				pData->player.m_bMute = false;
			}

			str = m_xml.GetAttrib(_T("Volume"));
			pData->player.m_byteVolumn = (byte)_ttoi(str.c_str());
			if (pData->player.m_byteVolumn > 100)
			{
				pData->player.m_byteVolumn = 100;
			}

			str = m_xml.GetAttrib(_T("Balance"));
			pData->player.m_byteBalance = (byte)_ttoi(str.c_str());
			if (pData->player.m_byteBalance < -100)
				pData->player.m_byteBalance = -100;
			if (pData->player.m_byteBalance > 100)
				pData->player.m_byteBalance = 100;

			str = m_xml.GetAttrib(_T("PlayMode"));
			pData->player.m_bytePlayMode = (byte)_ttoi(str.c_str());
		}


		//////////////////////////////////////////////////////////////////////////
		// 可视化配置信息

		if (m_xml.FindElem(_T("Visual")))
		{
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
	}
	while(0);

	return true;
}

bool CPlayerConfigData::Save(CConfigData* pData)
{
	bool  bNeedSave = false;
	TCHAR szText[64] = _T("");
	do
	{
		m_xml.ResetPos();
		if (false == m_xml.FindElem())    break;
		if (false == m_xml.IntoElem())     break;

		if (pData->player.m_bDirty)
		{
			if (false == m_xml.FindElem(_T("Player")))
			{
				m_xml.InsertElem(_T("Player"));
			}

			_stprintf(szText, _T("%d"), pData->player.m_bMute?1:0);
			m_xml.SetAttrib(_T("Mute"), szText);
			_stprintf(szText, _T("%d"), pData->player.m_byteVolumn);
			m_xml.SetAttrib(_T("Volume"), szText);
			_stprintf(szText, _T("%d"), pData->player.m_byteBalance);
			m_xml.SetAttrib(_T("Balance"), szText);
			_stprintf(szText, _T("%d"), pData->player.m_bytePlayMode);
			m_xml.SetAttrib(_T("PlayMode"), szText);
			bNeedSave = true;
		}

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

			bNeedSave = true;
		}
	}
	while(0);

	if (!bNeedSave)
	{
		return true;
	}

	String  strConfigXmlPath;
	this->GetConfigXmlPath(strConfigXmlPath);

	return m_xml.Save(strConfigXmlPath);
}		