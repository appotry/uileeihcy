#pragma once
#include "Resource.h"
#include "RichEditCtrl.h"

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum{IDD = IDD_ABOUTBOX};
	CMainDlg(void);
	~CMainDlg(void);

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
	END_MSG_MAP()

protected:
	void  OnClose();
	BOOL  OnInitDialog(HWND wndFocus, LPARAM);

private:
	RichEditCtrl   m_richedit;
};
