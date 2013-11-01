
#pragma once
#include "uimessagewnd.h"
#include "App\IM\include\framework_inc.h"
#include "App/IM/FrameWork/framework/src/pluginmgr.h"

//  遗留问题:
//  1. 如何卸载一个模块
//     m_pFrameWork->ReleaseModule(MODULE_ID_LOGIN);  -- TODO: 如何卸载自己，还能保证本模块中所有的task都被释放完了
//  2. 手动调用FreeLibrary会导致显示内存泄露数据时崩溃
//
//  3. Register/Unreisger UI/Biz 放在哪个线程好，需要加锁吗
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
//    long  LoadModuleByTime_(const TCHAR* szTime);   // 跨线程版本
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
    //  主线程
    //////////////////////////////////////////////////////////////////////////
    DWORD  m_dwMainThreadId;

    //////////////////////////////////////////////////////////////////////////
    //   界面线程
    //////////////////////////////////////////////////////////////////////////

    UI::IUIApplication*  m_pUIApp;
    UI::ISkinManager*    m_pSkinMgr;

    CUIMessageWnd  m_UIMessageWnd;
    bool    m_bQuitUIThreadRef;   // 用于退出UI线程
    HANDLE  m_hUIThread;
    DWORD   m_dwUIThreadId;

    vector<UIEvent*>  m_vecUIEvent;


    //////////////////////////////////////////////////////////////////////////
    //  Biz线程
    //////////////////////////////////////////////////////////////////////////
    bool    m_bQuitBusinessThreadRef;   // 用于退出UI线程
    HANDLE  m_hBusinessThread;
    DWORD   m_dwBizThreadId;

    //////////////////////////////////////////////////////////////////////////
    //  模块列表
    //////////////////////////////////////////////////////////////////////////
    vector<ModuleItem*>  m_vecModules;
    CRITICAL_SECTION     m_csModules;
    HANDLE  m_hWaitEvent;

    //////////////////////////////////////////////////////////////////////////
    //  插件模块
    //////////////////////////////////////////////////////////////////////////
    CPluginManager  m_mgrPlugin;

    //////////////////////////////////////////////////////////////////////////
    //  事件列表
    //////////////////////////////////////////////////////////////////////////


    IM::IFramework*  m_pIFramework;
};
extern CFramework   g_framework;