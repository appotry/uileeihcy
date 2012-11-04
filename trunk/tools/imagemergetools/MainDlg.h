#pragma once
#include <ShellAPI.h>
#include "Resource.h"


enum IMAGE_ITEM_DIRECTION_TYPE
{
	DIRECTION_H,
	DIRECTION_V,
};

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum {IDD=IDD_MAINDLG};

	CMainDlg(void);
	~CMainDlg(void);

	BEGIN_MSG_MAP(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DROPFILES(OnDropFiles)
		MSG_WM_PAINT(OnPaint)
		COMMAND_HANDLER_EX(IDCANCEL, BN_CLICKED, OnCancel)
		COMMAND_HANDLER_EX(IDC_BTN_SAVEAS, BN_CLICKED, OnSaveAs)
		COMMAND_HANDLER_EX(IDC_BTN_SAVE, BN_CLICKED, OnSave)
		COMMAND_HANDLER_EX(IDC_BTN_RESET, BN_CLICKED, OnReset)
		COMMAND_HANDLER_EX(IDC_RADIO_H, BN_CLICKED, OnRadioH)
		COMMAND_HANDLER_EX(IDC_RADIO_V, BN_CLICKED, OnRadioV)
	END_MSG_MAP()
public:
	
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDropFiles(HDROP hDropInfo);
	void OnPaint(CDCHandle dc);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnSaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnSave(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnReset(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnRadioH(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnRadioV(UINT uNotifyCode, int nID, CWindow wndCtl);
	

	//////////////////////////////////////////////////////////////////////////

	void AppendFile(TCHAR* szFileName);

protected:
	CEdit   m_EditWidth;
	CEdit   m_EditHeight;
	CEdit   m_EditItemCount;
	CEdit   m_EditItemWidth;
	CEdit   m_EditItemHeight;
	CButton m_btnRadioH;
	CButton m_btnRadioV;
	CButton m_btnAddLast;
	CButton m_btnInsert;
	CEdit   m_EditInsertPos;

	//////////////////////////////////////////////////////////////////////////
	UI::Image  m_image;
	TCHAR  m_szFileSavePath[MAX_PATH];  // ±£´æÂ·¾¶


	int  m_nImageItemCount;
	int  m_nImageWidth;
	int  m_nImageHeight;
	int  m_nItemWidth;
	int  m_nItemHeight;
	
	IMAGE_ITEM_DIRECTION_TYPE m_eDirType;
};
