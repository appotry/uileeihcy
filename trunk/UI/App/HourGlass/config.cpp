#include "stdafx.h"
#include "config.h"
#include "3rd/markup/markup.h"

CConfig*  GetConfig()
{
    static CConfig _s;
    return &_s;
}

CConfig::CConfig()
{
    m_bPlayMusic = false;

    m_nBreakTime = 180;
    m_nHour = 0;
    m_nMinute = 30;
    m_nSecond = 0;
    m_bDirty = false;
	m_nMainWndTransparent = 255;
	m_hls_s = 0;
	m_hls_h = 0;
}

CConfig::~CConfig()
{
}

void CConfig::Load()
{
	TCHAR szPath[MAX_PATH] = {0};
	UI::Util::GetAppPath_(szPath);
	_tcscat(szPath, _T("HourGlass.xml"));

	CMarkup xml;
	if (!xml.Load(szPath))
		return;

	if (!xml.FindElem())  
		return;
	if (!xml.IntoElem())
		return;

    xml.SavePos();
	if (xml.FindElem(_T("picture")) && xml.IntoElem())
	{
		if (xml.FindElem(_T("folder")))
		{
			m_strPicFolder = xml.GetData();
		}
		xml.OutOfElem();
	}

    xml.RestorePos();
    if (xml.FindElem(_T("misc")))
    {
        m_nHour = _ttoi(xml.GetAttrib(_T("hour")).c_str());
        m_nMinute = _ttoi(xml.GetAttrib(_T("minute")).c_str());
        m_nSecond = _ttoi(xml.GetAttrib(_T("second")).c_str());
        m_nBreakTime = _ttoi(xml.GetAttrib(_T("breaktime")).c_str());
		m_nMainWndTransparent = _ttoi(xml.GetAttrib(_T("transparent")).c_str());
		m_hls_h = _ttoi(xml.GetAttrib(_T("hls_h")).c_str());
		m_hls_s = _ttoi(xml.GetAttrib(_T("hls_s")).c_str());

        if (0 == m_nBreakTime)
        {
            m_nBreakTime = 60;
            m_bDirty = true;
        }
        if (0 == m_nHour && 0 == m_nMinute && 0 == m_nSecond)
        {
            m_nMinute = 30;
            m_bDirty = true;
        }
        if (0 == m_nMainWndTransparent)
        {
            m_nMainWndTransparent = 255;
        }
    }
}

void  CConfig::Save()
{
    if (!m_bDirty)
        return;

    TCHAR szPath[MAX_PATH] = {0};
    UI::Util::GetAppPath_(szPath);
    _tcscat(szPath, _T("HourGlass.xml"));

    CMarkup xml;
    if (!xml.Load(szPath))
        return;

    if (!xml.FindElem())  
        return;
    if (!xml.IntoElem())
        return;

    xml.SavePos();
    {
        if (!xml.FindElem(_T("picture")))
            xml.AddElem(_T("picture"));
        xml.IntoElem();

        if (!xml.FindElem(_T("folder")))
            xml.AddElem(_T("folder"));

        xml.SetData(m_strPicFolder);
        xml.OutOfElem();
    }
    xml.RestorePos();
    {
        if (!xml.FindElem(_T("misc")))
            xml.AddElem(_T("misc"));
        
        TCHAR szText[64] = {0};
        _stprintf(szText, _T("%d"), m_nHour);
        xml.SetAttrib(_T("hour"), szText);

        _stprintf(szText, TEXT("%d"), m_nMinute);
        xml.SetAttrib(_T("minute"), szText);

        _stprintf(szText, TEXT("%d"), m_nSecond);
        xml.SetAttrib(_T("second"), szText);

        _stprintf(szText, TEXT("%d"), m_nBreakTime);
        xml.SetAttrib(_T("breaktime"), szText);

		_stprintf(szText, TEXT("%d"), m_nMainWndTransparent);
		xml.SetAttrib(_T("transparent"), szText);

		_stprintf(szText, TEXT("%d"), m_hls_h);
		xml.SetAttrib(_T("hls_h"), szText);

		_stprintf(szText, TEXT("%d"), m_hls_s);
		xml.SetAttrib(_T("hls_s"), szText);
    }

    xml.Save(szPath);
}

const TCHAR*  CConfig::GetPicFolder()
{
	return m_strPicFolder.c_str();
}

void  CConfig::SetPicFolder(const TCHAR* szPath)
{
    if (!szPath)
        return;

    m_bDirty = true; 
    m_strPicFolder = szPath;
}
void  CConfig::SetTime(int nHour, int nMinute, int nSecond)
{
    if (m_nHour == nHour && m_nMinute == nMinute && m_nSecond == nSecond)
        return;

    m_bDirty = true;
    m_nHour = nHour;
    m_nMinute = nMinute;
    m_nSecond = nSecond;
}
void  CConfig::SetBreakTime(int nBreakTime)
{
    if (m_nBreakTime == nBreakTime)
        return;

    m_bDirty = true;
    m_nBreakTime = nBreakTime;
}

int   CConfig::GetHour()
{
    return m_nHour;
}
int   CConfig::GetMinute()
{
    return m_nMinute;
}
int   CConfig::GetSecond()
{
    return m_nSecond;
}
int   CConfig::GetBreakTime()
{
    return m_nBreakTime;
}


void  CConfig::SetTransparent(int n)
{
	if (m_nMainWndTransparent == n)
		return;

	m_bDirty = true;
	m_nMainWndTransparent = n;
}
int   CConfig::GetTransparent()
{
	return m_nMainWndTransparent;
}
void  CConfig::SetHls_h(int h)
{
	if (h == m_hls_h)
		return;

	m_bDirty = true;
	m_hls_h = h;
}
int   CConfig::GetHls_h()
{
	return m_hls_h;
}
void  CConfig::SetHls_s(int s)
{
	if (s == m_hls_s)
		return;

	m_bDirty = true;
	m_hls_s = s;
}
int   CConfig::GetHls_s()
{
	return m_hls_s;
}