#pragma once
#include "UISDK\Control\Inc\Interface\ihyperlink.h"
#include "UISDK\Control\Src\Control\Button\button.h"
namespace UI
{

class HyperLink : public Button
{
public:
	HyperLink();

	UI_DECLARE_OBJECT3(HyperLink, OBJ_CONTROL|CONTROL_HYPERLINK, _T("UICtrl/Control"))

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR(OnSetCursor)
        UIMSG_BN_CLICKED2(m_pIHyperLink, OnClicked)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT(Button)

    void  SetIHyperLink(IHyperLink* p) { m_pIHyperLink = p;}

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  ResetAttribute();
	BOOL  OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
    void  OnClicked();

    IHyperLink*  m_pIHyperLink;

    String  m_strHref;
};


}
