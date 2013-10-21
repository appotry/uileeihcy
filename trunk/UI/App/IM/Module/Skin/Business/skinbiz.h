#pragma once
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\config_inc.h"


struct ThemeData
{
    TCHAR     szPath[MAX_PATH];
    int       nDrawType;
    COLORREF  bkcol;
    COLORREF  avgcol;
};

struct SkinData : public ThemeData
{
    byte  byteAlpha;
    byte  byteTextureAlpha;
};

class SkinBiz : public IM::IBiz
{
public:
    SkinBiz();
    ~SkinBiz();

public:
    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);
    
public:
    void  OnInitialize();
    bool  ParseSkinPackageXml(const TCHAR* szXmlPath, ThemeData* pData);
    long  ChangeSkinByConfig(BSTR bstrXml);
    long  OnSkinAlphaChanged(int n);
    long  OnSkinTextureAlphaChanged(int n);

private:
    long  _ChangeSkinByConfig(const TCHAR* szXml,  bool bWait);

private:
    IM::IConfigBiz*  m_pConfigBiz;
};