#ifndef CONTROL_H_1ED0C667_F0AA_4f48_BAC1_FB1618A7417C
#define CONTROL_H_1ED0C667_F0AA_4f48_BAC1_FB1618A7417C

#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{
class Control : public Object
{
public:
    Control();

    UI_DECLARE_OBJECT(Control, OBJ_CONTROL)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
		UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
    UI_END_MSG_MAP_CHAIN_PARENT(Object)

    IControl*  GetIControl() { return m_pIControl; }
    void  SetIControl(IControl* p) { m_pIControl = p; }

    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
protected:
    IControl*  m_pIControl;
};

}

#endif  // CONTROL_H_1ED0C667_F0AA_4f48_BAC1_FB1618A7417C