#pragma once
#include "App\Player\Player_UI\Other\Resource.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
    interface IListBox;
    interface IComboBox;
    interface IPanel;
    interface ISliderCtrl;
    interface IRichEdit;
    interface ILabel;
}
class COptionWindow : public UI::IWindow
{
public:
	enum{ IDD=IDD_OPTION };
	COptionWindow();

	UI_BEGIN_MSG_MAP_Ixxx(COptionWindow)
		UIMSG_WM_INITDIALOG(OnInitDialog)
		UIMSG_WM_CLOSE(OnClose)
		UIMSG_WM_DESTROY(OnDestroy)
		UIMSG_LCN_SELCHANGED(OnLCNSelChanged)
		UIMSG_BN_CLICKED(_T("btn_cancel"), OnBtnCancel)
		UIMSG_BN_CLICKED(_T("btn_save_all"),OnBtnSaveAll)
		UIMSG_BN_CLICKED(_T("btn_reset_all"), OnBtnResetAll)
		UICOMMAND_HANDLER_EX(IDC_BUTTON1, BN_CLICKED, OnBtnClick1 )
		UICOMMAND_HANDLER_EX(IDC_BUTTON2, BN_CLICKED, OnBtnClick2 )
		UIMSG_TRBN_POSCHANGED(m_pSliderVisualFps, OnVisualFpsChanged)
		UIMSG_CBN_SELCHANGED(OnCbnSelChanged)
		UIMSG_WM_RBUTTONUP(OnRButtonUp)
	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
	BOOL OnInitDialog(HWND, LPARAM);
	void OnDestroy();
	void OnClose();
	void OnBtnCancel();
	void OnBtnSaveAll();
	void OnBtnResetAll();

	void OnBtnClick1(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnBtnClick2(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnLCNSelChanged(UI::IMessage* pObjMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem);
	void OnCbnSelChanged(UI::IMessage* pObjMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem);
	void OnVisualFpsChanged( int nPos, int nScrollType );
private:
    UI::IListBox*   m_pListOption;
	UI::IComboBox*  m_pComboBox;
	UI::IPanel*     m_pPanelAbout;

	UI::IPanel*     m_pPanelVisualization;
	UI::IComboBox*  m_pComboboxVisualType;
	UI::ISliderCtrl* m_pSliderVisualFps;
	UI::ILabel*      m_pLabelVisualFps;

	UI::IPanel*     m_pPanelCtrlDemo;
	UI::IPanel*     m_pPanelRichEditDemo;

	// 
	UI::IRichEdit*  m_pIntroduceRichEdit;
};