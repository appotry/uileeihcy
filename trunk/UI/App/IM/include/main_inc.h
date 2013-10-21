#ifndef MAIN_INC_H_E19B36AE_0E54_4051_841B_C832423A6A28
#define MAIN_INC_H_E19B36AE_0E54_4051_841B_C832423A6A28
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\task.h"

#ifdef MAIN_EXPORTS
#define IMMAINAPI __declspec(dllexport)
#else
#define IMMAINAPI __declspec(dllimport)
#endif

namespace UI
{
    interface IButton;
}

namespace IM
{
    // struct IPluginBase
    // {
    //     virtual int   GetPluginType() = 0;        // 类型
    //     virtual const TCHAR*  GetName() = 0;      // mail
    // };

    struct IMainDlgToolbarPlugin
    {
        virtual const TCHAR*  GetName() = 0;      // mail
        virtual const TCHAR*  GetText() = 0;      // xxx邮箱
        virtual const TCHAR*  GetDesc() = 0;      // xxx邮箱
        virtual HBITMAP  GetIcon() = 0;           // 主窗口上的图标    
        virtual HBITMAP  GetIcon2() = 0;          // 设置窗口上的图标
        virtual void  OnCreate(UI::IButton* p) = 0;  // 创建通知
        virtual void  OnClick() = 0;
    };
    struct IMainDlgTabContentPlugin
    {

    };

    struct IMainBiz : public IBiz
    {
        virtual long  ShowDialog() = 0;
        virtual long  HideDialog() = 0;
    };

    struct IMainUI : public IUI
    {
        virtual long  AddToolbarPlugin(IMainDlgToolbarPlugin* pPlugin, int nSort=-1) = 0;
        virtual long  GetToolbarPluginCount() = 0;
        virtual IMainDlgToolbarPlugin*  GetToolbarPluginByIndex(int i, int* pnSort) = 0;
        virtual void  SetToolbarPluginSort(IMainDlgToolbarPlugin* pPlugin, int nSort) = 0;
        virtual void  ShowToolbarPluginBtn(IMainDlgToolbarPlugin* pPlugin, bool bShow) = 0;
    };
    // UI COMMAND

    // BIZ Command
}

#endif // MAIN_INC_H_E19B36AE_0E54_4051_841B_C832423A6A28