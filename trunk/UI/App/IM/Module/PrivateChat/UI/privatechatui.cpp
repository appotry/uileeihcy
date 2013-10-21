#include "stdafx.h"
#include "privatechatui.h"
#include "chatdlg.h"
#include "App\IM\include\task.h"
#include "App\IM\include\privatechat_inc.h"
#include "App\IM\include\skin_inc.h"
#include "emotiondlg.h"
//////////////////////////////////////////////////////////////////////////

PrivateChatDlgInfo::PrivateChatDlgInfo()
{
    m_pChatDlg = NULL;
}
PrivateChatDlgInfo::~PrivateChatDlgInfo()
{
    if (m_pChatDlg)
    {
        if (::IsWindow(m_pChatDlg->GetHWND()))
        {
            DestroyWindow(m_pChatDlg->GetHWND());
        }
        SAFE_DELETE_Ixxx(m_pChatDlg);
    }
}

//////////////////////////////////////////////////////////////////////////
CPrivateChatUI::CPrivateChatUI()
{
    m_pSkinUI = NULL;
    m_pEmotionDlg = NULL;
}

CPrivateChatUI::~CPrivateChatUI()
{
}


long CPrivateChatUI::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (nCommand == IM::BASE_CMD_ID_TASK)
    {
        IM::ITask* pTask = (IM::ITask*)wParam;
        switch (pTask->GetCommandId())
        {
        case PRIVATECHAT_UI_CMD_DESTROYPRIVATECHATDLG:
            {
                DestroyDlg((CChatDlg*)pTask->GetwParam());
            }
            break;
        }
    }
    else if (IM::BASE_CMD_ID_EXIT == nCommand)
    {
        vector<PrivateChatDlgInfo*>::iterator iter = m_vecPrivateChatDlg.begin();
        for (; iter != m_vecPrivateChatDlg.end(); iter++)
        {
            PrivateChatDlgInfo* p = *iter;
            delete p;
        }
        m_vecPrivateChatDlg.clear();
        
        if (m_pEmotionDlg)
        {
            DestroyWindow(m_pEmotionDlg->GetHWND());
            SAFE_DELETE_Ixxx(m_pEmotionDlg);
        }

    }

    return 0;
}


PrivateChatDlgInfo*  CPrivateChatUI::FindDlg(const TCHAR* szUserId)
{
    if (NULL == szUserId)
        return NULL;

    vector<PrivateChatDlgInfo*>::iterator iter = m_vecPrivateChatDlg.begin();
    for (; iter != m_vecPrivateChatDlg.end(); iter++)
    {
        PrivateChatDlgInfo* pInfo = *iter;
        if (0 == _tcscmp(pInfo->m_strUserId.c_str(), szUserId))
        {
            return pInfo;
        }
    }

    return NULL;
}

PrivateChatDlgInfo*  CPrivateChatUI::FindDlg2(CChatDlg* pDlg)
{
    if (NULL == pDlg)
        return NULL;

    vector<PrivateChatDlgInfo*>::iterator iter = m_vecPrivateChatDlg.begin();
    for (; iter != m_vecPrivateChatDlg.end(); iter++)
    {
        PrivateChatDlgInfo* pInfo = *iter;
        if (pInfo->m_pChatDlg == pDlg)
        {
            return pInfo;
        }
    }

    return NULL;
}

void  CPrivateChatUI::DelayDestroyDlg(CChatDlg* p)
{
    IM::GetFramework()->PostTask(
        IM::CreateTask(
            this,
            PRIVATECHAT_UI_CMD_DESTROYPRIVATECHATDLG,
            (WPARAM)p, 0));
}

void  CPrivateChatUI::DestroyDlg(CChatDlg* p)
{
    if (!p)
        return;
    
    vector<PrivateChatDlgInfo*>::iterator iter = m_vecPrivateChatDlg.begin();
    for (; iter != m_vecPrivateChatDlg.end(); iter++)
    {
        PrivateChatDlgInfo* pInfo = *iter;
        if (pInfo->m_pChatDlg == p)
        {
            DestroyWindow(p->GetHWND());
            delete pInfo;

            m_vecPrivateChatDlg.erase(iter);
            return;
        }
    }
}

long  CPrivateChatUI::OnOpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* pTask)
{
    PrivateChatDlgInfo*  pInfo = FindDlg(pTask->m_szUserId);
    if (pInfo)
    {
        if (!pInfo->m_pChatDlg)
        {
            UIASSERT(0);
            SAFE_RELEASE(pTask);
            return 0;
        }

        if (pTask->m_lOpenMode == IM::SENDTEXT)
        {
            
        }
        pInfo->m_pChatDlg->ShowWindow();
        SAFE_RELEASE(pTask);
        return 0;
    }

    pInfo = new PrivateChatDlgInfo;
    pInfo->m_strUserId = pTask->m_szUserId;

    CChatDlg::CreateInstance(IM::GetUIApplication(), &pInfo->m_pChatDlg);
    pInfo->m_pChatDlg->SetPrivateChatUI(this);
    pInfo->m_pChatDlg->Create(IM::GetUIApplication(), _T("chatdlg"));

    if (NULL == m_pSkinUI)
    {
        m_pSkinUI = (IM::ISkinUI*)IM::GetFramework()->GetUI(MODULE_ID_SKIN);
    }
    if (m_pSkinUI)
    {
        if (NULL == pInfo->m_pChatDlg->GetMaterialRender())
        {
            pInfo->m_pChatDlg->SetSkin(m_pSkinUI->GetShareSkinRender());
        }
    }

    if (pTask->m_lOpenMode == IM::SENDTEXT)
    {
    }

    pInfo->m_pChatDlg->ShowWindow();

    m_vecPrivateChatDlg.push_back(pInfo);

    SAFE_RELEASE(pTask);
    return 0;
}

void  CPrivateChatUI::ShowEmotionDlg( HWND hWnd, POINT pt)
{
    if (!m_pEmotionDlg)
    {
        EmotionDlg::CreateInstance(IM::GetUIApplication(), &m_pEmotionDlg);
        m_pEmotionDlg->Create(IM::GetUIApplication(), _T("emotiondlg"));
        m_pEmotionDlg->SetPrivateChatUI(this);
    }
    m_pEmotionDlg->SetParam(hWnd);
    m_pEmotionDlg->Show(pt);
}
