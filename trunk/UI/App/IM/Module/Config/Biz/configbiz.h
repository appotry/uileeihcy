#pragma once
#include "App\IM\include\config_inc.h"
#include "3rd\markup\markup.h"

class CConfigXmlData
{
public:
    CConfigXmlData();
    struct Skin
    {
        String  m_strCurSkinPath;  // 当前使用的皮肤图片路径名称
        byte  m_byteAlpha;
        byte  m_byteTextureAlpha;

        vector<String>  m_vecThemeBig;
        vector<String>  m_vecThemeSmallR1;
        vector<String>  m_vecThemeSmallR2;

        bool  m_bDirty;
    }skin;
};

class ConfigBiz : public IM::IConfigBiz
{
public:
    ConfigBiz();
    ~ConfigBiz();

    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);

    virtual  void  GetCurSkinPath(TCHAR* szText);
    virtual  void  SetCurSkinPath(const TCHAR* szText);
    virtual  byte  GetSkinAlpha();
    virtual  void  SetSkinAlpha(byte b);
    virtual  byte  GetSkinTextureAlpha();
    virtual  void  SetSkinTextureAlpha(byte b);

public:
    bool  Load();
    bool  Save();

protected:
    void  GetConfigXmlPath(TCHAR*);

private:
    CConfigXmlData   m_xmlData;
    CMarkup  m_xml;
};