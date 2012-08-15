#pragma once
#include "Resource.h"
#include "RichEditCtrl.h"
#include "WindowlessRichEdit.h"

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum{IDD = IDD_ABOUTBOX};
	CMainDlg(void);
	~CMainDlg(void);

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_SIZE(OnSize)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_PAINT(OnPaint)

		CHAIN_MSG_MAP_MEMBER(m_windowlessRichedit)
	END_MSG_MAP()

protected:
	void  OnClose();
	void  OnDestroy();
	BOOL  OnInitDialog(HWND wndFocus, LPARAM);
	void  OnSize(UINT nType, CSize size);
	void  OnPaint(HDC);

private:
	RichEditCtrl        m_richedit;
	WindowlessRichEdit  m_windowlessRichedit;
};
