#pragma once
#include "Resource.h"

class COptionWindow : public UI::Window
{
public:
	enum{ IDD=IDD_OPTION };
	COptionWindow();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_INITDIALOG(OnInitDialog)
		UIMSG_WM_CLOSE(OnClose)
		UIMSG_WM_DESTROY(OnDestroy)
		UIMSG_LCN_SELCHANGED(OnLCNSelChanged)
		UIMSG_BN_CLICKED(_T("btn_cancel"), OnBtnCancel)
		UIMSG_BN_CLICKED(_T("btn_save_all"),OnBtnSaveAll)
		UIMSG_BN_CLICKED(_T("btn_reset_all"), OnBtnResetAll)
		UICOMMAND_HANDLER_EX(IDC_BUTTON1, BN_CLICKED, OnBtnClick1 )
		UICOMMAND_HANDLER_EX(IDC_BUTTON2, BN_CLICKED, OnBtnClick2 )
		UIMSG_WM_RBUTTONUP(OnRButtonUp)
	UI_END_MSG_MAP_CHAIN_PARENT(WindowBase)

public:
	virtual BOOL PreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
	BOOL OnInitDialog( HWND, LPARAM );
	void OnDestroy();
	void OnClose();
	void OnBtnCancel();
	void OnBtnSaveAll();
	void OnBtnResetAll();

	void OnBtnClick1(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnBtnClick2(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnLCNSelChanged(ListItemBase* pOldSelItem, ListItemBase* pSelItem);

private:
	ListBox*   m_pListOption;
	Combobox*  m_pComboBox;
	Panel*     m_pPanelAbout;
	Panel*     m_pPanelVisualization;
	Panel*     m_pPanelCtrlDemo;
	Panel*     m_pPanelRichEditDemo;

	// 
	RichEdit*  m_pIntroduceRichEdit;
};