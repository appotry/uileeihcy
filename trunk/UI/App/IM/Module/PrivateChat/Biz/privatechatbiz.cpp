#include "stdafx.h"
#include "privatechatbiz.h"
#include "App\IM\include\task.h"
#include "../UI/privatechatui.h"

CPrivateChatBiz::CPrivateChatBiz()
{
    m_pFrameWork = IM::GetFramework();
}
CPrivateChatBiz::~CPrivateChatBiz()
{
}

long  CPrivateChatBiz::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (nCommand == IM::BASE_CMD_ID_EXIT)
    {
//         IM::ITask* pTask = (IM::ITask*)wParam;
//         switch (pTask->GetCommandId())
//         {
//        
//         }
    }

    return 0;
}


long  CPrivateChatBiz::OpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* p)
{
    p->AddRef();
    IM::GetFramework()->PostFunction(IM::Ui, IM::create_mem_fun1_pkg(&CPrivateChatUI::OnOpenPrivateChatDlg, g_pPrivateChatUI, p), false);

    SAFE_RELEASE(p);
    return 0;
}
