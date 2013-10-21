#ifndef CHECKBUTTON_H_3EF5E2FB_6904_4312_81CB_C12CAB443199
#define CHECKBUTTON_H_3EF5E2FB_6904_4312_81CB_C12CAB443199
#include "UISDK\Control\Src\Control\Button\button.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"

namespace UI
{

class CheckButton : public Button
{
public:
    CheckButton();
    ~CheckButton();

    UI_DECLARE_OBJECT3(CheckButton, OBJ_CONTROL|CONTROL_CHECKBUTTON, _T("UICtrl/Control"))

    UI_BEGIN_MSG_MAP
        UIMSG_BN_CLICKED2(m_pIButtonBase, OnClicked)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)

    void  SetICheckButton(ICheckButton* p) { m_pICheckButton = p; }

    HRESULT  FinalConstruct(IUIApplication* p);
    void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

protected:
    ICheckButton*  m_pICheckButton;
};


}
#endif // CHECKBUTTON_H_3EF5E2FB_6904_4312_81CB_C12CAB443199