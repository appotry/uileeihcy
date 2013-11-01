#include "stdafx.h"
#include "framework.h"
#include "3rd/markup/markup.h"
#include "moduleitem.h"
#include "App\IM\include\login_inc.h"
#include "App\IM\FrameWork\framework\src\event.h"

DWORD WINAPI _UIThread(LPVOID lpParameter);
DWORD WINAPI _BusinessThread(LPVOID lpParameter);
DWORD WINAPI _NetThread(LPVOID lpParameter);

void  CFramework::Run(long lThreadMode)
{
    m_dwMainThreadId = GetCurrentThreadId();

    if (!LoadFrameworkXml())
    {
        ::MessageBox(NULL,_T("LoadFrameworkXml Failed"), _T("Error"), MB_OK|MB_ICONWARNING);
        return;
    }

    if (!m_mgrPlugin.LoadPluginXml())
    {
        ::MessageBox(NULL,_T("LoadPluginXml Failed"), _T("Error"), MB_OK|MB_ICONWARNING);
        return;
    }

    m_hUIThread = ::CreateThread(NULL, 0, _UIThread, this, 0, &m_dwUIThreadId);
    WaitForSingleObject(m_hWaitEvent, INFINITE);
    m_hBusinessThread = ::CreateThread(NULL, 0, _BusinessThread, this, 0, &m_dwBizThreadId);
    WaitForSingleObject(m_hWaitEvent, INFINITE);
    ::CreateThread(NULL, 0, _NetThread, this, 0, 0);

    LoadModuleByTime(_T("startup"));

    MSG  msg;
    while (::GetMessage(&msg, 0, 0, 0))  // WM_QUIT ÍË³öÑ­»·
    {
        if (MAIN_MSG_FUNCTION == msg.message)
        {
            IM::fun_pkg* p = (IM::fun_pkg*)msg.wParam;
            p->Invoke();
            p->Release();
        }
        else if (MAIN_MSG_TASK == msg.message)
        {
            IM::ITask* pTask = (IM::ITask*)msg.wParam;
            switch (pTask->GetCommandId())
            {
            case IM::MAIN_CMD_ID_LOGON:
                {
                    OnLogon();
                }
                break;
            }
            SAFE_RELEASE(pTask);
        }
        else
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    if (m_hUIThread)
    {
        WaitForSingleObject(m_hUIThread, 30000);
    }
    if (m_hBusinessThread)
    {
        WaitForSingleObject(m_hBusinessThread, 30000); 
    }
}


void  CFramework::OnLogon()
{
    LoadModuleByTime(_T("logon"));
    m_mgrPlugin.Load();
}

