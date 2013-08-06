#ifndef ITABCTRL_H_8F8A91F6_6870_4bc7_BA67_6E875E19CF12
#define ITABCTRL_H_8F8A91F6_6870_4bc7_BA67_6E875E19CF12
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
namespace UI
{

class TabCtrlBase;
interface IPanel;

interface UICTRLAPI ITabCtrlBase : public IPanel
{
    UI_DECLARE_Ixxx_INTERFACE(ITabCtrlBase, TabCtrlBase);

    IPanel*  GetHeadPanel();
    IPanel*  GetContentPanel();

};

class TabCtrl;
interface UICTRLAPI ITabCtrl : public ITabCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(ITabCtrl, TabCtrl);

    void  AddItem(const TCHAR*  szText, IObject* pContentObj);
};

class TabCtrlButton;
interface  ITabCtrlButton : public IButton
{
    UI_DECLARE_Ixxx_INTERFACE(ITabCtrlButton, TabCtrlButton);
};


}

#endif // ITABCTRL_H_8F8A91F6_6870_4bc7_BA67_6E875E19CF12