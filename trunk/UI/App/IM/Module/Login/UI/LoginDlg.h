#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Control\Inc\Interface\itrayicon.h"
namespace UI
{
    interface IButton;
    interface IPanel;
    interface IStoryboard;
    interface IFlashWrap;
    interface IGifCtrl;
    interface ILabel;
}
interface IQQLoginAccountComboBox;
class CLoginUI;
class CLoginDlg : public UI::ICustomWindow
{
public:
	CLoginDlg(void);
	~CLoginDlg(void);

	UI_BEGIN_MSG_MAP_Ixxx(CLoginDlg)
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
		UIMSG_WM_ANIMATE_OVER(OnAnimateOver)
        UIMSG_WM_TIMER(OnTimer)
		UIMSG_BN_CLICKED2(m_pBtnSet, OnBtnSet)
		UIMSG_BN_CLICKED2(m_pBtnLoginState, OnBtnClickLoginStatus)
		UIMSG_BN_CLICKED2(m_pBtnNetSetCancel, OnBtnNetSetCancel)
		UIMSG_BN_CLICKED2(m_pBtnLogon, OnBtnLogin)
        UIMSG_BN_CLICKED2(m_pBtnLogonCacel, OnBtnLogonCancel)
		UIMESSAGE_HANDLER_EX(UI_WM_TRAYICON, OnTrayIconNotify)
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pTrayIcon)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMSG_WM_SYSCOMMAND(OnSysCommand)
        UIMESSAGE_HANDLER_EX(UI_WM_SHOWMODALWINDOW, OnShowModal)
	UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

    void  SetLoginUI(CLoginUI* p) { m_pLoginUI = p; }
    void  ShowLogining(const TCHAR* szAccount);
    void  OnLoginResult(long lRstCode, const TCHAR* szDesc);
    void  Show();
    void  Hide(bool bQuitOrDestroy);
    void  OnCancelLoginOk();

protected:
    void  OnInitWindow();
    LRESULT  OnShowModal(UINT, WPARAM, LPARAM);
    void  OnSysCommand(UINT nID, CPoint point);

    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
    void  OnAnimateTick_GoStep1(UI::IStoryboard* p);
    void  OnAnimateTick_BackStep1(UI::IStoryboard* p);
    void  OnAnimateTick_BackStep2(UI::IStoryboard* p);
    void  OnAnimateTick_Hide(UI::IStoryboard* p);
    void  OnAnimateTick_Show(UI::IStoryboard* p);

	void  OnAnimateOver();
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void  OnBtnSet();
	void  OnBtnClickLoginStatus();
	void  OnBtnNetSetCancel();
	void  OnBtnLogin();
    void  OnBtnLogonCancel();

	void  OnPaint(UI::IRenderTarget* pRenderTarget);
	LRESULT OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void  InitFlash();
    void  AnimateHideWindow(int nId);
    void  AnimateShowWindow(int nId);

private:
	UI::IButton*  m_pBtnSet;
	UI::IButton*  m_pBtnLoginState;
	UI::IButton*  m_pBtnNetSetCancel;
	UI::IButton*  m_pBtnLogon;
    UI::IButton*  m_pBtnLogonCacel;
	UI::IPanel*   m_pPanelLogin;
    UI::IPanel*   m_pPanelLoginInput;
    UI::IPanel*   m_pPanelLogining;
    UI::IPanel*   m_pPanelNetSet;

    UI::IGifCtrl*    m_pGifLoadding;
	UI::ITrayIcon*   m_pTrayIcon;
    UI::IBufferRenderLayer*  m_pFlashLayer;
    UI::IFlashWrap*  m_pFlashCtrl;
    UI::ILabel*      m_pLabelLoginingText;

 	IQQLoginAccountComboBox*  m_pAccoutCombobox;

    String  m_strLoginingText;
    int     m_nTrailPointCount;  // 正在登录(xxx) 后面的...数量

    CLoginUI*  m_pLoginUI;
};
