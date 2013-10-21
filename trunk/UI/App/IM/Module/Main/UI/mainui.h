#pragma once
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\main_inc.h"
#include "toolbaraddbtn.h"

class CMainDlg;

namespace IM
{
    struct ISkinUI;
}


class ToolbarBtnPlugin
{
public:
    ToolbarBtnPlugin()
    {
        m_pPlugin = NULL;
        m_pButton = NULL;
        nSort = 0;
    }
    
public:
    IM::IMainDlgToolbarPlugin*  m_pPlugin;
    UI::IButton*  m_pButton;
    int  nSort;
};

class CMainUI : public IM::IMainUI
{
public:
    CMainUI();
    ~CMainUI();

public:
    // IUI
    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);

public:
    // IMainUI
    virtual long  AddToolbarPlugin(IM::IMainDlgToolbarPlugin* pPlugin, int nSort=-1);
    virtual long  GetToolbarPluginCount();
    virtual IM::IMainDlgToolbarPlugin*  GetToolbarPluginByIndex(int i, int* pnSort);
    virtual void  SetToolbarPluginSort(IM::IMainDlgToolbarPlugin* pPlugin, int nSort);
    virtual void  ShowToolbarPluginBtn(IM::IMainDlgToolbarPlugin* pPlugin, bool bShow);

public:
    void  ChangeSkinAlpha(int n);
    void  Quit();
    void  OpenPrivateChatDlg(const TCHAR* szUserId, int nOpenMode, WPARAM wParam, LPARAM lParam);
    void  ShowSkinDlg();
    long  Create();
    long  Show();
    long  DestroyWindow();
    void  OnToolbarPluginBtnClick(UI::IButton* pBtn);

private:
    ToolbarBtnPlugin*  FindToolbarPluginByPlugin(IM::IMainDlgToolbarPlugin* p);
    ToolbarBtnPlugin*  FindToolbarPluginByButton(UI::IButton* p);

public:
    CMainDlg*  m_pMainDlg;
    
    IM::IBiz*  m_pPrivateChatBiz;
    IM::IUI*  m_pPrivateChatUI;

    IM::ISkinUI*  m_pSkinUI;
    
    // 插件数据
    vector<ToolbarBtnPlugin*>  m_vecPlugins;
    ToolbarAddBtn  m_btnAddPlugin;   // 自己的插件对象
};