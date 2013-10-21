#ifndef RADIOBUTTON_H_3913A8BF_C812_4c9a_A29B_9237BFFC5319
#define RADIOBUTTON_H_3913A8BF_C812_4c9a_A29B_9237BFFC5319

#include "UISDK\Control\Src\Control\Button\button.h"
#include "UISDK\Control\Inc\Interface\iradiobutton.h"


namespace UI
{

class RadioButton : public Button
{
public:
    RadioButton();
    
    UI_DECLARE_OBJECT3(RadioButton, OBJ_CONTROL|CONTROL_RADIOBUTTON, _T("UICtrl/Control"))

    UI_BEGIN_MSG_MAP
        UIMSG_BN_CLICKED2(m_pIButtonBase, OnClicked)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT(Button)

    void  SetIRadioButton(IRadioButton* p) { m_pIRadioButton = p; }

public:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  OnClicked();
    UINT  OnGetDlgCode(LPMSG lpMsg) { return 0; }

protected:
    IRadioButton*  m_pIRadioButton;
};

}

#endif // RADIOBUTTON_H_3913A8BF_C812_4c9a_A29B_9237BFFC5319