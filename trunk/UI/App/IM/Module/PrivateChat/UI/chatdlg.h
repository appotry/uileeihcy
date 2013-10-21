#pragma once
//#include "..\IM_Ctrls\QQContactMgrListView.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "emotiondlg.h"

namespace UI
{
    interface  IStoryboard;
    interface  IButton;
    interface  ISliderCtrl;
    interface  IPanel;
    interface  IRichEdit;
    interface  ISplitterBar;
    interface  ICheckButton;
    interface  IComboBox;
}
class CPrivateChatUI;

class CChatDlg : public UI::ICustomWindow
{
public:
	CChatDlg(void);
	~CChatDlg(void);

	UI_BEGIN_MSG_MAP_Ixxx(CChatDlg)
		UIMSG_BN_CLICKED2(m_pBtnAnimate, OnBnClickAnimate)
        UIMSG_BN_CLICKED2(m_pBtnFont, OnBtnFont)
        UIMSG_BN_CLICKED2(m_pBtnEmotion, OnBtnEmotion)
        UIMSG_BN_CLICKED2(m_pBtnFontBold, OnBtnFontBold)
        UIMSG_BN_CLICKED2(m_pBtnFontItalic, OnBtnFontItalic)
        UIMSG_BN_CLICKED2(m_pBtnFontUnderline, OnBtnFontUnderline)
        UIMSG_BN_CLICKED2(m_pBtnFontColor, OnBtnFontColor)
        UIMSG_CBN_SELCHANGED(m_pComboFontSize, OnComboSizeSelChanged)
        UIMSG_CBN_SELCHANGED(m_pComboFontFace, OnComboFontFaceSelChanged)
        UIMSG_BN_CLICKED2(m_pBtnClose, OnBtnClose)
        UIMSG_BN_CLICKED2(m_pBtnSend, OnBtnSend)
        UIMSG_BN_CLICKED2(m_pBtnSendMode, OnBtnSendMode)

        UIMESSAGE_HANDLER_EX(WM_EMOTION_DLG_HIDE, OnEmotionDlgHide)
        UIMESSAGE_HANDLER_EX(WM_EMOTION_INSERT, OnInsertEmotion)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
		UIMSG_WM_SYSCOMMAND(OnSysCommand)
        UIMSG_WM_CLOSE(OnClose)
        UIMSG_WM_INITIALIZE(OnInitWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(UI::ICustomWindow)

public:
	void  OnInitWindow();
	void  OnTextureAlphaChanged(int nNewAlpha);
	void  OnBnClickAnimate();
    void  OnBtnFont();
    void  OnBtnEmotion();
	void  OnSysCommand(UINT nID, CPoint point);
    void  OnClose();
    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
    void  OnBtnFontBold();
    void  OnBtnFontItalic();
    void  OnBtnFontColor();
    void  OnBtnFontUnderline();
    void  OnBtnClose();
    void  OnBtnSend();
    void  OnBtnSendMode();
    void  OnComboSizeSelChanged(UI::IListItemBase* pOldItem, UI::IListItemBase* pNewItem);
    void  OnComboFontFaceSelChanged(UI::IListItemBase* pOldItem, UI::IListItemBase* pNewItem);
    
    LRESULT  OnEmotionDlgHide(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnInsertEmotion(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void  SetPrivateChatUI(CPrivateChatUI* p);
    void  SetSkin(UI::IRenderBase* p);
private:
	UI::IButton*   m_pBtnAnimate;
	UI::IPanel*    m_pRightPanel;
	UI::IPanel*    m_pLeftPanel;
    UI::IRichEdit*  m_pRichEditInput;
    UI::IRichEdit*  m_pRichEditOutput;
    UI::ISplitterBar*  m_pSplitterBar;
    UI::ICheckButton*  m_pBtnFont;
    UI::IButton*   m_pBtnEmotion;

    UI::IButton*   m_pBtnClose;
    UI::IButton*   m_pBtnSend;
    UI::IButton*   m_pBtnSendMode;

    // ×ÖÌå
    UI::IPanel*    m_pPanelFontToolbar;
    UI::ICheckButton*  m_pBtnFontBold;
    UI::ICheckButton*  m_pBtnFontItalic;
    UI::ICheckButton*  m_pBtnFontUnderline;
    UI::IComboBox*     m_pComboFontFace;
    UI::IComboBox*     m_pComboFontSize;
    UI::IButton*       m_pBtnFontColor;

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

    CPrivateChatUI*  m_pPrivateChatUI;
};

