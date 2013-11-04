#pragma once
#include "UISDK/Kernel/Src/Base/Message/message.h"

//////////////////////////////////////////////////////////////////////////
//              惯性滚动
//
//  原理：
//   1. 以m_nScroll_V0作为初速度做减速运动，最终速度到达0停止
//   2. 每一次MOUSEWHEEL时，将速度重置为m_nScroll_V0
//   3. 将滚动能量作为路程
//   4. 每一次MOUSEWHEEL时，累加所需要的路程
//   5. 时间不需要，2as=v^2，没有时间参数
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{

#define SCROLL_POWER_EACH_MOUSEWHEEL  120    // 每一个WHEEL_DELTA的平滑滚动位移
#define SCROLL_POWER_KEEP_SPEED       360    // 当大于之个能量时，保持匀速运动，小于时做减速运动
#define SCROLL_INERTIA_SPEED_V0       30     // 滚动初速度

interface  IStoryboard;

interface IInertiaScrollCallback
{
    virtual  void InertiaScroll_Start() = 0;
    virtual  void InertiaScroll_Stop() = 0;
    virtual  BOOL InertiaScroll_Scroll(int nDeltaPos) = 0;
};

class InertiaScroll : public Message
{
public:
    InertiaScroll();
    ~InertiaScroll();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
    UI_END_MSG_MAP

public:
    void  SetUIApplication(IUIApplication* p);
    void  SetCallback(IInertiaScrollCallback* pCallback);
    bool  IsEnable();
    void  SetEnable(bool b);
    bool  IsScrolling();

    void  OnAnimateTick(int nCount, IStoryboard** ppArray);
    void  OnTick();
    void  AddPower(int zDelta);
    void  StopInertia();
    void  Calc_A();

private:
    IUIApplication*  m_pUIApplication;
    IInertiaScrollCallback*  m_pCallback;

    bool    m_bEnable;
    bool    m_bMouseWheelUpDown;   // 从下至下滚动，还是从下至上
    IStoryboard*  m_pAnimateStoryboard;
    int     m_nOldFps;      // 平滑滚动的帧数太高，需要临时修改。保存修改之前的值用于还原

    int     m_nScroll_S;    // 每一次MOUSEWHEEL加SCROLL_HEIGHT_EACH_MOUSEWHEEL，每次tick减m_nScroll_V
    float   m_nScroll_A;    // 通过 2as = V0^2 计算而来
    float   m_nScroll_V;    // 通过 Vt = V0 + at 计算而来
};

}