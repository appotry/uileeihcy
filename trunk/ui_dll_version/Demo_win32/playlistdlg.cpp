#include "StdAfx.h"
#include "playlistdlg.h"




//////////////////////////////////////////////////////////////////////////


void TTPlayerPlaylistCtrl::ResetAttribute()
{
	__super::ResetAttribute();
	m_MgrScrollbar.SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
	m_MgrScrollbar.SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);
}

bool TTPlayerPlaylistCtrl::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	return __super::SetAttribute(mapAttrib, bReload);
}
void TTPlayerPlaylistCtrl::OnDrawItem(HRDC hRDC, ListItemBase* p)
{
	if (NULL == p)
		return;
	TTPlayerPlaylistItem* pData = (TTPlayerPlaylistItem*)p;

	CRect rcItem;
	p->GetParentRect(&rcItem);

	if ( 0 == p->GetLineIndex()%2 )
	{
		::FillRect(hRDC, &rcItem, RGB(0,0,0));
	}
	else
	{
		::FillRect(hRDC, &rcItem, RGB(32,32,32));
	}

	COLORREF rgbText = RGB(0,128,255);
	if (0/*NULL != m_pForegndRender*/)
	{
		if (p->IsDisable())
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE);
		}
		else if( m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED);
		}
		else if( m_pPressItem == p )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS);
		}
		else if( NULL == m_pPressItem && m_pHoverItem == p )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER);
		}
	}
	else
	{
		if (p->IsDisable())
		{

		}
		else if( m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL )
		{
			GradientFillV(hRDC, &rcItem, RGB(47,100,190), RGB(4,10,19));
			Rectangle(hRDC, &rcItem, RGB(255,255,255), NULL, 1, true);

			rgbText = RGB(255,255,255);
		}
		else if( m_pPressItem == p )
		{
		}
		else if( NULL == m_pPressItem && m_pHoverItem == p )
		{
		}
	}

	if (NULL != pData)
	{
		CRect rcNum = rcItem;
		CRect rcTime = rcItem;
		CRect rcText = rcItem;

		rcText.left = rcNum.right = 20;
		rcText.right = rcTime.left = rcItem.right - 25;
		rcTime.right--;

		TCHAR szNum[16] = _T("");
		_stprintf(szNum, _T("%d."), p->GetLineIndex()+1);
		DrawString( hRDC, szNum, &rcNum, 
			DT_SINGLELINE|DT_RIGHT|DT_VCENTER, 
			this->GetFont(), rgbText );

		DrawString( hRDC, pData->m_strFileName.c_str(), &rcText, 
			DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER, 
			this->GetFont(), rgbText );

		DrawString( hRDC, pData->m_strFileTime.c_str(), &rcTime, 
			DT_SINGLELINE|DT_RIGHT|DT_VCENTER, 
			this->GetFont(), rgbText );
	}
}
void TTPlayerPlaylistCtrl::AddFileItem(const String& strFilePath, bool bUpdate)
{
	int nPos = strFilePath.rfind(_T('\\'), strFilePath.length());
	TTPlayerPlaylistItem* pItem = new TTPlayerPlaylistItem(this);
	pItem->m_strFilePath = strFilePath;
	pItem->m_strFileName = strFilePath.substr(nPos+1, strFilePath.length()-nPos-1);
	pItem->m_strFileTime = _T("3:25");

	__super::AddItem( pItem, bUpdate );
}

SIZE TTPlayerPlaylistCtrl::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,20};
	return s;
}
void TTPlayerPlaylistCtrl::OnDeleteItem( ListItemBase* p )
{
}


CPlayListDlg::CPlayListDlg(void)
{
	this->SetWindowResizeType(WRSB_ALL);
	m_plistctrl = NULL;
}

CPlayListDlg::~CPlayListDlg(void)
{
}

BOOL CPlayListDlg::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.dwExStyle &= ~WS_EX_APPWINDOW;
	cs.dwExStyle |= WS_EX_TOOLWINDOW;
	return __super::PreCreateWindow(cs);
}
 
int CPlayListDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	return 0;
}

void CPlayListDlg::OnInitWindow( )
{
	m_plistctrl = (TTPlayerPlaylistCtrl*)this->FindChildObject(_T("playlistctrl"));
	UIASSERT(NULL != m_plistctrl);
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
	
	CRect rc;
	pBtnObj->GetWindowRect(&rc);
	::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rc, 2);
	UINT nRet = ::TrackPopupMenu(hMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, m_hWnd, NULL );
	switch(nRet)
	{
	case 100:
		{
			CFileDialog dlg(TRUE, _T("*.mp3"), 0,4|2, _T("*.mp3\0*.mp3\0\0"));
			if(IDCANCEL != dlg.DoModal())
			{
				this->AddFile(dlg.m_szFileName);
			}
		}
		break;

	case 101:
		{
			CFolderDialog dlg;
			if( IDCANCEL != dlg.DoModal() )
			{
				this->AddDirectory(dlg.m_szFolderPath);
			}
		}
		break;
	}
	::DestroyMenu(hMenu);
}

void CPlayListDlg::AddFile(const String& strFile)
{
	if (NULL == m_plistctrl)
		return;

	m_plistctrl->AddFileItem(strFile);
}

bool CALLBACK MyEnumFileInDirProc(const TCHAR* szDir, const TCHAR* szFileName, WPARAM wParam)
{
	CPlayListDlg* pThis = (CPlayListDlg*)wParam;
	if (NULL == pThis)
		return false;

	String str = szDir;
	str += szFileName;
	if( str.substr(str.length()-4,4) == _T(".mp3") )
	{
		pThis->AddFile(str);
	}
	return true;
}
void CPlayListDlg::AddDirectory(const String& strDir)
{
	if (NULL == m_plistctrl)
		return;

	Util::EnumFileInDirectory(strDir.c_str(), MyEnumFileInDirProc, (WPARAM)this);
}
