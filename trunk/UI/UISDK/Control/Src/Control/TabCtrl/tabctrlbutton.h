#pragma once
#include "UISDK\Control\Inc\Interface\itabctrl.h"
#include "UISDK\Control\Src\Control\Button\button.h"

namespace UI
{

class TabCtrlButton : public Button
{
public:
    TabCtrlButton();
    ~TabCtrlButton();

    UI_DECLARE_OBJECT3(TabCtrlButton, OBJ_CONTROL|CONTROL_BUTTON, _T("UICtrl/Control"))

    UI_BEGIN_MSG_MAP
        UIMSG_WM_STATECHANGED2(OnStateChanged)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)  

    void SetITabCtrlButton(ITabCtrlButton* p) { m_pITabCtrlButton = p; SetIMessageProxy(p); }

protected:
    HRESULT FinalConstruct(IUIApplication* p);
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  OnStateChanged(UINT nMask);
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
    void  OnLButtonDown(UINT nFlags, POINT point);

protected:
    ITabCtrlButton*  m_pITabCtrlButton;
};

}