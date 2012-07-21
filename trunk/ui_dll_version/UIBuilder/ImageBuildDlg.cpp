#include "StdAfx.h"
#include "ImageBuildDlg.h"
#include "Dialogs.h"


CImageDispalyView::CImageDispalyView()
{
	m_hBkBrush = NULL;
	m_pImage   = NULL;

	m_eDisplayType = IMAGE_DISPLAY_TYPE_CENTER_STRETCH;
}
CImageDispalyView::~CImageDispalyView()
{
	if( NULL != m_hBkBrush )
	{
		::DeleteObject( m_hBkBrush );
		m_hBkBrush = NULL;
	}
	if( NULL != m_pImage )
	{
		m_pImage->Release();
		m_pImage = NULL;
	}
}

int CImageDispalyView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HBITMAP hBitmap = ::LoadBitmap( _Module.m_hInst, MAKEINTRESOURCE(IDB_TRANSPARENT_BK) );
	if( NULL != hBitmap )
	{
		m_hBkBrush = ::CreatePatternBrush(hBitmap);
		::DeleteObject(hBitmap);
	}
	return 0;
}

void CImageDispalyView::Attach( UIImage* pImage )
{
	if( NULL != m_pImage )
		m_pImage->Release();

	m_pImage = pImage;

	this->Invalidate();
	this->UpdateWindow();

#if 0 // TODO
	if( m_strImagePath != strImagePath )
	{
		m_image.Release();
		m_strImagePath = strImagePath;
		m_image.SetPath( strImagePath );
		m_image.AddRef();

		this->Invalidate();
		this->UpdateWindow();
	}
#endif
	return;
}

void CImageDispalyView::OnPaint( HDC hDC )
{
	PAINTSTRUCT ps;
	hDC = this->BeginPaint(&ps);

	CRect rc;
	this->GetClientRect(&rc);

	HDC     hMemDC   = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap  = ::CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP hOldBmp  = (HBITMAP)::SelectObject(hMemDC,hBitmap);

	CDCHandle dc(hMemDC);
	DoPaint(dc);

	::BitBlt( hDC, 0,0, rc.Width(), rc.Height(), hMemDC, 0,0, SRCCOPY );
	::SelectObject(hMemDC,hOldBmp);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);

	this->EndPaint(&ps);
}

BOOL CImageDispalyView::OnEraseBkgnd( HDC hDC )
{

	return 1;
}

void CImageDispalyView::DoPaint(CDCHandle dc )
{
	CRect  rc;
	this->GetClientRect(&rc);

	// 透明背景
	HBRUSH hOldBrush = dc.SelectBrush(m_hBkBrush);
	dc.PatBlt( 0,0, rc.right, rc.bottom, PATCOPY );
	dc.SelectBrush(hOldBrush);


	// 黑色边框
	HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0,0,0) );
	POINT pts[] = 
	{
		{ 0, 0 },
		{ rc.right-1, 0 },
		{ rc.right-1, rc.bottom-1 },
		{ 0, rc.bottom-1 },
		{ 0, 0 },
	};

	HPEN hOldPen = dc.SelectPen( hPen );
	dc.Polyline( pts, 5 );
	dc.SelectPen(hOldPen);
	::DeleteObject( hPen );

	if( NULL == m_pImage )
		return;

	switch(m_eDisplayType)
	{
	case IMAGE_DISPLAY_TYPE_CENTER_BITBLT:
		m_pImage->Draw(dc.m_hDC, (rc.Width()-m_pImage->GetWidth())/2, (rc.Height()-m_pImage->GetHeight())/2 );
		break;
	case IMAGE_DISPLAY_TYPE_CENTER_STRETCH:
		{
			::SetStretchBltMode(dc.m_hDC, HALFTONE);
			double tan_x_y_image  = 0;
			double tan_x_y_window = 0;

			bool bNeedToStretch = false;
			int  xImage = m_pImage->GetWidth();
			int  yImage = m_pImage->GetHeight();

			if( rc.Width() < m_pImage->GetWidth() || rc.Height() < m_pImage->GetHeight() )
			{
				bNeedToStretch = true;

				tan_x_y_image = (double)m_pImage->GetWidth() / (double)m_pImage->GetHeight();
				tan_x_y_window = (double)rc.Width() / (double)rc.Height();

				if( tan_x_y_image > tan_x_y_window ) // 横向占满
				{
					xImage = rc.Width();
					yImage = (int)((double)xImage/tan_x_y_image);
				}
				else   // 纵向占满
				{
					yImage = rc.Height();
					xImage = (int)(yImage*tan_x_y_image);
				}
			}

			// 计算图片显示位置
			int xDisplayPos = (rc.Width()-xImage)/2;
			int yDisplayPos = (rc.Height()-yImage)/2;

			if( bNeedToStretch )
			{
				m_pImage->Draw(dc.m_hDC, xDisplayPos, yDisplayPos, xImage, yImage, 0,0, m_pImage->GetWidth(), m_pImage->GetHeight() );
			}
			else
			{
				m_pImage->Draw(dc.m_hDC, xDisplayPos, yDisplayPos );
			}
		}
		break;
	default:
		m_pImage->BitBlt(dc.m_hDC,0,0);
		break;
	}
}

CImageViewDlg::CImageViewDlg()
{
	m_hSkin = NULL;
	m_pImageDisplayView = NULL;
}
CImageViewDlg::~CImageViewDlg()
{
	m_hSkin = NULL;
	if( NULL != m_pImageDisplayView )
	{
		delete m_pImageDisplayView;
	}
	m_pImageDisplayView = NULL;
}


BOOL CImageViewDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_pImageDisplayView = new CImageDispalyView;
	m_pImageDisplayView->Create( m_hWnd );
	m_pImageDisplayView->ShowWindow(SW_SHOW);

	m_listctrl.Attach( GetDlgItem(IDC_FILELIST) );
	m_listctrl.InsertColumn( 0, _T("id"), LVCFMT_LEFT, 120 );
	m_listctrl.InsertColumn( 1, _T("path"), LVCFMT_LEFT, 400 );

	m_listctrl.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT );
	m_listctrl.ModifyStyle( 0, LVS_SINGLESEL|LVS_SHOWSELALWAYS );

	return TRUE;
}

//
//	设置数据源
//
void CImageViewDlg::AttachSkin( HSKIN h )
{
	if( m_hSkin == h )
		return;

	// changed notify
	m_hSkin = h;

	Reload();
}

bool CImageViewDlg::Reload()
{
	if( NULL == m_hSkin )
		return false;

	// 获取该皮肤中的所有图片
	m_listctrl.DeleteAllItems();

	int nCount = ::UI_GetSkinImageCount( m_hSkin );
	for( int i = 0; i < nCount; i++ )
	{
		IImageItemInfo*  pInfo = NULL;
		if( ::UI_GetSkinImageItemInfo( m_hSkin, i, &pInfo ) && pInfo!=NULL )
		{
			int nItemCount = m_listctrl.GetItemCount();
			m_listctrl.InsertItem( nItemCount, pInfo->GetIDRef().c_str() );
			m_listctrl.SetItemText( nItemCount, 1, pInfo->GetPathRef().c_str() );
		}
		if( NULL != pInfo )
			pInfo->Release();
	}

	m_pImageDisplayView->Attach(NULL);

	// 将新增的图片显示出来
	m_listctrl.SelectItem(0);

	return true;
}

void CImageViewDlg::OnSize(UINT nType, CSize size)
{
	int x = WINDOW_PADDING;/*(size.cx - 4*WIDTH_TOOLBAR_BTN - 3*WINDOW_PADDING - WIDTH_IMAGEBUILD_LISTCTRL )/2*/
	GetDlgItem(IDC_BUTTON_ADD).SetWindowPos    ( NULL, x,                                  WINDOW_PADDING, WIDTH_TOOLBAR_BTN, HEIGHT_TOOLBAR_BTN, SWP_NOZORDER );
	GetDlgItem(IDC_BUTTON_DELETE).SetWindowPos ( NULL, x + WIDTH_TOOLBAR_BTN   + WINDOW_PADDING,   WINDOW_PADDING, WIDTH_TOOLBAR_BTN, HEIGHT_TOOLBAR_BTN, SWP_NOZORDER );
	GetDlgItem(IDC_BUTTON_LEFT).SetWindowPos   ( NULL, x + 2*WIDTH_TOOLBAR_BTN + 2*WINDOW_PADDING, WINDOW_PADDING, WIDTH_TOOLBAR_BTN, HEIGHT_TOOLBAR_BTN, SWP_NOZORDER );
	GetDlgItem(IDC_BUTTON_RIGHT).SetWindowPos  ( NULL, x + 3*WIDTH_TOOLBAR_BTN + 3*WINDOW_PADDING, WINDOW_PADDING, WIDTH_TOOLBAR_BTN, HEIGHT_TOOLBAR_BTN, SWP_NOZORDER );

	m_listctrl.SetWindowPos( 0, size.cx-WIDTH_IMAGEBUILD_LISTCTRL-WINDOW_PADDING, 2*WINDOW_PADDING+HEIGHT_TOOLBAR_BTN, WIDTH_IMAGEBUILD_LISTCTRL,size.cy-HEIGHT_TOOLBAR_BTN-3*WINDOW_PADDING,SWP_NOZORDER );

	if( NULL != m_pImageDisplayView )
	{
		m_pImageDisplayView->SetWindowPos( NULL, WINDOW_PADDING, 2*WINDOW_PADDING+HEIGHT_TOOLBAR_BTN, size.cx - WIDTH_IMAGEBUILD_LISTCTRL-3*WINDOW_PADDING, size.cy-HEIGHT_TOOLBAR_BTN-3*WINDOW_PADDING, SWP_NOZORDER );
	}
}


void CImageViewDlg::OnBtnAdd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if( NULL == m_hSkin )
		return;

	CNewImageDlg  dlg(m_hSkin);
	if( IDOK == dlg.DoModal() )
	{
		int nCount = m_listctrl.GetItemCount();
		m_listctrl.InsertItem( nCount, dlg.m_strId.c_str() );
		m_listctrl.SetItemText( nCount, 1, dlg.m_strPath.c_str() );

		// 将新增的图片显示出来
		m_listctrl.SelectItem(nCount);

		this->SET_DIRTY3(true);
	}



}
void CImageViewDlg::OnBtnRemove(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// 1. 删除资源 

	int nIndex = this->m_listctrl.GetSelectedIndex();
	if( -1 == nIndex )
		return;

	m_pImageDisplayView->Attach(NULL);  // 必须在UI_RemoveImageItem前进行释放

	TCHAR szText[MAX_STRING_LEN] = _T("");
	m_listctrl.GetItemText(nIndex, 0, szText, MAX_STRING_LEN );

	if( false == ::UI_RemoveImageItem( m_hSkin, szText ) )
	{
		::MessageBox( NULL, _T("删除失败"), _T("Error"), MB_OK|MB_ICONERROR	);
		return;
	}

	this->m_listctrl.DeleteItem(nIndex);

	// 更新界面
	int nCount = m_listctrl.GetItemCount();
	if( nIndex >= nCount-1 )
	{
		nIndex = nCount-1;
	}
	this->m_listctrl.SelectItem(nIndex);

	this->SET_DIRTY3(true);
}
void CImageViewDlg::OnBtnRight(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCount = m_listctrl.GetItemCount();
	if( 0 == nCount )
		return;

	int nIndex = m_listctrl.GetSelectedIndex();
	if( -1 == nIndex )
		nIndex = 0;
	else
		nIndex++;

	if( nIndex >= nCount )
		nIndex = nCount-1;

	m_listctrl.SelectItem(nIndex);
}
void CImageViewDlg::OnBtnLeft(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCount = m_listctrl.GetItemCount();
	if( 0 == nCount )
		return;

	int nIndex = m_listctrl.GetSelectedIndex();
	nIndex--;

	if( nIndex <= 0 )
		nIndex = 0;

	m_listctrl.SelectItem(nIndex);
}

//
//	显示当前被选中的图片
//
LRESULT  CImageViewDlg::OnItemChanged(LPNMHDR pnmh)
{
	LPNMLISTVIEW lpnmh = (LPNMLISTVIEW)pnmh;
	if( NULL == m_pImageDisplayView )
		return 0;

	if( !(lpnmh->uOldState & LVIS_SELECTED ) &&
		( lpnmh->uNewState & LVIS_SELECTED ) )
	{
		int nIndex = m_listctrl.GetSelectedIndex();
		TCHAR szImageName[MAX_STRING_LEN] = _T("");
		m_listctrl.GetItemText( nIndex, 0, szImageName, MAX_STRING_LEN );

		String strImageName(szImageName);

		UIImage* pImage = NULL;
		::UI_GetImage( strImageName, &pImage, m_hSkin );

		m_pImageDisplayView->Attach( pImage );
		pImage = NULL;
	}
	return 0;
}

LRESULT  CImageViewDlg::OnItemDBClick(LPNMHDR pnmh)
{
	LRESULT lret = this->DefWindowProc();

	LPNMITEMACTIVATE p = (LPNMITEMACTIVATE )pnmh;
	if( -1 == p->iItem )
		return lret;

	TCHAR szImageName[MAX_STRING_LEN] = _T("");
	TCHAR szImagePath[MAX_PATH] = _T("");
	m_listctrl.GetItemText( p->iItem, 0, szImageName, MAX_STRING_LEN );
	m_listctrl.GetItemText( p->iItem, 1, szImagePath, MAX_PATH );

	CNewImageDlg dlg(m_hSkin,true);
	dlg.SetImageInfo(szImageName,szImagePath);
	if( IDOK == dlg.DoModal() )
	{
		m_listctrl.SetItemText( p->iItem, 1, dlg.m_strPath.c_str() );

		m_pImageDisplayView->Invalidate(TRUE);
		m_pImageDisplayView->UpdateWindow();

		this->SET_DIRTY3(true);
	}

	return 0;
}




//////////////////////////////////////////////////////////////////////////

CImageCodeDlg::CImageCodeDlg() : CResourceCodeDlg(UI_RESOURCE_IMAGE)
{
}
CImageCodeDlg::~CImageCodeDlg()
{
}





CImageBuildDlg::CImageBuildDlg()
{
	m_pImageViewPanel = NULL;
	m_pImageCodePanel = NULL;
	m_hSkin = NULL;
}

CImageBuildDlg::~CImageBuildDlg()
{
	if( NULL != m_pImageViewPanel )
	{
		delete m_pImageViewPanel;
		m_pImageViewPanel = NULL;
	}
	if( NULL != m_pImageCodePanel )
	{
		delete m_pImageCodePanel;
		m_pImageCodePanel = NULL;
	}
	m_hSkin = NULL;
}

BOOL CImageBuildDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_tabCtrl.Attach(GetDlgItem(IDC_TAB));
	m_tabCtrl.InsertItem(0, _T("view") );
	m_tabCtrl.InsertItem(1, _T("code") );

	m_pImageViewPanel = new CImageViewDlg;
	m_pImageViewPanel->Create(m_tabCtrl.m_hWnd);
	m_pImageCodePanel = new CImageCodeDlg;
	m_pImageCodePanel->Create(m_tabCtrl.m_hWnd);

	m_pImageViewPanel->ShowWindow(SW_SHOW);
	return TRUE;
}
void CImageBuildDlg::OnSize(UINT nType, CSize size)
{
	if (NULL == m_pImageViewPanel || NULL == m_pImageCodePanel )
		return;

	RECT rc = {WINDOW_PADDING,WINDOW_PADDING,size.cx-WINDOW_PADDING,size.cy-WINDOW_PADDING};
	m_tabCtrl.SetWindowPos(NULL,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
	m_tabCtrl.ModifyStyle(0,WS_CLIPCHILDREN);
	
	RECT rcItem;
	m_tabCtrl.GetItemRect(0,&rcItem);

	m_tabCtrl.GetClientRect(&rc);
//	::InflateRect(&rc, -WINDOW_PADDING, -WINDOW_PADDING );
	rc.bottom -= Util::RectH(&rcItem);

	m_pImageViewPanel->SetWindowPos(0,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
	m_pImageCodePanel->SetWindowPos(0,rc.left,rc.top,Util::RectW(&rc),Util::RectH(&rc),SWP_NOZORDER);
}

LRESULT CImageBuildDlg::OnSelChanging(LPNMHDR pnmh)
{	
	if( NULL == m_pImageCodePanel || NULL == m_pImageViewPanel )
		return 0;

	int nSel = m_tabCtrl.GetCurSel();
	switch(nSel)
	{
	case 1:
		m_pImageCodePanel->Commit_2_TempFile();
		if( false == UI_ReloadResource(m_hSkin, UI_RESOURCE_IMAGE) )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}

		m_pImageViewPanel->ShowWindow(SW_SHOW);	
		m_pImageCodePanel->ShowWindow(SW_HIDE);
		m_pImageViewPanel->Reload();

		break;

	case 0:

		if( false == m_pImageCodePanel->Update_From_TempFile() )
		{
			::MessageBox(NULL, _T("更新失败"), _T("Error"), MB_OK|MB_ICONERROR);
			return TRUE;
		}
		m_pImageCodePanel->ShowWindow(SW_SHOW);
		m_pImageViewPanel->ShowWindow(SW_HIDE);	
		m_pImageViewPanel->GetImageDisplayView()->Attach(NULL);
		break;
	}
	return 0;
}

void CImageBuildDlg::AttachSkin( HSKIN hSkin )
{
	if(NULL == m_pImageViewPanel)
		return;

	m_hSkin = hSkin;
	m_pImageViewPanel->AttachSkin(hSkin);
	m_pImageCodePanel->AttachSkin(hSkin);
}

