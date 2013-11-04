#pragma  once
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"

namespace UI
{

//
// 备注：
//		由于使用了分层窗口，在用Gdiplus DrawString时，对某些字体(如宋体）会出现中文不居中的情况
//      该问题最后没有解决。通过SetPadding()，稍稍改观了一些。
//
class TooltipWindow: public CustomWindow, public IToolTipUI
{
public:
	TooltipWindow();
	virtual ~TooltipWindow();

	UI_DECLARE_OBJECT_INFO(TooltipWindow, OBJ_WINDOW|WINDOW_TOOLTIP)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

public:
	void   OnPaint(IRenderTarget* pRenderTarget);

	BOOL  PreCreateWindow(CREATESTRUCT* pcs);
	virtual void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	virtual void  OnFinalMessage();

	virtual bool  Create();
	virtual bool  Destroy();
	virtual bool  SetText(const String& strText);
	virtual bool  SetTitle(const String& strText);
	virtual bool  Show();
	virtual bool  Hide();
	virtual void  SetAttribute(const ATTRMAP& mapAttrib);
//	virtual bool  SetUIApplication(IUIApplication* p) { return false; }
private:
	String   m_strText;
	int      m_nLimitWidth;
};


}