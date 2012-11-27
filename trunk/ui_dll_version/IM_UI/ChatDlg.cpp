#include "StdAfx.h"
#include "ChatDlg.h"

CChatDlg::CChatDlg(void)
{
	m_pChangeBkImgBtn = NULL;
	this->SetWindowResizeType(WRSB_ALL);
}

CChatDlg::~CChatDlg(void)
{
}

// #include "dwmapi.h"
// #pragma comment(lib, "dwmapi.lib")

void CChatDlg::OnInitWindow()
{
	__super::OnInitWindow();

// 	HRGN hRgn = CreateRectRgn(10,10,500,500);
// 	DWM_BLURBEHIND blurbehind = {0};
// 	blurbehind.dwFlags = DWM_BB_ENABLE|DWM_BB_BLURREGION|DWM_BB_TRANSITIONONMAXIMIZED;
// 	blurbehind.fEnable = true;
// 	blurbehind.hRgnBlur = hRgn;
// 	blurbehind.fTransitionOnMaximized = TRUE;
// 	DwmEnableBlurBehindWindow(m_hWnd, &blurbehind);
// 	DeleteObject(hRgn);

// 	MARGINS m = {-1};
// 	DwmExtendFrameIntoClientArea(m_hWnd, &m);

	m_pChangeBkImgBtn = (Button*)this->FindChildObject(_T("btn_change_bk_img"));
}
void CChatDlg::OnBnClickChangeBkImg()
{
	ReleaseCapture();
	CChatDlg    chatDlg;
	chatDlg.DoModal(_T("chatdlg"),m_hWnd);
	return;
	CFileDialog  dlg(TRUE);
	if (IDOK !=dlg.DoModal())
		return;

	String strFileName = dlg.m_szFileName;
	if (NULL != m_pForegndRender)
	{
	//	m_pForegndRender->Set
	}

}