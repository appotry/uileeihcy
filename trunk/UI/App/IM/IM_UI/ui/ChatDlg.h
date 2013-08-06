#pragma once
//#include "..\IM_Ctrls\QQContactMgrListView.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"

namespace UI
{
    interface  IStoryboard;
    interface  IButton;
    interface  ISliderCtrl;
    interface  IPanel;
    interface  IRichEdit;
}

class CChatDlg : public UI::ICustomWindow
{
public:
	CChatDlg(void);
	~CChatDlg(void);

	UI_BEGIN_MSG_MAP_Ixxx(CChatDlg)
		UIMSG_BN_CLICKED2(m_pBtnAnimate, OnBnClickAnimate)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
		UIMSG_WM_SYSCOMMAND(OnSysCommand)
        UIMSG_WM_INITIALIZE(OnInitWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(UI::ICustomWindow)

public:
	void    OnInitWindow();
	void    OnTextureAlphaChanged(int nNewAlpha);
	void    OnBnClickAnimate();
	void    OnSysCommand(UINT nID, CPoint point);
    void    OnAnimateTick(int nCount, UI::IStoryboard** ppArray);

private:
	UI::IButton*   m_pBtnAnimate;
	UI::IPanel*    m_pRightPanel;
	UI::IPanel*    m_pLeftPanel;
    UI::IRichEdit*  m_pRichEditInput;
    UI::IRichEdit*  m_pRichEditOutput;

//	QQContactMgrListView*  m_pListView;

	enum RIGHT_PANEL_STATE
	{
		RIGHT_PANEL_STATE_EXPAND,
		RIGHT_PANEL_STATE_COLLAPSE,
		RIGHT_PANEL_STATE_ANIMATING_EXPAND,
		RIGHT_PANEL_STATE_ANIMATING_COLLAPSE
	};
	RIGHT_PANEL_STATE m_eRightPanelState;
	int m_nRightPanelConfigWidth;
	int m_nLeftPanelWidth;
};

