#include "StdAfx.h"
#include "ChatDlg.h"

CChatDlg::CChatDlg(void)
{
	m_pAlphaSlider = NULL;
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
	m_pAlphaSlider = (SliderCtrl*)this->FindChildObject(_T("alpha_slider"));
	if (NULL != m_pAlphaSlider)
	{
		m_pAlphaSlider->SetPos(90, false);

		OnAlphaChanged(90, SB_ENDSCROLL);
	}
}
void CChatDlg::OnBnClickChangeBkImg()
{
	CFileDialog  dlg(TRUE, _T("*.png"), 0,4|2, _T("*.png\0*.png\0*.jpg\0*.jpg\0*.bmp\0*.bmp\0*.*\0*.*\0\0"));
	if (IDOK !=dlg.DoModal())
		return;

	String strFileName = dlg.m_szFileName;

	UI_ModifyImageItemInRunTime(_T("chatframe_surface"), strFileName);
	
	// 求出图片平均色。注：QQ在这里对替换的图片还做了渐变处理，使得图片边缘与背景色连接融洽
	IRenderBitmap* pRenderBitmap = UI_GetBitmap(_T("chatframe_surface"));
	COLORREF avgColor = 0;
	if (NULL != pRenderBitmap)
	{
		avgColor = pRenderBitmap->GetAverageColor();
		SAFE_RELEASE(pRenderBitmap);
	}

	UIColor* pColor = NULL;
	UI_GetColor(_T("avg_surface_color"), &pColor);
	if (NULL != pColor)
	{
		pColor->SetColor(avgColor);
		SAFE_RELEASE(pColor);
	}

	UI_RedrawTopWindows();
}

void CChatDlg::OnAlphaChanged(int nPos, int nScrollType)
{
	if (nScrollType == SB_ENDSCROLL)
	{
		UI_ModifyImageItemAlpha(_T("chatframe.png"), nPos);
		UI_RedrawTopWindows();
	}
}