#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
class CControlDemoWindow : public UI::IWindow
{
public:
    CControlDemoWindow();
	~CControlDemoWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CControlDemoWindow)
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMESSAGE_HANDLER_EX(UI_WM_DROPTARGETEVENT, OnDropTargetEvent)
        UIMSG_BN_CLICKED3(OnBnClicked)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    LRESULT OnDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  OnPaint(UI::IRenderTarget* pRenderTarget);

    void  OnBnClicked(IMessage*  pMsgFrom);
};

