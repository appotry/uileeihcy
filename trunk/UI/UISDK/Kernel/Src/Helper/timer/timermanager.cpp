#include "stdafx.h"
#include "timermanager.h"

namespace UI
{

TimerHelper* TimerHelper::GetInstance()
{
    static TimerHelper s;
    return &s;
}

TimerHelper::TimerHelper()
{
    // m_thunk.Init((DWORD_PTR)TimerHelper::TimerProc, this);
}

UINT_PTR  TimerHelper::SetTimer(int nElapse, IMessage* pNotify)
{
    if (NULL == pNotify)
        return 0;

    TimerItem  ti;
    ti.pNotify = pNotify;
    return this->SetTimer(nElapse, &ti);
}
UINT_PTR  TimerHelper::SetTimerById(int nElapse, int nId, IMessage* pNotify)
{
    if (NULL == pNotify)
        return 0;

    // 查找该pNotify对应的nId是否已经存在
    _MyIter iter = m_mapTimerItem.begin();
    for (; iter != m_mapTimerItem.end(); iter++)
    {
        if (iter->second.nId == nId && iter->second.pNotify == pNotify)
            return 0;
    }

    TimerItem  ti;
    ti.nId = nId;
    ti.pNotify = pNotify;
    return this->SetTimer(nElapse, &ti);
}

UINT_PTR  TimerHelper::SetTimer(int nElapse, TimerItem* pItem)
{
    if (NULL == pItem)
        return 0;

    UINT_PTR nTimerID = ::SetTimer(NULL, 0, nElapse, TimerHelper::TimerProc);
    m_mapTimerItem[nTimerID] = *pItem;

    int nCount = m_mapTimerItem.size();
    UI_LOG_DEBUG(_T("%s  timer id:%d, nElapse=%d, id=%d, pNotify=0x%x"), FUNC_NAME, nTimerID, nElapse, pItem->nId, pItem->pNotify);
    return nTimerID;
}
void  TimerHelper::KillTimer(UINT_PTR& nTimerIdRef)
{
    _MyIter iter = m_mapTimerItem.find(nTimerIdRef);
    if (iter != m_mapTimerItem.end())
    {
        UI_LOG_DEBUG(_T("%s timer id:%d"), FUNC_NAME, nTimerIdRef);

        ::KillTimer(0, nTimerIdRef);

        m_mapTimerItem.erase(iter);
        nTimerIdRef = 0;
        return;
    }
}

void  TimerHelper::KillTimerById(int nId, IMessage* pNotify)
{
    _MyIter iter = m_mapTimerItem.begin();
    for (; iter != m_mapTimerItem.end(); iter++)
    {
        if (iter->second.nId == nId && iter->second.pNotify == pNotify)
        {
            UI_LOG_DEBUG(_T("%s timer id:%d, id=%d"), FUNC_NAME, iter->first, nId);

            ::KillTimer(NULL, iter->first);
            m_mapTimerItem.erase(iter);
            return;
        }
    }
}

VOID CALLBACK  TimerHelper::TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent,	DWORD dwTime )
{
    TimerHelper::GetInstance()->OnTimer(idEvent);
}
void  TimerHelper::OnTimer(UINT_PTR idEvent)
{
    if (0 == m_mapTimerItem.count(idEvent))
    {
        UIASSERT(0);
        TimerHelper::GetInstance()->KillTimer(idEvent);
        return;
    }
    TimerItem& ti = m_mapTimerItem[idEvent];

    if (-1 != ti.nRepeatCount)
        ti.nRepeatCount --;

    if (ti.pNotify)
    {
        ::UISendMessage(ti.pNotify, WM_TIMER, idEvent, (LPARAM)&ti);
    }
    else
    {
        ti.pProc(idEvent, &ti);
    }

    if (0 == ti.nRepeatCount)
    {
        KillTimer(idEvent);
    }
}

}