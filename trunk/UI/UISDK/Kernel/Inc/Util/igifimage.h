#ifndef IGIFIMAGE_H_C186D14A_0339_4606_A0FA_FC39C9732836
#define IGIFIMAGE_H_C186D14A_0339_4606_A0FA_FC39C9732836

namespace UI
{
interface  IGifImageRender;

// Gif�������Ƶķ�ʽ
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
        struct  // ֱ����ʾ�ڴ��ڵ�ĳ��λ��(δ���ǲü�)
        {
            HWND   hWnd;
            HDC    hDC;
            int    x;
            int    y;
        }notify_hwnd;

        struct  // ������Ϣ�������̣߳�֪ͨ�����Լ�����
        {
            IMessage*  pNotifyMsgObj;
            UINT       nTimerID;
            LPARAM     lParam;

        }notify_ui_msg;
    };
};

class GifImageBase;
class UISDKAPI IGifImage
{
public:
    IGifImage(GifImageBase* p);
    IGifImage(IUIApplication* pUIApp);
    ~IGifImage();

    void  CreateGifImpl(IUIApplication* pUIApp);
    void  CreatePnglistGifImpl();
    GifImageBase*  GetImpl();
    void  SetImpl(GifImageBase* p);

public:
    bool  Load(const TCHAR* szPath, IMapAttribute* pMapAttrib=NULL);
    bool  Destroy();

    IGifImageRender*  AddRender(Gif_Timer_Notify* pNotify, int* pIndex = NULL);
    bool  ModifyRender(Gif_Timer_Notify* pNotify, int nIndex=-1);
    bool  DeleteRender(int nIndex=-1);

   bool   SetTransparentColor(COLORREF colTransparent = GetSysColor(COLOR_BTNFACE));
private:
    GifImageBase*  m_pImpl;
    bool  m_bCreateGifImageImpl;
};

//
//	GIF����״̬
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
    virtual void  OnAlphaPaint(HDC hDC, int x, int y) = 0;
    virtual GIF_DRAW_STATUS GetStatus() = 0;
};

}

#endif // IGIFIMAGE_H_C186D14A_0339_4606_A0FA_FC39C9732836