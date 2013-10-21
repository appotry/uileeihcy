// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "../src/framework.h"
#include "App\IM\include/framework_inc.h"

CFramework   g_framework;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



namespace IM
{

IFramework*  GetFramework()
{
    return g_framework.GetIFramework();
}

UI::IUIApplication*  GetUIApplication()
{
    return g_framework.GetUIApplication();
}

IFramework::IFramework(CFramework* p)
{
    m_pImpl = p;
}
void  IFramework::Run(long lThreadMode)
{
    m_pImpl->Run(lThreadMode);
}
void  IFramework::Exit()
{
    m_pImpl->Exit();
}
void  IFramework::LoadModuleByTime(const TCHAR* szTime)
{
    m_pImpl->LoadModuleByTime(szTime);
}
void  IFramework::LoadModuleByName(const TCHAR* szName, IM::IUI** ppUI, IM::IBiz** ppBiz)
{
    m_pImpl->LoadModuleByName(szName, ppUI, ppBiz);
}
IBiz*  IFramework::GetBiz(const TCHAR* szName)
{
    return m_pImpl->GetBiz(szName);
}
IUI*  IFramework::GetUI(const TCHAR* szName)
{
    return m_pImpl->GetUI(szName);
}

void  IFramework::PostTask(ITask* p)
{
    m_pImpl->PostTask(p);
}
void  IFramework::PostFunction(IM::THREAD_TYPE eType, IM::fun_pkg* p, bool bWait)
{
    m_pImpl->PostFunction(eType, p, bWait);
}
void  IFramework::AddUIEvent(EventType szName)
{
    m_pImpl->AddUIEvent(szName);
}
void  IFramework::RemoveUIEvent(EventType szName)
{
    m_pImpl->RemoveUIEvent(szName);
}
void  IFramework::SubscribeUIEvent(EventType szName, IM::IUI* pUI)
{
    m_pImpl->SubscribeUIEvent(szName, pUI);
}
void  IFramework::UnsubscribeUIEvent(EventType szName, IM::IUI* pUI)
{
    m_pImpl->UnsubscribeUIEvent(szName, pUI);
}
void  IFramework::FireUIEvent(EventType szName, LPARAM lParam)
{
    m_pImpl->FireUIEvent(szName, lParam);
}
}