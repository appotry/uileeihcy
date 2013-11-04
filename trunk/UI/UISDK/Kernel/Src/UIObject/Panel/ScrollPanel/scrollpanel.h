#pragma once
#include "UISDK\Kernel\Inc\Interface\iscrollpanel.h"
#include "UISDK\Kernel\Src\UIObject\ScrollBar\scrollbarmanager.h"
#include "UISDK\Kernel\Src\UIObject\Panel\panel.h"

namespace UI
{

class ScrollPanel : public Panel
{
public:
	ScrollPanel();
	~ScrollPanel();

    UI_DECLARE_OBJECT3(ScrollPanel, OBJ_CONTROL|PANEL_SCROLL, _T("Kernel/Container"))

	UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
        UICHAIN_MSG_MAP_POINT_MEMBER(m_pMgrScrollBar)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT(Panel)

    void  SetIScrollPanel(IScrollPanel* p) { m_pIScrollPanel = p; }
    IScrollPanel*  GetIScrollPanel() { return m_pIScrollPanel; }

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
	void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload );
    void  OnSize(UINT nType, int cx, int cy);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
//	UINT  OnHitTest(POINT* pt);

protected:
    IScrollPanel*      m_pIScrollPanel;
	ScrollBarManager*  m_pMgrScrollBar;

    SIZE    m_sizeView;  // 视图大小（包含滚动区域），如果未指定，则获取所有子控件的位置进行计算
};
}