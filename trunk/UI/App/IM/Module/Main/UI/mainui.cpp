#include "stdafx.h"
#include "mainui.h"
#include "maindlg.h"
#include "App\IM\include\task.h"
#include "App\IM\include\main_inc.h"
#include "App\IM\include\privatechat_inc.h"
#include "App\IM\include\skin_inc.h"
#include "App\IM\Module\Main\Biz\mainbiz.h"
#include "App\IM\include\skin_inc.h"


inline bool  IsEqualEvent(const TCHAR* szName, WPARAM wParam)
{
    if (!szName || !wParam)
        return false;

    return 0 == _tcscmp(szName, (TCHAR*)wParam);
}


CMainUI::CMainUI()
{
    m_pMainDlg = NULL;
    m_pPrivateChatBiz = NULL;
    m_pPrivateChatUI = NULL;
    m_pSkinUI = NULL;
}

CMainUI::~CMainUI()
{
    SAFE_DELETE_Ixxx(m_pMainDlg);

    vector<ToolbarBtnPlugin*>::iterator iter = m_vecPlugins.begin();
    for (; iter != m_vecPlugins.end(); iter++)
    {
        ToolbarBtnPlugin* p = *iter;
        SAFE_DELETE(p);
    }
    m_vecPlugins.clear();
}


long CMainUI::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (nCommand == IM::BASE_CMD_ID_TASK)
    {
        IM::ITask* pTask = (IM::ITask*)wParam;
//         switch (pTask->GetCommandId())
//         {
//         }
    }
    else if (nCommand == IM::BASE_CMD_ID_EVENT)
    {
        if (IsEqualEvent(EVENT_SKIN_TEXTURE_ALPHA_CHANGED, wParam))
        {
            int nAlpha = (int)lParam;
            if (m_pMainDlg)
            {
                m_pMainDlg->ChangeTextureAlpha(nAlpha);
            }
        }
    }
    else if (nCommand == IM::BASE_CMD_ID_INIT)
    {
        AddToolbarPlugin(&m_btnAddPlugin, 0x7FFFFFFF);
    }

    return 0;
}

void  CMainUI::Quit()
{
    if (m_pMainDlg)
    {
        m_pMainDlg->HideWindow();
    }
    
    IM::GetFramework()->PostFunction(IM::Biz, IM::create_mem_fun0_pkg(&CMainBiz::OnQuit, g_pMainBiz), false);
}

long  CMainUI::Create()
{
    if (NULL == m_pMainDlg)
    {
        CMainDlg::CreateInstance(IM::GetUIApplication(), &m_pMainDlg);
        m_pMainDlg->SetMainUI(this);

        m_pMainDlg->Create(IM::GetUIApplication(), _T("maindlg"), NULL);

        if (NULL == m_pSkinUI)
        {
            m_pSkinUI = (IM::ISkinUI*)IM::GetFramework()->GetUI(MODULE_ID_SKIN);
        }
        if (m_pSkinUI)
        {
            IM::GetFramework()->SubscribeUIEvent(EVENT_SKIN_TEXTURE_ALPHA_CHANGED, static_cast<IM::IUI*>(this));
            if (NULL == m_pMainDlg->GetMaterialRender())
            {
                m_pMainDlg->SetMaterialRender(m_pSkinUI->GetShareSkinRender());
            }
            m_pMainDlg->ChangeTextureAlpha(m_pSkinUI->GetTextureAlpha());
        }
    }
    return 0;
}
long  CMainUI::Show()
{
    if (m_pMainDlg)
    {
        m_pMainDlg->Show();
    }
    return 0;
}
long CMainUI::DestroyWindow()
{
    if (m_pMainDlg)
    {
        ::DestroyWindow(m_pMainDlg->GetHWND());
        SAFE_DELETE_Ixxx(m_pMainDlg);
    }
    return 0;
}

void  CMainUI::OpenPrivateChatDlg(const TCHAR* szUserId, int nOpenMode, WPARAM wParam, LPARAM lParam)
{
    if (g_pMainBiz)
    {
        IM::OpenPrivateChatDlgReqTask*  pTask = new IM::OpenPrivateChatDlgReqTask(g_pMainBiz, szUserId);
        pTask->m_lOpenMode = nOpenMode;
        pTask->m_wParam = wParam;
        pTask->m_lParam = lParam;
        IM::GetFramework()->PostFunction(IM::Biz, IM::create_mem_fun1_pkg(&CMainBiz::OpenPrivateChatDlg, g_pMainBiz, pTask), false);
    }
}

void  CMainUI::ShowSkinDlg()
{
    if (!m_pSkinUI)
        return;

    m_pSkinUI->Show();
}

void  CMainUI::ChangeSkinAlpha(int n)
{
    if (m_pMainDlg)
    {
        m_pMainDlg->ChangeTextureAlpha(n);
    }
}

long CMainUI::AddToolbarPlugin(IM::IMainDlgToolbarPlugin* pPlugin, int nSort)
{
    if (!m_pMainDlg)
        return 0;

    UI::IButton* pBtn = m_pMainDlg->InsertPluginBtn2Servicebar(pPlugin, nSort);
    if (pBtn)
    {
        ToolbarBtnPlugin*  pData = new ToolbarBtnPlugin;
        pData->m_pPlugin = pPlugin;
        pData->m_pButton = pBtn;
        pData->nSort = nSort;
        m_vecPlugins.push_back(pData);

        pPlugin->OnCreate(pBtn);
    }

    return 0;
}

long  CMainUI::GetToolbarPluginCount()
{
    return (long)m_vecPlugins.size();
}

IM::IMainDlgToolbarPlugin*  CMainUI::GetToolbarPluginByIndex(int i, int* pnSort)
{
    if (i < 0 || i >= GetToolbarPluginCount())
        return NULL;

    if (pnSort)
    {
        *pnSort = m_vecPlugins[i]->nSort;
    }
    return m_vecPlugins[i]->m_pPlugin;
}
void  CMainUI::SetToolbarPluginSort(IM::IMainDlgToolbarPlugin* pPlugin, int nSort)
{
    ToolbarBtnPlugin* pData = FindToolbarPluginByPlugin(pPlugin);
    if (NULL == pData)
        return;

    pData->nSort = nSort;
    m_pMainDlg->ChangeToolbarPluginSort(pData->m_pButton, nSort);
}

void  CMainUI::ShowToolbarPluginBtn(IM::IMainDlgToolbarPlugin* pPlugin, bool bShow)
{
    ToolbarBtnPlugin* pData = FindToolbarPluginByPlugin(pPlugin);
    if (pData)
    {
        m_pMainDlg->ChangeToolbarPluginVisible(pData->m_pButton, bShow);
    }
}

void  CMainUI::OnToolbarPluginBtnClick(UI::IButton* pBtn)
{
    vector<ToolbarBtnPlugin*>::iterator iter = m_vecPlugins.begin();
    for (; iter != m_vecPlugins.end(); iter++)
    {
        ToolbarBtnPlugin* p = *iter;
        if (p->m_pButton == pBtn)
        {
            p->m_pPlugin->OnClick();
            break;
        }
    }
}

ToolbarBtnPlugin*  CMainUI::FindToolbarPluginByPlugin(IM::IMainDlgToolbarPlugin* p)
{
    vector<ToolbarBtnPlugin*>::iterator iter = m_vecPlugins.begin();
    for (; iter != m_vecPlugins.end(); iter++)
    {
        ToolbarBtnPlugin* pPlugin = *iter;
        if (pPlugin->m_pPlugin == p)
        {
            return pPlugin;
        }
    }
    return NULL;
}
ToolbarBtnPlugin*  CMainUI::FindToolbarPluginByButton(UI::IButton* p)
{
    vector<ToolbarBtnPlugin*>::iterator iter = m_vecPlugins.begin();
    for (; iter != m_vecPlugins.end(); iter++)
    {
        ToolbarBtnPlugin* pPlugin = *iter;
        if (pPlugin->m_pButton == p)
        {
            return pPlugin;
        }
    }
    return NULL;
}