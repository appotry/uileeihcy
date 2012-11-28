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
	CFileDialog  dlg(TRUE, _T("*.png"), 0,4|2, _T("*.png\0*.png\0*.jpg\0*.jpg\0*.bmp\0*.bmp\0*.*\0*.*\0\0"));
	if (IDOK !=dlg.DoModal())
		return;

	String strFileName = dlg.m_szFileName;
//	SAFE_DELETE(m_pForegndRender);

	UI_ModifyImageItemInRunTime(_T("chatframe_surface"), strFileName);
	
	IRenderBitmap* pRenderBitmap = UI_GetBitmap(_T("chatframe_surface"));
	COLORREF avgColor = pRenderBitmap->GetAverageColor();

	UIColor* pColor = NULL;
	UI_GetColor(_T("avg_surface_color"), &pColor);
	if (NULL != pColor)
	{
		pColor->SetColor(avgColor);
	}

	UI_RedrawTopWindows();
//  RenderBase* pRender = RenderFactory::GetRender(RENDER_TYPE_IMAGE, this);
// 	ImageRender* pImageRender = dynamic_cast<ImageRender*>(pRender);
// 	if (NULL == pImageRender)
// 	{
// 		SAFE_DELETE(pRender);
// 		return;
// 	}
// 	pImageRender->SetBkColorDirect(RGB(255,255,255));
// 
// 	UI_InsertImageItem(NULL, strFileName, strFileName);
// 	pImageRender->SetRenderBitmapDirect(::UI_GetBitmap(strFileName, ::GetGraphicsRenderType(this)));
// 	
// 	m_pForegndRender = pRender;
// 	this->UpdateObject();
}
