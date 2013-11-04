#include "stdafx.h"
#include "tooltipwindow.h"
#include <Uxtheme.h>

#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"

TooltipWindow::TooltipWindow()
{
	m_nLimitWidth = -1;
}
TooltipWindow::~TooltipWindow()
{
	this->Destroy();
}

BOOL TooltipWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    if (FALSE == GetCurMsg()->lRet)
        return FALSE;

	pcs->lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;  // 带阴影
	pcs->dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE;
	return TRUE;
}

void TooltipWindow::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	this->SetWindowLayered(true);  // 先于__super::SetAttribute，指定创建Gdiplus类型资源

	__super::SetAttribute(pMapAttrib, bReload);

	if (NULL == m_pBkgndRender)
	{
		m_pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_TOOLTIP_WINDOW_BKGND, this->GetIObject(), &m_pBkgndRender);
	}

	CRegion4 b(1,1,1,1);
	this->SetBorderRegion(&b);
	CRegion4 r(4,3,4,3);
	this->SetPaddingRegion(&r);      // 文字与边缘的间距

	this->SetWindowResizeType(WRSB_NONE);  // 禁止拖拽

	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE|SWP_FRAMECHANGED);
}

void TooltipWindow::OnPaint(IRenderTarget* pRenderTarget)
{
	CRect rc;
	this->GetClientRectAsWin32(&rc);

    DRAWTEXTPARAM  param;
	param.color.ReplaceRGB(GetSysColor(COLOR_INFOTEXT));
	if (IsThemeActive())
	{
		param.color.ReplaceRGB(RGB(87,87,87));  // TODO: GetSysColor(COLOR_INFOTEXT)拿不到WIN7主题下面提示条字体颜色
	}
    param.szText = m_strText.c_str();
    param.prc = &rc;
	pRenderTarget->DrawString(m_pDefaultFont, &param);
}

bool TooltipWindow::Create()
{
// 	if (m_hWnd)
// 		return false;
// 
// 	CustomWindow::Create(_T(""),NULL);
// 
// 	this->SetWindowLayered(true);
// 
// 	ATTRMAP map;
// 	this->SetAttribute(map, false);  // 创建一些默认属性
// 
// 	CRegion4 b(1,1,1,1);
// 	this->SetBorderRegion(&b);
// 	CRegion4 r(4,3,4,3);
// 	this->SetPaddingRegion(&r);      // 文字与边缘的间距
// 
// 	this->SetWindowResizeType(WRSB_NONE);  // 禁止拖拽
// 
// 	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE|SWP_FRAMECHANGED);
	return true;
}
bool TooltipWindow::Destroy() 
{
	if (NULL == m_hWnd)
		return false;

	::DestroyWindow(m_hWnd);
	return true;
}

 void TooltipWindow::OnFinalMessage()
 {
 	delete this;
 }

bool TooltipWindow::SetText(const String& strText)
{
	if (NULL == m_hWnd)
	{
		Create();
	}
	m_strText = strText;

	// 计算窗口的新大小
	SIZE s = {0,0};
	IRenderFont* pFont = this->GetRenderFont();
	if (pFont)
	{
		s= pFont->MeasureString(strText.c_str(), m_nLimitWidth);
	}
	s.cx += this->GetNonClientW();
	s.cy += this->GetNonClientH();

	this->SetObjectPos(0,0, s.cx,s.cy, SWP_NOMOVE);
	return true;
}
bool TooltipWindow::SetTitle(const String& strText)
{
	if (NULL == m_hWnd)
	{
		Create();
	}
	return false;
}
bool TooltipWindow::Show()
{
	POINT pt;
	::GetCursorPos(&pt);
	this->SetObjectPos(pt.x, pt.y+22, 0,0, SWP_NOSIZE);

	//::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	return true;
}
bool TooltipWindow::Hide() 
{
	this->HideWindow();
	return true;
}
void TooltipWindow::SetAttribute(const ATTRMAP& mapAttrib) 
{
}