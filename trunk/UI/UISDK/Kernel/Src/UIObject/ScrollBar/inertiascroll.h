#pragma once
#include "UISDK/Kernel/Src/Base/Message/message.h"

//////////////////////////////////////////////////////////////////////////
//              ���Թ���
//
//  ԭ��
//   1. ��m_nScroll_V0��Ϊ���ٶ��������˶��������ٶȵ���0ֹͣ
//   2. ÿһ��MOUSEWHEELʱ�����ٶ�����Ϊm_nScroll_V0
//   3. ������������Ϊ·��
//   4. ÿһ��MOUSEWHEELʱ���ۼ�����Ҫ��·��
//   5. ʱ�䲻��Ҫ��2as=v^2��û��ʱ�����
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{

#define SCROLL_POWER_EACH_MOUSEWHEEL  120    // ÿһ��WHEEL_DELTA��ƽ������λ��
#define SCROLL_POWER_KEEP_SPEED       360    // ������֮������ʱ�����������˶���С��ʱ�������˶�
#define SCROLL_INERTIA_SPEED_V0       30     // �������ٶ�

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
    bool    m_bMouseWheelUpDown;   // �������¹��������Ǵ�������
    IStoryboard*  m_pAnimateStoryboard;
    int     m_nOldFps;      // ƽ��������֡��̫�ߣ���Ҫ��ʱ�޸ġ������޸�֮ǰ��ֵ���ڻ�ԭ

    int     m_nScroll_S;    // ÿһ��MOUSEWHEEL��SCROLL_HEIGHT_EACH_MOUSEWHEEL��ÿ��tick��m_nScroll_V
    float   m_nScroll_A;    // ͨ�� 2as = V0^2 �������
    float   m_nScroll_V;    // ͨ�� Vt = V0 + at �������
};

}