#include "StdAfx.h"
#include "playlistdlg.h"

CPlayListDlg::CPlayListDlg(void)
{
	this->SetWindowResizeType(WRSB_ALL);
}

CPlayListDlg::~CPlayListDlg(void)
{
}

BOOL CPlayListDlg::PreCreateWindow( CREATESTRUCT& cs, DWORD& dwStyleEx )
{
	dwStyleEx &= ~WS_EX_APPWINDOW;
	dwStyleEx |= WS_EX_TOOLWINDOW;
	return __super::PreCreateWindow(cs,dwStyleEx);
}
 
int CPlayListDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

void CPlayListDlg::OnInitWindow( )
{
	Button* pBton = (Button*)this->FindChildObject(_T("sys_close"));
	if( NULL != pBton )
	{
//		this->AddToolTip(pBton);
	}
}

void CEqualizerDlg::OnInitWindow( )
{
	CustomWindow::OnInitWindow();

	SliderCtrl*  p = (SliderCtrl*)this->FindChildObject( _T("progress_hue1"));
	if( NULL != p )
	{
		p->SetRange(0,5);
		p->SetPos(3);
	}
	p = (SliderCtrl*)this->FindChildObject( _T("progress_hue2"));
	if( NULL != p )
	{
		p->SetRange(0,16);
		p->SetPos(8);
	}
}
void CEqualizerDlg::OnClose()
{
	this->HideWindow();
}
void CEqualizerDlg::OnHueChanged1( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
		UI_ChangeSkinH(nPos*5);
}
void CEqualizerDlg::OnHueChanged2( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
		UI_ChangeSkinH(100+nPos*5);
}

void CPlayListDlg::OnClose()
{
	this->HideWindow();
}

void CPlayListDlg::OnBtnClickAdd(Object* pBtnObj, POINT* pt)
{
	::MapWindowPoints( m_hWnd, NULL, pt, 1);
	HMENU hMenu = ::CreatePopupMenu();
	AppendMenu( hMenu, MF_STRING, 100, _T("文件"));
	AppendMenu( hMenu, MF_STRING, 101, _T("文件夹"));
	AppendMenu( hMenu, MF_STRING, 102, _T("本地搜索"));
	AppendMenu( hMenu, MF_STRING, 103, _T("网络搜索"));
	POINT ptMenu = pBtnObj->GetRealPosInWindow();
	::MapWindowPoints(m_hWnd, NULL, &ptMenu,1 );
	::TrackPopupMenu(hMenu, 0, ptMenu.x, ptMenu.y , 0, m_hWnd, NULL );
	::DestroyMenu(hMenu);
}

void CLyricDlg::OnPaint( HRDC hRDC )
{
	CRect rcClient;
	this->GetClientRect(&rcClient);

	rcClient.top += 20;
	DrawString(hRDC, _T("千千静听Demo v0.5.100"), &rcClient, DT_SINGLELINE|DT_VCENTER|DT_CENTER, m_hFont, RGB(84,142,165) );
}
void CLyricDlg::OnClose()
{
	this->HideWindow();
}

void CLyricDlg::OnSwitchLayered()
{
	if( IsWindowLayered() )
	{
		this->SetWindowTransparentMaskType(WINDOW_TRANSPARENT_TYPE_MASKALPHA);
		this->SetWindowTransparentAlphaMask(1);
		this->SetWindowLayered(false);
	}
	else
	{
		this->SetWindowTransparentMaskType(WINDOW_TRANSPARENT_TYPE_LAYERED|WINDOW_TRANSPARENT_TYPE_MASKALPHA);
		this->SetWindowTransparentAlphaMask(230);
		this->SetWindowLayered(true);
	}

	// 测试分层窗口的控件隐藏功能
// 	Object* p = FindChildObject(_T("sys_close"));
// 	if( NULL != p )
// 	{
// 		p->SetVisible(!p->IsVisible());
// 	}
}