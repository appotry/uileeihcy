#pragma once
#include "Resource.h"
#include "RichEditCtrl.h"
#include "WindowlessRichEdit.h"
#include "LayeredWindow.h"
#include "CaretWindow.h"

class CMainDlg : public CDialogImpl<CMainDlg>, CLayeredWindow<CMainDlg>
{
public:
	enum{IDD = IDD_MAINDLG};
	CMainDlg(void);
	~CMainDlg(void);

	BEGIN_MSG_MAP_EX(CMainDlg)
		//MSG_WM_SIZE(OnSize)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(1111,OnInvalidate);
		if (WM_NCDESTROY != uMsg && WM_TIMER != uMsg)  // Ö÷´°¿ÚÏú»Ù
		{
			CHAIN_MSG_MAP_MEMBER(m_windowlessRichedit)
		}
	END_MSG_MAP()

protected:
	void    OnClose();
	void    OnDestroy();
	BOOL    OnInitDialog(HWND wndFocus, LPARAM);
	void    OnSize(UINT nType, CSize size);
	void    OnMove(POINT ptPos);
	void    OnPaint(HDC);
	UINT    OnNcHitTest(POINT point);
	LRESULT OnInvalidate(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void    OnTimer(UINT_PTR nIDEvent);

private:
	RichEditCtrl        m_richedit;
	WindowlessRichEdit  m_windowlessRichedit;
};
