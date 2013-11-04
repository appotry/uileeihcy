#include "stdafx.h"
#include "intanimatetimeline.h"


IntLinearMove::IntLinearMove()
{
    m_from = m_to = m_t = 0;
    m_v = 0.0f;
    m_eFinishtType = CHECK_FINISH_BY_S;
}

IntLinearMove::~IntLinearMove()
{

}

void IntLinearMove::SetParam1(int from, int to, int t)
{
    m_from = from;
    m_to = to;
    m_t = t;

    UIASSERT(m_t != 0);

    if (0 == m_t)
        m_t = 1;

    m_v = (m_to - m_from)/(float)m_t;
    m_eFinishtType = CHECK_FINISH_BY_T;
}
void IntLinearMove::SetParam2(int from, int to, float v)
{
    m_from = from;
    m_to = to;
    m_v = v;

    UIASSERT(m_from != m_to);
    UIASSERT(v != 0.0f);

    if (0.0f == v)
        m_v = 1.0f;

    m_t = (int)((m_to - m_from)/(float)m_v + 0.5);
    m_eFinishtType = CHECK_FINISH_BY_S;
}
void IntLinearMove::SetParam3(int from, float v, int t)
{
    m_from = from;
    m_to = 0;
    m_t = t;
    m_v = v;

    UIASSERT(v != 0.0f);
    UIASSERT(m_t != 0);

    m_to = m_from + (int)(m_t*m_v + 0.5);
    m_eFinishtType = CHECK_FINISH_BY_T;
}

bool IntLinearMove::OnTick(int t, int* pCurrentValue)
{
// #ifdef _DEBUG
//     {
//         static int s_tprev = 0;
//         TCHAR szText[64] = _T("");
//         _stprintf(szText, _T("%d\n"), t-s_tprev);
//         s_tprev = t;
//         ::OutputDebugString(szText);
//     }
// #endif

    bool bFinish = false;
    if (CHECK_FINISH_BY_T == m_eFinishtType)
    {
        if (t >= m_t)
        {
            bFinish = true;
            *pCurrentValue = m_to;
        }
        else
        {
            *pCurrentValue = m_from + (int)(m_v * t + 0.5);
        }
    }
    else if (CHECK_FINISH_BY_S == m_eFinishtType)
    {
        *pCurrentValue = m_from + (int)(m_v * t + 0.5);
        if ((m_to > m_from && *pCurrentValue >= m_to) ||
            (m_to < m_from && *pCurrentValue <= m_to))
        {
            bFinish = true;
            *pCurrentValue = m_to;
        }
    }

    return bFinish;
}

//////////////////////////////////////////////////////////////////////////

IntAccelerateMove::IntAccelerateMove()
{
    m_from = m_to = m_t = 0;
    m_vo = m_a = 0.0f;
    // m_vt = 0.0f;
    m_eFinishtType = CHECK_FINISH_BY_T;
}
IntAccelerateMove::~IntAccelerateMove()
{

}

void IntAccelerateMove::SetParam1(int from, int to, int t, float Vo)
{
    m_from = from;
    m_to = to;
    m_t = t;
    m_vo = Vo;
    m_eFinishtType = CHECK_FINISH_BY_T;

    UIASSERT (0 != t);
    if (0 == t)
        t = 1;

    int s = m_to - m_from;
    m_a = (float)((s<<1) - ((t<<1)*Vo)) / (t*t);
}
// void IntAccelerateMove::SetParam2(int from, int t, int a, int Vo, int Vt)
// {
//     m_from = from;
//     m_t = t;
//     m_vo = Vo;
//     m_vt = Vt;
//     m_a = a;
// 
//     float s = Vo * t + 0.5 * a * t * 5;
//     m_to = m_from + s;
// 
//     m_eFinishtType = CHECK_FINISH_BY_T;
// }

bool IntAccelerateMove::OnTick(int t, int* pCurrentValue)
{
    bool bFinish = false;
    if (CHECK_FINISH_BY_T == m_eFinishtType)
    {
        if (t >= m_t)
        {
            bFinish = true;
            *pCurrentValue = m_to;
        }
        else
        {
            *pCurrentValue = m_from + (int)(m_vo * t + 0.5f * m_a * t * t);
        }
    }
    else if (CHECK_FINISH_BY_S == m_eFinishtType)
    {
        *pCurrentValue = m_from + (int)(m_vo * t + 0.5f * m_a * t * t);
        if (*pCurrentValue >= m_to)
        {
            bFinish = true;
            *pCurrentValue = m_to;
        }
    }
    return bFinish;
}
//////////////////////////////////////////////////////////////////////////

IntAnimateTimeline::IntAnimateTimeline()
{
	m_nCurValue = 0;
	m_pOutRef = NULL;
    m_pMoveAlgorithm = NULL;
}

IntAnimateTimeline::~IntAnimateTimeline()
{
    SAFE_DELETE(m_pMoveAlgorithm);
}

void IntAnimateTimeline::Init()
{

}
void IntAnimateTimeline::x_OnTick()
{
	if (m_nFlags & ANIMATE_TIME_LINE_FLAG_NEED_RESET)
	{
		m_nFlags &= ~ANIMATE_TIME_LINE_FLAG_NEED_RESET;
		m_nCurValue = 0;

        if (m_eTimeType == TLT_BY_FRAME)
            m_nBeginTime = 0;
        else
		    m_nBeginTime = GetTickCount();
	}
    else if (IsFinish())
    {
        return;
    }

	BOOL bTimelineFinish = FALSE;
	int nTimeElapse = 0;
    switch (m_eTimeType)
    {
    case TLT_BY_FRAME:
        nTimeElapse ++;
        break;

    case TLT_BY_MS:
        nTimeElapse = GetTickCount() - m_nBeginTime;
        break;

    case TLT_BY_SECOND:
        nTimeElapse = (GetTickCount() - m_nBeginTime)/1000;
    }
    
    if (m_pMoveAlgorithm)
    {
        bTimelineFinish = m_pMoveAlgorithm->OnTick(nTimeElapse, &m_nCurValue);
    }
    else
    {
        bTimelineFinish = true;
    }
	
	if (bTimelineFinish)
	{
		// 一次播放结束
		if (0 == --m_nRepeatTimes)
        {
            x_SetFinishFlag();
        }
		else
        {
			m_nFlags |= ANIMATE_TIME_LINE_FLAG_NEED_RESET;  // 下次开始前先重置动画参数
        }
	}

	if (m_pOutRef)
	{
		*m_pOutRef = m_nCurValue;
	}
}

IIntMoveAlgorithm*  IntAnimateTimeline::CreateMoveAlgorithm(E_INTTIMELINE_MOVE_ALGORITHM eType)
{
    SAFE_DELETE(m_pMoveAlgorithm);
    switch (eType)
    {
    case ITMA_Linear:
        {
            m_pMoveAlgorithm = new IntLinearMove; 
        }
        break;

    case ITMA_Accelerate:
        {
            m_pMoveAlgorithm = new IntAccelerateMove;
        }
        break;
    }

    return static_cast<IIntMoveAlgorithm*>(m_pMoveAlgorithm);
}

void IntAnimateTimeline::GetCurrentValue(void* pOut)
{
	if (NULL == pOut)
		return;

	*(int*)pOut = m_nCurValue;
}

void IntAnimateTimeline::SetOutRef(int* pRef)
{
	m_pOutRef = pRef;
}
