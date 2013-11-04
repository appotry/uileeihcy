#include "stdafx.h"
#include "inertiascroll.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

InertiaScroll::InertiaScroll()
{
    m_pCallback = NULL;
    m_pUIApplication = NULL;

    m_bEnable = true;
    m_bMouseWheelUpDown = true;
    m_pAnimateStoryboard = NULL;

    m_nScroll_S = 0;
    m_nScroll_A = 0;
    m_nScroll_V = 0;
    m_nOldFps = 0;
}
InertiaScroll::~InertiaScroll()
{
    StopInertia();
}

void  InertiaScroll::SetUIApplication(IUIApplication* p)
{
    m_pUIApplication = p;
}
void  InertiaScroll::SetCallback(IInertiaScrollCallback* pCallback)
{
    m_pCallback = pCallback;
}

bool  InertiaScroll::IsEnable()
{
    return m_bEnable;
}
void  InertiaScroll::SetEnable(bool b)
{
    m_bEnable = b;
}

bool  InertiaScroll::IsScrolling()
{
    return m_pAnimateStoryboard == NULL ? false:true;
}

void  InertiaScroll::AddPower(int zDelta)
{
    if (0 == zDelta) 
        return;

    bool bUpDown = zDelta < 0;
    if (m_bMouseWheelUpDown != bUpDown && IsScrolling())
    {
        StopInertia();
    }

    m_bMouseWheelUpDown = bUpDown;
    zDelta = abs(zDelta);

    int nMouseWheelDelta = zDelta/WHEEL_DELTA;
    m_nScroll_S += nMouseWheelDelta * SCROLL_POWER_EACH_MOUSEWHEEL;
    m_nScroll_V = (float)SCROLL_INERTIA_SPEED_V0; // 重置速度
    Calc_A();
    
    if (NULL == m_pAnimateStoryboard && m_pUIApplication)
    {
        IAnimateManager* pAnimateMgr = m_pUIApplication->GetAnimateMgr();
        m_pAnimateStoryboard = pAnimateMgr->CreateStoryboard();
        m_pAnimateStoryboard->SetNotifyObj(this->GetIMessage());
        INoneTimeline* pTimeline = (INoneTimeline*)m_pAnimateStoryboard->CreateTimeline(TLV_NONE, 0, 0, NULL);
        m_pAnimateStoryboard->Begin();

        m_nOldFps = pAnimateMgr->SetFps(66);
        if (m_pCallback)
        {
            m_pCallback->InertiaScroll_Start();
        }
    }
}

// 计算加速度
void  InertiaScroll::Calc_A()
{
    if (m_nScroll_S > SCROLL_POWER_KEEP_SPEED)
    {
        m_nScroll_A = 0;
    }
    else
    {
        // 重新计算当前加速度  2as = v^2;
        if (0 == m_nScroll_S)
            m_nScroll_A = 0;
        else
            m_nScroll_A = (float)(m_nScroll_V * m_nScroll_V) / (float)(m_nScroll_S << 1);
    }
}
void  InertiaScroll::StopInertia()
{
    if (!IsScrolling())
        return;

    m_nScroll_S = 0;
    m_nScroll_A = 0;
    m_nScroll_V = 0;
    m_bMouseWheelUpDown = true;

    if (m_pUIApplication)
    {
        IAnimateManager* pAnimateMgr = m_pUIApplication->GetAnimateMgr();
        pAnimateMgr->SetFps(m_nOldFps);
        m_nOldFps = 0;

        pAnimateMgr->RemoveStoryboard(m_pAnimateStoryboard);
        m_pAnimateStoryboard = NULL;
    }

    if (m_pCallback)
    {
        m_pCallback->InertiaScroll_Stop();
    }
}


void  InertiaScroll::OnAnimateTick(int nCount, IStoryboard** ppArray)
{
    UIASSERT(nCount == 1);
    OnTick();  

//     int nTick = GetTickCount();
//     TCHAR szText[24] = _T("");
//     _stprintf(szText, _T("%d \r\n"), nTick);
//     ::OutputDebugString(szText);

}

void  InertiaScroll::OnTick()
{
    int nScrollNow = (int)m_nScroll_V;
    m_nScroll_S -= (int)m_nScroll_V;
    m_nScroll_V -= m_nScroll_A;

    if (0 == m_nScroll_A)
        Calc_A();

#ifdef _DEBUG
//     TCHAR szText[64] = _T("");
//     _stprintf(szText, _T("S: %3d, V: %f, A: %f\n"), m_nScroll_S, m_nScroll_V, m_nScroll_A);
//     ::OutputDebugString(szText);
#endif

    if (!m_bMouseWheelUpDown)
        nScrollNow = -nScrollNow;

    BOOL bNotifyOk = TRUE;
    if (m_pCallback)
    {
        bNotifyOk = m_pCallback->InertiaScroll_Scroll(nScrollNow);
    }
  
    if (!bNotifyOk        ||
        m_nScroll_S <= 0  ||
        m_nScroll_V <= 0  )
    {
        StopInertia();
    }
}
