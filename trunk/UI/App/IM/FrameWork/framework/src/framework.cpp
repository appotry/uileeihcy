#include "stdafx.h"
#include "framework.h"
#include "3rd/markup/markup.h"
#include "moduleitem.h"
#include "App\IM\include\login_inc.h"
#include "App\IM\FrameWork\framework\src\event.h"

CFramework::CFramework() : m_UIMessageWnd(this)
{
    m_pIFramework = new IM::IFramework(this);

    m_pUIApp = NULL;
    m_pSkinMgr = NULL;
    m_bQuitUIThreadRef = false;
    m_bQuitBusinessThreadRef = false;
    m_dwBizThreadId = 0;
    m_dwMainThreadId = 0;
    m_hBusinessThread = 0;
    m_hUIThread = 0;
    m_dwUIThreadId = 0;
    m_hWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ::InitializeCriticalSection(&m_csModules);
}
CFramework::~CFramework()
{
    MODULES_LOCK;

    vector<ModuleItem*>::iterator iter = m_vecModules.begin();
    for (; iter != m_vecModules.end(); iter++)
    {
        ModuleItem* pItem = *iter;
        SAFE_DELETE(pItem);
    }
    m_vecModules.clear();

    ::CloseHandle(m_hWaitEvent);
    ::DeleteCriticalSection(&m_csModules);

    SAFE_DELETE(m_pIFramework);
}


// 业务线程中
void  CFramework::Exit()
{
    m_bQuitBusinessThreadRef = true;
    m_bQuitUIThreadRef = true;

    if (m_dwUIThreadId)
        ::PostThreadMessage(m_dwUIThreadId, WM_NULL, 0, 0);
    if (m_dwBizThreadId)
        ::PostThreadMessage(m_dwBizThreadId, WM_NULL, 0, 0);
    ::PostThreadMessage(m_dwMainThreadId, WM_QUIT, 0, 0);
}

void  CFramework::PostTask(IM::ITask* pTask)
{
    if (NULL == pTask)
        return;

    IM::THREAD_TYPE eType = IM::Main;
    if (pTask->GetDest())
        eType = pTask->GetDest()->GetUiBizType();

    if (IM::Ui == eType)
    {
        if (pTask->GetTaskType() == IM::TASK_TYPE_PENDING)
        {
            if (IsUIThread())
            {
                OnTask(pTask, 0);
            }
            else
            {
                HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                ::PostMessage(m_UIMessageWnd.m_hWnd, UI_MSG_TASK, (WPARAM)pTask, (LPARAM)hEvent);
                WaitForSingleObject(hEvent, INFINITE);
            }
            return;
        }
        else
        {
            ::PostMessage(m_UIMessageWnd.m_hWnd, UI_MSG_TASK, (WPARAM)pTask, 0);
        }
    }
    else if (IM::Biz == eType)
    {
        if (pTask->GetTaskType() == IM::TASK_TYPE_PENDING)
        {
            if (IsBizThread())
            {
                OnTask(pTask, 0);
            }
            else
            {
                if (IsUIThread())
                {
                    UIASSERT(0 && _T("禁止UI线程向BIZ线程以PENDDING方式发送消息"));
                    return;
                }
                HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                ::PostThreadMessage(m_dwBizThreadId, BIZ_MSG_TASK, (WPARAM)pTask, (LPARAM)hEvent);
                WaitForSingleObject(hEvent, INFINITE);
            }
            return;
        }
        else
        {
            ::PostThreadMessage(m_dwBizThreadId, BIZ_MSG_TASK, (WPARAM)pTask, 0);
        }
    }
    else if (IM::Main == eType)
    {
        ::PostThreadMessage(m_dwMainThreadId, MAIN_MSG_TASK, (WPARAM)pTask, 0);
    }
}

void  CFramework::OnTask(IM::ITask* pTask, LPARAM lParam)
{
    if (pTask->GetDest())
    {
        pTask->GetDest()->ExecuteCommand(IM::BASE_CMD_ID_TASK, (WPARAM)pTask, 0);
    }
    
    switch (pTask->GetTaskType())
    {
    case IM::TASK_TYPE_PENDING:
        {
            // 解除阻塞
            if (lParam)
            {
                SAFE_RELEASE(pTask);  // 注：必须先释放pTask，再放开EVENT，否则Task所在模块可能先被卸载了，再release task就崩溃
                HANDLE hWaitEvent = (HANDLE)lParam;
                SetEvent(hWaitEvent);
            }
        }
        break;

    case IM::TASK_TYPE_CALLBACK:
        {
//             IM::ICallbackTask* pTask = static_cast<IM::ICallbackTask*>(pTask);
//             this->PostBizTask(IM::)
//             {
//                 UI_2_BIZ(pCallbackPackage->lId, pCallbackPackage->nCommand, pCallbackPackage->wParam, pCallbackPackage->lParam);
//             }
//             SAFE_DELETE(pCallbackPackage);
            SAFE_RELEASE(pTask);
        }
        break;

    default:
        {
            SAFE_RELEASE(pTask);
        }
        break;
    }
}


void  CFramework::PostFunction(IM::THREAD_TYPE eType, IM::fun_pkg* p, bool bWait)
{
    UIASSERT( p ) ;
    if (NULL == p)
        return;

    if (IM::Ui == eType)
    {
        if (bWait)
        {
            if (IsUIThread())
            {
                OnFunction(p, 0);
            }
            else
            {
                HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                ::PostMessage(m_UIMessageWnd.m_hWnd, UI_MSG_FUNCTION, (WPARAM)p, (LPARAM)hEvent);
                WaitForSingleObject(hEvent, INFINITE);
            }
            return;
        }
        else
        {
            ::PostMessage(m_UIMessageWnd.m_hWnd, UI_MSG_FUNCTION, (WPARAM)p, 0);
        }
    }
    else if (IM::Biz == eType)
    {
        if (bWait)
        {
            if (IsBizThread())
            {
                OnFunction(p, 0);
            }
            else
            {
                if (IsUIThread())
                {
                    UIASSERT(0 && _T("禁止UI线程向BIZ线程以PENDDING方式发送消息"));
                    return;
                }
                HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                ::PostThreadMessage(m_dwBizThreadId, BIZ_MSG_FUNCTION, (WPARAM)p, (LPARAM)hEvent);
                WaitForSingleObject(hEvent, INFINITE);
            }
            return;
        }
        else
        {
            ::PostThreadMessage(m_dwBizThreadId, BIZ_MSG_FUNCTION, (WPARAM)p, 0);
        }
    }   
    else if (IM::Main == eType)
    {
        ::PostThreadMessage(m_dwMainThreadId, MAIN_MSG_FUNCTION, (WPARAM)p, 0);
    }
}

void  CFramework::OnFunction(IM::fun_pkg* p, LPARAM lParam)
{
    p->Invoke();
    p->Release();

    // 解除阻塞
    if (lParam)
    {
        HANDLE hWaitEvent = (HANDLE)lParam;
        SetEvent(hWaitEvent);
    }
}

