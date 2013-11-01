
#pragma once
#include "uimessagewnd.h"
#include "App\IM\include\framework_inc.h"
#include "App/IM/FrameWork/framework/src/pluginmgr.h"

//  ��������:
//  1. ���ж��һ��ģ��
//     m_pFrameWork->ReleaseModule(MODULE_ID_LOGIN);  -- TODO: ���ж���Լ������ܱ�֤��ģ�������е�task�����ͷ�����
//  2. �ֶ�����FreeLibrary�ᵼ����ʾ�ڴ�й¶����ʱ����
//
//  3. Register/Unreisger UI/Biz �����ĸ��̺߳ã���Ҫ������
//
//  4. IFramework/CFramework --> PImpl
//

namespace IM
{
    class Task;
}
class UIEvent;
class ModuleItem;
class PluginItem;

#define  BIZ_MSG_TASK       (WM_USER)
#define  BIZ_MSG_FUNCTION   (WM_USER+1)

#define  MAIN_MSG_FUNCTION  (WM_USER)
#define  MAIN_MSG_TASK      (WM_USER+1)

#define MODULES_LOCK  IM::CSLock __cslockModules(&m_csModules);

class CFramework
{
public:
    CFramework();
    ~CFramework();

    void  Run(long lThreadMode);
    void  Exit();
    void  LoadModuleByTime(const TCHAR* szTime);
//    long  LoadModuleByTime_(const TCHAR* szTime);   // ���̰߳汾
    void  LoadModuleByName(const TCHAR* szName, IM::IUI** ppUI, IM::IBiz** ppBiz);

    void  PostTask(IM::ITask*);
    void  PostFunction(IM::THREAD_TYPE eType, IM::fun_pkg* p, bool bWait);

    void  AddUIEvent(IM::EventType szName);
    void  RemoveUIEvent(IM::EventType szName);
    void  SubscribeUIEvent(IM::EventType szName, IM::IUI* pUI);
    void  UnsubscribeUIEvent(IM::EventType szName, IM::IUI* pUI);
    void  FireUIEvent(IM::EventType szName, LPARAM lParam);

    IM::IBiz*  GetBiz(const TCHAR* szName);
    IM::IUI*  GetUI(const TCHAR* szName);

public:
    void  UIThread();
    bool  InitUI();
    void  UninitUI();

    void  BizThread();
    void  InitBiz();
    void  UninitBiz();
    
    void  NetThread();
    void  InitNet();
    void  UninitNet();

    void  OnTask(IM::ITask* pTask, LPARAM lParam);
    void  OnFunction(IM::fun_pkg* p, LPARAM lParam);

    DWORD GetUIThreadId()    { return m_dwUIThreadId; }
    DWORD GetBizThreadId()   { return m_dwBizThreadId; }
    DWORD GetMainThreadId()  { return m_dwMainThreadId; }
    bool  IsUIThread()       { return m_dwUIThreadId == GetCurrentThreadId(); }
    bool  IsBizThread()      { return m_dwBizThreadId == GetCurrentThreadId(); }
    bool  IsMainThread()     { return m_dwMainThreadId == GetCurrentThreadId(); }

public:
    UI::IUIApplication*  GetUIApplication() { return m_pUIApp; }
    IM::IFramework*  GetIFramework() { return m_pIFramework; }

public:
    long  OnModuleBizLoad(IM::IBiz* pBiz, ModuleItem* pItem);
    long  OnModuleUiLoad(IM::IUI* pUI, ModuleItem* pItem);
    long  OnPluginBizLoad(IM::IBiz* pBiz, PluginItem* pItem);
    long  OnPluginUiLoad(IM::IUI* pUI, PluginItem* pItem);

protected:
    void  LoadLog();
    bool  LoadFrameworkXml();
    void  OnLogon();
   
    ModuleItem*  FindModule(const TCHAR* szName);
    UIEvent*  FindUIEvent(const TCHAR* szName);

public:

    //////////////////////////////////////////////////////////////////////////
    //  ���߳�
    //////////////////////////////////////////////////////////////////////////
    DWORD  m_dwMainThreadId;

    //////////////////////////////////////////////////////////////////////////
    //   �����߳�
    //////////////////////////////////////////////////////////////////////////

    UI::IUIApplication*  m_pUIApp;
    UI::ISkinManager*    m_pSkinMgr;

    CUIMessageWnd  m_UIMessageWnd;
    bool    m_bQuitUIThreadRef;   // �����˳�UI�߳�
    HANDLE  m_hUIThread;
    DWORD   m_dwUIThreadId;

    vector<UIEvent*>  m_vecUIEvent;


    //////////////////////////////////////////////////////////////////////////
    //  Biz�߳�
    //////////////////////////////////////////////////////////////////////////
    bool    m_bQuitBusinessThreadRef;   // �����˳�UI�߳�
    HANDLE  m_hBusinessThread;
    DWORD   m_dwBizThreadId;

    //////////////////////////////////////////////////////////////////////////
    //  ģ���б�
    //////////////////////////////////////////////////////////////////////////
    vector<ModuleItem*>  m_vecModules;
    CRITICAL_SECTION     m_csModules;
    HANDLE  m_hWaitEvent;

    //////////////////////////////////////////////////////////////////////////
    //  ���ģ��
    //////////////////////////////////////////////////////////////////////////
    CPluginManager  m_mgrPlugin;

    //////////////////////////////////////////////////////////////////////////
    //  �¼��б�
    //////////////////////////////////////////////////////////////////////////


    IM::IFramework*  m_pIFramework;
};
extern CFramework   g_framework;