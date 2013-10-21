#include "stdafx.h"
#include "event.h"
#include "framework.h"

#define EVENTLock  IM::CSLock __cslock(&m_cs);
UIEvent::UIEvent(CFramework* pFramework, const TCHAR* szName)
{
    InitializeCriticalSection(&m_cs);
    m_pFramework = pFramework;
    if (szName)
    {
        m_strName = szName;
    }
}
UIEvent::~UIEvent()
{
    DeleteCriticalSection(&m_cs);
}

void UIEvent::AddListener( IM::IUI* pUI )
{
    EVENTLock;
    if (this->IsListenExist(pUI))
        return;

    m_vecListener.push_back(pUI);
}

void UIEvent::RemoveListener( IM::IUI* pUI )
{
    EVENTLock;

    vector<IM::IUI*>::iterator iter = std::find(m_vecListener.begin(), m_vecListener.end(), pUI);
    if (iter == m_vecListener.end())
        return;

    m_vecListener.erase(iter);
}

void UIEvent::FireEvent(LPARAM lParam)
{
    if (!m_pFramework->IsUIThread())
        return;

    EVENTLock;

    vector<IM::IUI*>::iterator iter = m_vecListener.begin();
    for (; iter != m_vecListener.end(); iter++)
    {
        (*iter)->ExecuteCommand(IM::BASE_CMD_ID_EVENT, (WPARAM)m_strName.c_str(), lParam);
    }
}

int UIEvent::GetCount()
{
    return (int)m_vecListener.size();
}
const TCHAR*  UIEvent::GetName()
{
    return m_strName.c_str();
}
bool UIEvent::IsListenExist( IM::IUI* pUI )
{
    vector<IM::IUI*>::iterator iter = std::find(m_vecListener.begin(), m_vecListener.end(), pUI);
    if (iter == m_vecListener.end())
        return false;

    return true;
}