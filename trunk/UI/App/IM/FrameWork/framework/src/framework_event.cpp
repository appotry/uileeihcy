#include "stdafx.h"
#include "framework.h"
#include "App\IM\FrameWork\framework\src\event.h"


void  CFramework::AddUIEvent(IM::EventType szName)
{
    if (NULL == szName)
        return;

    UIASSERT(IsUIThread());
    if (!IsUIThread())
        return;

    if (FindUIEvent(szName))
        return;

    UIEvent* pEvent = new UIEvent(this, szName);
    m_vecUIEvent.push_back(pEvent);
}
void  CFramework::RemoveUIEvent(IM::EventType szName)
{
    if (NULL == szName)
        return;

    UIASSERT(IsUIThread());
    if (!IsUIThread())
        return;

    vector<UIEvent*>::iterator iter = m_vecUIEvent.begin();
    for (; iter != m_vecUIEvent.end(); iter++)
    {
        if (0 == _tcscmp((*iter)->GetName(), szName))
        {
            m_vecUIEvent.erase(iter);
            return;
        }
    }
}
UIEvent*  CFramework::FindUIEvent(IM::EventType szName)
{
    if (NULL == szName)
        return NULL;

    vector<UIEvent*>::iterator iter = m_vecUIEvent.begin();
    for (; iter != m_vecUIEvent.end(); iter++)
    {
        if (0 == _tcscmp((*iter)->GetName(), szName))
            return (*iter);
    }

    return NULL;
}
void  CFramework::SubscribeUIEvent(IM::EventType szName, IM::IUI* pUI)
{
    if (NULL == szName || NULL == pUI)
        return;

    UIASSERT(IsUIThread());
    if (!IsUIThread())
        return;

    UIEvent* pEvent = FindUIEvent(szName);
    if (NULL == pEvent)
        return;

    pEvent->AddListener(pUI);
}
void  CFramework::UnsubscribeUIEvent(IM::EventType szName, IM::IUI* pUI)
{
    if (NULL == szName || NULL == pUI)
        return;

    UIASSERT(IsUIThread());
    if (!IsUIThread())
        return;

    UIEvent* pEvent = FindUIEvent(szName);
    if (NULL == pEvent)
        return;

    pEvent->RemoveListener(pUI);
}
void  CFramework::FireUIEvent(IM::EventType szName, LPARAM lParam)
{
    if (NULL == szName)
        return;

    UIASSERT(IsUIThread());
    if (!IsUIThread())
        return;

    UIEvent* pEvent = FindUIEvent(szName);
    if (NULL == pEvent)
        return;

    pEvent->FireEvent(lParam);
}
