#ifndef FRAMEWORK_H_8DF2EBA5_3AD5_4cd1_8E87_0AA8DFF52F93
#define FRAMEWORK_H_8DF2EBA5_3AD5_4cd1_8E87_0AA8DFF52F93
#include "task.h"
#include "function.h"

#ifdef FRAMEWORK_EXPORTS
#define IMFRAMEWORKAPI __declspec(dllexport)
#else
#define IMFRAMEWORKAPI __declspec(dllimport)
#endif

namespace UI
{
    struct IUIApplication;
}

class CFramework;;
namespace IM
{
struct IBiz;
struct IUI;
struct ITask;
enum THREAD_TYPE
{
    Ui,
    Biz,
    Main,
};
typedef const TCHAR* EventType;

//////////////////////////////////////////////////////////////////////////
//
// 接口定义
//
//////////////////////////////////////////////////////////////////////////

class IMFRAMEWORKAPI IFramework
{
public:
    IFramework(CFramework* p);

    void  Run(long lThreadMode);
    void  Exit();
    void  LoadModuleByTime(const TCHAR* szTime);
    void  LoadModuleByName(const TCHAR* szName, IM::IUI** ppUI, IM::IBiz** ppBiz);

    IBiz*  GetBiz(const TCHAR* szName);
    IUI*  GetUI(const TCHAR* szName);

    void  PostTask(ITask*);
    void  PostFunction(IM::THREAD_TYPE eType, IM::fun_pkg* p, bool bWait);

    void  AddUIEvent(EventType szName);
    void  RemoveUIEvent(EventType szName);
    void  SubscribeUIEvent(EventType szName, IM::IUI* pUI);
    void  UnsubscribeUIEvent(EventType szName, IM::IUI* pUI);
    void  FireUIEvent(EventType szName, LPARAM lParam);

    // TODO:
//     virtual void  ScheduleUITask(ITask*) = 0;
//     virtual void  ScheduleBizTask(ITask*) = 0;

private:
    CFramework*  m_pImpl;
};

struct IUiBizBase
{
    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam) = 0;
    virtual THREAD_TYPE  GetUiBizType() = 0;
};

struct IBiz : public IUiBizBase
{
    virtual THREAD_TYPE  GetUiBizType() { return Biz; }
};

struct IUI : public IUiBizBase
{
    virtual THREAD_TYPE  GetUiBizType() { return Ui; }
};

enum  PLUGIN_TYPE 
{
    MAINDLG_TOOLBAR,
    MAINDLG_TAB_CONTENT,
};

//////////////////////////////////////////////////////////////////////////
//
// 导出函数
//
//////////////////////////////////////////////////////////////////////////

typedef IBiz*  (*LPFUNCLOADBIZ)(const TCHAR* szName, IFramework* pFrameWork);
typedef IUI* (*LPFUNCLOADUI)(const TCHAR* szName, IFramework* pFrameWork);

extern "C"
{
    IMFRAMEWORKAPI IFramework*  GetFramework();
    IMFRAMEWORKAPI UI::IUIApplication*  GetUIApplication();
}


//////////////////////////////////////////////////////////////////////////
//
// Module Id Define
//
//////////////////////////////////////////////////////////////////////////

#define MODULE_ID_LOGIN         _T("login")
#define MODULE_ID_MAIN          _T("main")
#define MODULE_ID_CONFIG        _T("config")
#define MODULE_ID_PRIVATECHAT   _T("privatechat")
#define MODULE_ID_SKIN          _T("skin")
#define MODULE_ID_TRAYICON      _T("trayicon")



enum UI_BIZ_CMD_ID
{
    BASE_CMD_ID_TASK = 0x1000,
    BASE_CMD_ID_EXIT = 0x1001,
    BASE_CMD_ID_INIT = 0x1002,
    BASE_CMD_ID_EVENT = 0x1003,

    BIZ_CMD_ID_BASE = 0x2000,

    UI_CMD_ID_BASE = 0x3000,

    MAIN_CMD_ID_BASE = 0x4000,
    MAIN_CMD_ID_LOGON = 0x4001,   // 登录成功，加载其它组件
};


//////////////////////////////////////////////////////////////////////////
//
//  Util
//
//////////////////////////////////////////////////////////////////////////
class CSLock
{
public:
    CSLock(CRITICAL_SECTION* p)
    {
        m_p = p;
        ::EnterCriticalSection(m_p);
    }
    ~CSLock()
    {
        ::LeaveCriticalSection(m_p);
    }
private:
    CRITICAL_SECTION* m_p;
};

}
#endif // FRAMEWORK_H_8DF2EBA5_3AD5_4cd1_8E87_0AA8DFF52F93