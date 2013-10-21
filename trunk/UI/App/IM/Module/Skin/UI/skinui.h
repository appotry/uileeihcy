#pragma once
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\skin_inc.h"

class CSkinDlg;
struct ThemeData;
struct SkinData;

namespace IM
{
    struct IMainUI;
    struct IMainDlgToolbarPlugin;
}
class SkinUI : public IM::ISkinUI
{
public:
    SkinUI();
    ~SkinUI();

public:
    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);

    virtual UI::IImageRender*  GetShareSkinRender();
    virtual int   GetTextureAlpha() { return (int)m_byteTextureAlpha; }
    virtual void  Show();

public:
    long  ChangeSkin(ThemeData* pThemeData);
    long  InitSkinData(SkinData* pSkinData);
        
public:

    void  ChangeSkinByConfig(const TCHAR*  szXml);
    void  ChangeDefaultSkin();
    void  ChangeCustomSkin(const TCHAR*  szImgPath);

    void  ChangeSkin(const String&  strFile, int nImageDrawType, int nColor, int nHLSColor);
    void  ChangeSkinTransparent(int n);
    void  ChangeSkinTextureAlpha(int n);

    void  ShowToolbarPluginBtn(IM::IMainDlgToolbarPlugin* p, bool bShow);

protected:
    void  CreateShareSkinRender();

public:
    CSkinDlg*  m_pSkinDlg;
    IM::IMainUI*  m_pMainUI;

    UI::IImageRender*   m_pShareSkinRender;

    byte  m_byteAlpha;
    byte  m_byteTextureAlpha;  // 每个窗口显示之前获取一下
};