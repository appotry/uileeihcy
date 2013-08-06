#ifndef IGIFIMAGE_H_C186D14A_0339_4606_A0FA_FC39C9732836
#define IGIFIMAGE_H_C186D14A_0339_4606_A0FA_FC39C9732836

namespace UI
{
interface  IGifImageRender;

// Gif动画绘制的方式
enum Gif_Timer_Notify_Type
{
    Gif_Timer_Notify_Direct_Hwnd,
    Gif_Timer_Notify_Post_Thread_Msg,
};
struct Gif_Timer_Notify
{
//     Gif_Timer_Notify();
//     Gif_Timer_Notify(HWND hWnd, int x, int y);
//     Gif_Timer_Notify(IMessage* pNotifyObj, UINT nTimerID, LPARAM lParam=NULL);
//
//     Gif_Timer_Notify::Gif_Timer_Notify()
//     {
//         eType = Gif_Timer_Notify_Direct_Hwnd;
//         notify_hwnd.hWnd = NULL;
//         notify_hwnd.hDC = NULL;
//         notify_hwnd.x = 0;
//         notify_hwnd.y = 0;
//     }
//     Gif_Timer_Notify::Gif_Timer_Notify(HWND hWnd, int x, int y)
//     {
//         eType = Gif_Timer_Notify_Direct_Hwnd;
//         notify_hwnd.hWnd = hWnd;
//         notify_hwnd.hDC = NULL;
//         notify_hwnd.x = x;
//         notify_hwnd.y = y;
//     }
//     Gif_Timer_Notify::Gif_Timer_Notify(IMessage* pNotifyObj, UINT nTimerID, LPARAM lParam)
//     {
//         eType = Gif_Timer_Notify_Post_Thread_Msg;
//         notify_ui_msg.nTimerID = nTimerID;
//         notify_ui_msg.pNotifyMsgObj = pNotifyObj;
//         notify_ui_msg.lParam = lParam;
//     }

    Gif_Timer_Notify_Type  eType;
    union
    {
        struct  // 直接显示在窗口的某个位置(未考虑裁剪)
        {
            HWND   hWnd;
            HDC    hDC;
            int    x;
            int    y;
        }notify_hwnd;

        struct  // 发送消息给界面线程，通知对象自己绘制
        {
            IMessage*  pNotifyMsgObj;
            UINT       nTimerID;
            LPARAM     lParam;

        }notify_ui_msg;
    };
};

interface  IGifImage
{
    virtual bool  Load(const TCHAR* szPath, IMapAttribute* pMapAttrib=NULL) = 0;
    virtual bool  Destroy() = 0;

    virtual IGifImageRender*  AddRender(Gif_Timer_Notify* pNotify, int* pIndex = NULL) = 0;
    virtual bool  ModifyRender(Gif_Timer_Notify* pNotify, int nIndex=-1) = 0;
    virtual bool  DeleteRender(int nIndex=-1) = 0;
};

//
//	GIF动画状态
//	
enum GIF_DRAW_STATUS
{
    GIF_DRAW_STATUS_START,
    GIF_DRAW_STATUS_STOP,
    GIF_DRAW_STATUS_PAUSE
};

interface  IGifImageRender
{
    virtual void  Release() = 0;

    virtual void  Start() = 0;
    virtual void  Pause() = 0;
    virtual void  Stop() = 0;

    virtual int  GetWidth() = 0;
    virtual int  GetHeight() = 0;
    virtual void  OnPaint(HDC hDC, int x, int y) = 0;
    virtual GIF_DRAW_STATUS GetStatus() = 0;
};

}

#endif // IGIFIMAGE_H_C186D14A_0339_4606_A0FA_FC39C9732836