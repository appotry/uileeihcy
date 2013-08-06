#ifndef COMPOUNDBUTTON_H_B8543ACA_36A7_4c89_A4C1_4E759AAC329F
#define COMPOUNDBUTTON_H_B8543ACA_36A7_4c89_A4C1_4E759AAC329F
#include "UISDK\Control\Src\Control\Button\button.h"
#include "UISDK\Control\Inc\Interface\icompoundbutton.h"

namespace UI
{
interface ILayout;

// 复合按钮类型
class CompoundButton : public ButtonBase
{
public:
	UI_DECLARE_OBJECT3(CompoundButton, OBJ_COMPOUND_CONTROL, _T("UICtrl/Control"))

	CompoundButton();
	~CompoundButton();

	UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
		UIMESSAGE_HANDLER_EX(UI_WM_GETLAYOUT, OnGetLayoutPtr)
		UIMSG_WM_OBJECTLOADED(OnObjectLoaded)
		UIMSG_WM_STATECHANGED2(OnStateChanged2)
		UIMSG_WM_REDRAWOBJECT(OnRedrawObject)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
	UIALT_MSG_MAP(1)
		UIMSG_WM_STATECHANGED2(OnChildObjStateChanged)
	UI_END_MSG_MAP_CHAIN_PARENT(ButtonBase)
	
    void  SetICompoundButton(ICompoundButton* p) { m_pICompoundButton = p; }

public:
    void  OnSize(UINT nType, int cx, int cy);
	void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMatAttrib, bool bReload);
	void  GetDesiredSize(SIZE* pSize);
	void  DrawFocus(IRenderTarget*);
	
	void  OnStateChanged2(UINT nMask);
	void  OnObjectLoaded();
	void  OnRedrawObject();
	LRESULT  OnGetLayoutPtr(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void  OnChildObjStateChanged(UINT nMask);

protected:
	ILayout*  m_pLayout;
    ICompoundButton*  m_pICompoundButton;
};


}

#endif // COMPOUNDBUTTON_H_B8543ACA_36A7_4c89_A4C1_4E759AAC329F