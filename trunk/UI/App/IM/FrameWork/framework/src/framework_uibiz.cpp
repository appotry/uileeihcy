#include "stdafx.h"
#include "framework.h"
#include "moduleitem.h"
#include "App\IM\FrameWork\framework\src\event.h"

DWORD WINAPI _UIThread(LPVOID lpParameter)
{
    CFramework* p = (CFramework*)lpParameter;
    if (!p->InitUI())
        return 0;

    p->UIThread();
    p->UninitUI();
    return 0;
}

DWORD WINAPI _BusinessThread(LPVOID lpParameter)
{
    CFramework* p = (CFramework*)lpParameter;
    p->InitBiz();
    p->BizThread();
    p->UninitBiz();
    return 0;
}

DWORD WINAPI _NetThread(LPVOID lpParameter)
{
    CFramework* p = (CFramework*)lpParameter;
    p->InitNet();
    p->NetThread();
    p->UninitNet();
    return 0;
}



void  CFramework::UIThread()
{
    m_pUIApp->MsgHandleLoop(&m_bQuitUIThreadRef);
}

bool  CFramework::InitUI()
{
    CreateUIApplicationInstance(&m_pUIApp);
    m_pSkinMgr = m_pUIApp->GetSkinMgr();

    LoadLog();

    TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);

    String str = szPath;
    str.append(_T("skin"));
    m_pUIApp->SetSkinDirection(str.c_str());
    if (FAILED(m_pUIApp->LoadSkin(_T("2012"))))
    {
        SetEvent(m_hWaitEvent);
        return false;
    }

    UI_Ctrl_RegisterUIObject(m_pUIApp);   // ע��ؼ�
    IM_Ctrl_RegisterUIObject(m_pUIApp);   // ע��ؼ�

    m_UIMessageWnd.Create(HWND_MESSAGE);

    // �����߳���Ϣ����
    //
    // PostThreadMessage:
    // The function fails if the specified thread does not have a message queue. 
    // The system creates a thread's message queue when the thread makes 
    // its first call to one of the User or GDI functions
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    SetEvent(m_hWaitEvent);
    return true;
}

void  CFramework::LoadLog()
{
    TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
    String strLogPath = szPath;

    // ������־
    strLogPath.append(_T("im.xml"));
    m_pUIApp->SetLog(NULL, (BSTR)strLogPath.c_str());
    //     m_pUIApp->GetLog(&g_pLog);
    //     if (g_pLog)
    //     {
    //         ILogItem*  pLogItem = NULL;
    //         m_pUIApp->GetLogItemByName(_T("IM"), &pLogItem);
    //         if (pLogItem)
    //         {
    //             pLogItem->GetCookie(&g_lLogCookie);
    //         }
    //     }
}

void CFramework::UninitUI()
{
    {
        MODULES_LOCK;
        vector<ModuleItem*>::iterator iter = m_vecModules.begin();
        for (; iter != m_vecModules.end(); iter++)
        {
            ModuleItem* pItem = *iter;
            if (NULL == pItem || NULL == pItem->m_pUI)
                continue;

            pItem->m_pUI->ExecuteCommand(IM::BASE_CMD_ID_EXIT, 0, 0);
        }
    }
    {
        vector<UIEvent*>::iterator iter = m_vecUIEvent.begin();
        for (; iter != m_vecUIEvent.end(); iter++)
        {
            UIEvent* pItem = *iter;
            SAFE_DELETE(pItem);
        }
        m_vecUIEvent.clear();
    }

    m_UIMessageWnd.DestroyWindow();
    m_UIMessageWnd.m_hWnd = NULL;

    SAFE_RELEASE(m_pUIApp);
    m_hUIThread = NULL;
    m_dwUIThreadId = 0;
}

void  CFramework::BizThread()
{
    MSG  msg;
    while (false == m_bQuitBusinessThreadRef)
    {
        GetMessage(&msg, 0, 0, 0);

        switch (msg.message)
        {
        case BIZ_MSG_TASK:
            {
                IM::ITask* pTask = (IM::ITask*)msg.wParam;
                OnTask(pTask, msg.lParam);
            }
            break;

        case BIZ_MSG_FUNCTION:
            {
                IM::fun_pkg* p = (IM::fun_pkg*)msg.wParam;
                OnFunction(p, msg.lParam);
            }
            break;

        case WM_QUIT:
            break;

        default:
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
            break;
        }
    }
}
void  CFramework::InitBiz()
{
    // �����߳���Ϣ����
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    SetEvent(m_hWaitEvent);
}

void  CFramework::UninitBiz()
{
    {
        MODULES_LOCK;
        vector<ModuleItem*>::iterator iter = m_vecModules.begin();
        for (; iter != m_vecModules.end(); iter++)
        {
            ModuleItem* pItem = *iter;
            if (NULL == pItem || NULL == pItem->m_pBiz)
                continue;

            pItem->m_pBiz->ExecuteCommand(IM::BASE_CMD_ID_EXIT, 0, 0);
        }
    }

    m_dwBizThreadId = 0;
    m_hBusinessThread = NULL;
}

void  CFramework::NetThread()
{
    // ����ģ������
}
void  CFramework::InitNet()
{
    // ��������ģ��

    // �����߳���Ϣ����
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
}
void  CFramework::UninitNet()
{
    // ж������ģ��
}
