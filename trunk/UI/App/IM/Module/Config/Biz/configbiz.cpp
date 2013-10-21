#include "stdafx.h"
#include "configbiz.h"

CConfigXmlData::CConfigXmlData()
{
    skin.m_bDirty = false;
    skin.m_byteAlpha = 255;
    skin.m_byteTextureAlpha = 255;
}


ConfigBiz::ConfigBiz()
{

}

ConfigBiz::~ConfigBiz()
{

}

void  ConfigBiz::GetConfigXmlPath(TCHAR* szPath)
{
    UI::Util::GetAppPath_(szPath);
    _tcscat(szPath, _T("im.xml"));
}

long ConfigBiz::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (IM::BASE_CMD_ID_INIT == nCommand)
    {
        Load();
    }
    else if (IM::BASE_CMD_ID_EXIT == nCommand)
    {
        Save();
    }
    return 0;
}


bool  ConfigBiz::Load()
{
    TCHAR szXmlPath[MAX_PATH] = _T("");
    GetConfigXmlPath(szXmlPath);
    if (false == m_xml.Load(szXmlPath))
        return false;

    do
    {
        if (false == m_xml.FindElem())    break;
        if (false == m_xml.IntoElem())    break;

        //////////////////////////////////////////////////////////////////////////
        // ∆§∑Ù≈‰÷√–≈œ¢

        if (m_xml.FindElem(_T("skin")))
        {
            m_xmlData.skin.m_strCurSkinPath = m_xml.GetAttrib(_T("curtheme"));
            m_xmlData.skin.m_byteAlpha = (byte)_ttoi(m_xml.GetAttrib(_T("alpha")).c_str());
            m_xmlData.skin.m_byteTextureAlpha = (byte)_ttoi(m_xml.GetAttrib(_T("texturealpha")).c_str());

            if (m_xml.IntoElem())
            {
                m_xml.SavePos();
                if (m_xml.FindElem(_T("big")) && m_xml.IntoElem())
                {
                    while (m_xml.FindElem(_T("theme")))
                    {
                        m_xmlData.skin.m_vecThemeBig.push_back(m_xml.GetAttrib(_T("xml")));
                    }
                    m_xml.OutOfElem();
                }
                m_xml.RestorePos();
                if (m_xml.FindElem(_T("small_row1")) && m_xml.IntoElem())
                {
                    while (m_xml.FindElem(_T("theme")))
                    {
                        m_xmlData.skin.m_vecThemeSmallR1.push_back(m_xml.GetAttrib(_T("xml")));
                    }
                    m_xml.OutOfElem();
                }
                m_xml.RestorePos();
                if (m_xml.FindElem(_T("small_row2")) && m_xml.IntoElem())
                {
                    while (m_xml.FindElem(_T("theme")))
                    {
                        m_xmlData.skin.m_vecThemeSmallR2.push_back(m_xml.GetAttrib(_T("xml")));
                    }
                    m_xml.OutOfElem();
                }
                m_xml.RestorePos();
                m_xml.OutOfElem();
            }
        }
    }
    while(0);

    return true;
}

bool ConfigBiz::Save()
{
    bool  bNeedSave = false;
    TCHAR szText[64] = _T("");
    do
    {
        m_xml.ResetPos();
        if (false == m_xml.FindElem())    break;
        if (false == m_xml.IntoElem())     break;

        if (m_xmlData.skin.m_bDirty)
        {
            if (false == m_xml.FindElem(_T("skin")))
            {
                m_xml.InsertElem(_T("skin"));
            }
            TCHAR szText[32] = _T("");

            m_xml.SetAttrib(_T("curtheme"), m_xmlData.skin.m_strCurSkinPath);
            m_xml.SetAttrib(_T("alpha"), (int)m_xmlData.skin.m_byteAlpha);
            m_xml.SetAttrib(_T("texturealpha"), (int)m_xmlData.skin.m_byteTextureAlpha);

            bNeedSave = true;
            m_xmlData.skin.m_bDirty = false;
        }
    }
    while(0);

    if (!bNeedSave)
        return true;

    TCHAR szXmlPath[MAX_PATH] = _T("");
    GetConfigXmlPath(szXmlPath);
    return m_xml.Save(szXmlPath);
}		

void ConfigBiz::GetCurSkinPath(TCHAR* szText)
{
    if (NULL == szText)
        return;

    const TCHAR* p = m_xmlData.skin.m_strCurSkinPath.c_str();
    if (UI::Util::IsFullPath(p))
    {
        _tcsncpy(szText, p, MAX_PATH-1);
    }
    else
    {
        TCHAR szXmlPath[MAX_PATH] = _T("");
        GetConfigXmlPath(szXmlPath);
        UI::Util::CalcFullPathByRelative(szXmlPath, p, szText);
    }
//    return m_xmlData.skin.m_strCurSkinPath.c_str();
}
void  ConfigBiz::SetCurSkinPath(const TCHAR* szText)
{
    if (NULL == szText)
        return;

    m_xmlData.skin.m_strCurSkinPath = szText;
    m_xmlData.skin.m_bDirty = true;
}
byte  ConfigBiz::GetSkinAlpha()
{
    return m_xmlData.skin.m_byteAlpha;
}
void  ConfigBiz::SetSkinAlpha(byte b)
{
    m_xmlData.skin.m_byteAlpha = b;
    m_xmlData.skin.m_bDirty = true;
}
byte  ConfigBiz::GetSkinTextureAlpha()
{
    return m_xmlData.skin.m_byteTextureAlpha;
}
void  ConfigBiz::SetSkinTextureAlpha(byte b)
{
    m_xmlData.skin.m_byteTextureAlpha = b;
    m_xmlData.skin.m_bDirty = true;
}