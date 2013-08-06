#ifndef CONTROLAPI_H_AC9FAB02_B6D5_4a6b_97CE_9C7A03586855
#define CONTROLAPI_H_AC9FAB02_B6D5_4a6b_97CE_9C7A03586855

namespace UI
{
interface IUIApplication;
interface IMenu;

extern "C"
UICTRLAPI bool  UI_Ctrl_RegisterUIObject(IUIApplication* p);

extern "C"
UICTRLAPI HRESULT  UICreateTrayIcon(IUIApplication* pUIApplication, ITrayIcon** ppOut);

extern "C"
UICTRLAPI bool  UILoadMenu(IUIApplication* pUIApplication, const TCHAR* szMenuId, IMenu** ppMenu);

}
#endif  // CONTROLAPI_H_AC9FAB02_B6D5_4a6b_97CE_9C7A03586855