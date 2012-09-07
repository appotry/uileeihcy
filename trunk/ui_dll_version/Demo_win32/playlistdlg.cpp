#include "StdAfx.h"
#include "playlistdlg.h"
#include "player.h"
#include "PlayerListMgr.h"
#include "MainMgr.h"

//////////////////////////////////////////////////////////////////////////
TTPlayerPlaylistCtrl::TTPlayerPlaylistCtrl()
{
	m_pPlayingItem = NULL;
}

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

void TTPlayerPlaylistCtrl::SetPlayingItem(TTPlayerPlaylistItem* pCtrlItem)
{
	bool bNeedUpdate = false;
	if (m_pPlayingItem != pCtrlItem)
	{
		bNeedUpdate = true;
	}

	m_pPlayingItem = pCtrlItem;
	this->MakeItemVisible(pCtrlItem, bNeedUpdate);	

	if (bNeedUpdate)
	{
		this->UpdateObject();
	}
}

void TTPlayerPlaylistCtrl::OnDrawItem(HRDC hRDC, ListItemBase* p)
{
	if (NULL == p)
		return;
	TTPlayerPlaylistItem* pItem = (TTPlayerPlaylistItem*)p;

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
		else if (m_pPlayingItem == p)  // 正在播放的歌曲
		{
			rgbText = RGB(0,255,0);
		}
		else if( m_pPressItem == p )
		{
		}
		else if( NULL == m_pPressItem && m_pHoverItem == p )
		{
		}
	}

	if (NULL != pItem && NULL != pItem->m_pItemInfo)
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

		String strText;
		this->BuildItemText(pItem, strText);
		DrawString( hRDC, strText.c_str(), &rcText, 
			DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER, 
			this->GetFont(), rgbText );

		DrawString( hRDC, pItem->m_pItemInfo->GetFileTime().c_str(), &rcTime, 
			DT_SINGLELINE|DT_RIGHT|DT_VCENTER, 
			this->GetFont(), rgbText );
	}
}
void TTPlayerPlaylistCtrl::BuildItemText(TTPlayerPlaylistItem* pCtrlItem, String& strOut)
{
	strOut = pCtrlItem->m_pItemInfo->GetFileName();
}

void TTPlayerPlaylistCtrl::AddFileItem(PlayerListItemInfo* pItemInfo, bool bUpdate)
{
//	int nPos = strFilePath.rfind(_T('\\'), strFilePath.length());
	TTPlayerPlaylistItem* pItem = new TTPlayerPlaylistItem(this);
	pItem->m_pItemInfo = pItemInfo;
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


CPlayListDlg::CPlayListDlg(CPlayerListMgr* pPlayerListMgr)
{
	m_pPlayerListMgr = pPlayerListMgr;
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
	m_plistctrl->GetScrollBarMgr()->SetVScrollLine(2);
	UIASSERT(NULL != m_plistctrl);
}

void CPlayListDlg::OnClose()
{
	this->HideWindow();
	GetPlayerListMgr()->OnPlayListDlgHide();
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
				::GetPlayerListMgr()->AddFile(dlg.m_szFileName);
			}
		}
		break;

	case 101:
		{
			CFolderDialog dlg;
			if( IDCANCEL != dlg.DoModal() )
			{
				::GetPlayerListMgr()->AddDirectory(dlg.m_szFolderPath);
			}
		}
		break;
	}
	::DestroyMenu(hMenu);
}

void CPlayListDlg::OnLCNDbclick(POINT pt, ListItemBase* pItem)
{
	if (NULL == pItem)
		return;

	TTPlayerPlaylistItem* pPlayItem = dynamic_cast<TTPlayerPlaylistItem*>(pItem);
	if (NULL == pPlayItem)
		return;

	GetMainMgr()->Play(pPlayItem->m_pItemInfo);
}

void CPlayListDlg::OnAddItem(PlayerListItemInfo* pItemInfo)
{
	if (NULL == m_plistctrl)
		return;

	m_plistctrl->AddFileItem(pItemInfo);
}

TTPlayerPlaylistItem* CPlayListDlg::GetCtrlItemByPlayItem(PlayerListItemInfo* p)
{
	if (NULL == p || NULL == m_plistctrl)
		return NULL;

	ListItemBase*  pItem = m_plistctrl->GetFirstItem();
	while (NULL != pItem)
	{
		TTPlayerPlaylistItem* pCtrlItem = (TTPlayerPlaylistItem*)pItem;
		if (pCtrlItem->m_pItemInfo == p)
		{
			return pCtrlItem;
		}
		pItem = pItem->GetNextItem();
	}
	return NULL;
}
void CPlayListDlg::OnMp3Start(PlayerListItemInfo* pItemInfo)
{
	if (NULL == m_plistctrl)
		return;

	TTPlayerPlaylistItem* pCtrlItem = this->GetCtrlItemByPlayItem(pItemInfo);
	if (NULL == pCtrlItem)
		return;

	m_plistctrl->SetPlayingItem(pCtrlItem);
}