#ifndef LOGIN_INC_H_00B33C7D_06A4_4e34_8646_B88222D6D849
#define LOGIN_INC_H_00B33C7D_06A4_4e34_8646_B88222D6D849
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\task.h"

#ifdef LOGIN_EXPORTS
#define IMLOGINAPI __declspec(dllexport)
#else
#define IMLOGINAPI __declspec(dllimport)
#endif

namespace IM
{
    
struct ILoginBiz : public IBiz
{
    virtual long  ShowDialog(long lType) = 0;
    virtual long  HideDialog(long lType) = 0;
};

// UI COMMAND
#define LOGIN_UI_CMD_HIDE_AND_DESTROY   139122142   // 主窗口显示后，发送给登录窗口关闭

// BIZ Command


}

#endif // LOGIN_INC_H_00B33C7D_06A4_4e34_8646_B88222D6D849