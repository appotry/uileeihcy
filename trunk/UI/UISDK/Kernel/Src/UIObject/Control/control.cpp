#include "stdafx.h"
#include "control.h"

namespace UI
{

Control::Control()
{
    m_pIControl = NULL;
}


void  Control::ResetAttribute()
{
    __super::ResetAttribute();
    this->ModifyStyle(CONTROL_STYLE_TABSTOP, 0, false);
}

void  Control::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    __super::SetAttribute(pMapAttrib, bReload);

    // 控件样式
    bool bTabStop = false;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_CONTROL_STYLE_TABABLE, true, &bTabStop))
    {
        if(false == bTabStop)
            m_nStyle &= ~CONTROL_STYLE_TABSTOP;  // 默认控件都有tabstop
    }

    bool bGroup = false;
    pMapAttrib->GetAttr_bool(XML_CONTROL_STYLE_GROUP, true, &bGroup);
    if(bGroup)
        m_nStyle |= CONTROL_STYLE_GROUP;
    else
        m_nStyle &= ~CONTROL_STYLE_GROUP;
}
void  Control::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
	__super::OnEditorGetAttrList(pData);

	IUIEditorGroupAttribute*  pPanelGroup = pData->pEditor->CreateGroupAttribute(pData->pGroupAttr, Control::GetXmlName(), NULL);
	pData->pEditor->CreateTextAttribute(pPanelGroup, XML_CONTROL_STYLE_TABABLE, pData->szPrefix);
	pData->pEditor->CreateTextAttribute(pPanelGroup, XML_CONTROL_STYLE_GROUP, pData->szPrefix);
}

}