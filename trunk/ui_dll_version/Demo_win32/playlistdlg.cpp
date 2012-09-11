#include "StdAfx.h"
#include "playlistdlg.h"
#include "player.h"
#include "PlayerListMgr.h"
#include "MainMgr.h"

//////////////////////////////////////////////////////////////////////////
TTPlayerPlaylistCtrl::TTPlayerPlaylistCtrl()
{
	m_pPlayingItem = NULL;
	m_pBkColor1 = NULL;
	m_pBkColor2 = NULL;
	m_pIndexTextColor = NULL;
	m_pIndexTextColorSelect = NULL;
	m_pIndexTextColorPlaying = NULL;
	m_pTextColor = NULL;
	m_pTextColorSelect = NULL;
	m_pTextColorPlaying = NULL;
	m_pTimeTextColor = NULL;
	m_pTimeTextColorSelect = NULL;
	m_pTimeTextColorPlaying = NULL;
	m_pSelectBorderColor = NULL;
	m_pSelectColor1 = NULL;
	m_pSelectColor2 = NULL;

}

TTPlayerPlaylistCtrl::~TTPlayerPlaylistCtrl()
{
	SAFE_RELEASE(m_pBkColor1);
	SAFE_RELEASE(m_pBkColor2);
	SAFE_RELEASE(m_pIndexTextColor);
	SAFE_RELEASE(m_pIndexTextColorSelect);
	SAFE_RELEASE(m_pIndexTextColorPlaying);
	SAFE_RELEASE(m_pTextColor);
	SAFE_RELEASE(m_pTextColorSelect);
	SAFE_RELEASE(m_pTextColorPlaying);
	SAFE_RELEASE(m_pTimeTextColor);
	SAFE_RELEASE(m_pTimeTextColorSelect);
	SAFE_RELEASE(m_pTimeTextColorPlaying);
	SAFE_RELEASE(m_pSelectBorderColor);
	SAFE_RELEASE(m_pSelectColor1);
	SAFE_RELEASE(m_pSelectColor2);

}

void TTPlayerPlaylistCtrl::ResetAttribute()
{
	__super::ResetAttribute();

	SAFE_RELEASE(m_pBkColor1);
	SAFE_RELEASE(m_pBkColor2);
	SAFE_RELEASE(m_pIndexTextColor);
	SAFE_RELEASE(m_pIndexTextColorSelect);
	SAFE_RELEASE(m_pIndexTextColorPlaying);
	SAFE_RELEASE(m_pTextColor);
	SAFE_RELEASE(m_pTextColorSelect);
	SAFE_RELEASE(m_pTextColorPlaying);
	SAFE_RELEASE(m_pTimeTextColor);
	SAFE_RELEASE(m_pTimeTextColorSelect);
	SAFE_RELEASE(m_pTimeTextColorPlaying);
	SAFE_RELEASE(m_pSelectBorderColor);
	SAFE_RELEASE(m_pSelectColor1);
	SAFE_RELEASE(m_pSelectColor2);

	m_MgrScrollbar.SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
	m_MgrScrollbar.SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);
}

bool TTPlayerPlaylistCtrl::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;
	
#define GET_COLOR(xml, p) \
	if (mapAttrib.count(xml)) \
	{ \
		const String& str = mapAttrib[xml]; \
		UI_GetColor(str, &p); \
		this->EraseAttribute(xml); \
	}

	GET_COLOR(XML_TTPLAYER_PLAYLIST_BKCOLOR1, m_pBkColor1);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_BKCOLOR2, m_pBkColor2);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR, m_pIndexTextColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR_SELECT, m_pIndexTextColorSelect);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR_PLAYING, m_pIndexTextColorPlaying);

	GET_COLOR(XML_TTPLAYER_PLAYLIST_TEXT_COLOR, m_pTextColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TEXT_COLOR_SELECT, m_pTextColorSelect);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TEXT_COLOR_PLAYING, m_pTextColorPlaying);

	GET_COLOR(XML_TTPLAYER_PLAYLIST_TIME_COLOR, m_pTimeTextColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TIME_COLOR_SELECT, m_pTimeTextColorSelect);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TIME_COLOR_PLAYING, m_pTimeTextColorPlaying);

	GET_COLOR(XML_TTPLAYER_PLAYLIST_SELECT_BORDER_COLOR, m_pSelectBorderColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_SELECT_COLOR1, m_pSelectColor1);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_SELECT_COLOR2, m_pSelectColor2);


	return true;
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
		if (NULL != m_pBkColor1)
			::FillRect(hRDC, &rcItem, m_pBkColor1->GetColor());
	}
	else
	{
		if (NULL != m_pBkColor2)
			::FillRect(hRDC, &rcItem, m_pBkColor2->GetColor());
	}

	COLORREF rgbText = m_pTextColor? m_pTextColor->GetColor():RGB(0,128,255);
	
// 	if (p->IsDisable())
// 	{
// 
// 	}
// 	else
	if( m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL )
	{
		COLORREF colBorder = m_pSelectBorderColor?m_pSelectBorderColor->GetColor():RGB(255,255,255);
		COLORREF colSelect1 = m_pSelectColor1?m_pSelectColor1->GetColor():RGB(47,100,190);
		COLORREF colSelect2 = m_pSelectColor2?m_pSelectColor2->GetColor():RGB(4,10,19);

		GradientFillV(hRDC, &rcItem, colSelect1, colSelect2);
		Rectangle(hRDC, &rcItem, colBorder, NULL, 1, true);

		rgbText = m_pTextColorSelect? m_pTextColorSelect->GetColor():RGB(255,255,255);
	}
	else if (m_pPlayingItem == p)  // 正在播放的歌曲
	{
		rgbText = m_pTextColorPlaying? m_pTextColorPlaying->GetColor() : RGB(0,255,0);
	}
	else if( m_pPressItem == p )
	{
	}
	else if( NULL == m_pPressItem && m_pHoverItem == p )
	{
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
	if (m_plistctrl)
	{
		m_plistctrl->GetScrollBarMgr()->SetVScrollLine(2);
	}
	UIASSERT(NULL != m_plistctrl);
}

void CPlayListDlg::OnClose()
{
	this->HideWindow();
	GetPlayerListMgr()->OnPlayListDlgHide();
}

void CPlayListDlg::OnBtnClickAdd(Object* pBtnObj, POINT* pt)
{
//	::MapWindowPoints( m_hWnd, NULL, pt, 1);
	Menu* pMenu = UI_LoadMenu(_T("menu_playlist_add"));
	if (NULL == pMenu)
		return;

	CRect rc;
	pBtnObj->GetWindowRect(&rc);
	::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rc, 2);
	UINT nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, this);
	SAFE_DELETE(pMenu);

// 	HMENU hMenu = ::CreatePopupMenu();
// 	AppendMenu( hMenu, MF_STRING, 100, _T("文件"));
// 	AppendMenu( hMenu, MF_STRING, 101, _T("文件夹"));
// 	AppendMenu( hMenu, MF_STRING, 102, _T("本地搜索"));
// 	AppendMenu( hMenu, MF_STRING, 103, _T("网络搜索"));
// 	
// 	UINT nRet = ::TrackPopupMenu(hMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, m_hWnd, NULL );
	switch(nRet)
	{
	case 1101:
		{
			CFileDialog dlg(TRUE, _T("*.mp3"), 0,4|2, _T("*.mp3\0*.mp3\0\0"));
			if(IDCANCEL != dlg.DoModal())
			{
				::GetPlayerListMgr()->AddFile(dlg.m_szFileName);
			}
		}
		break;

	case 1102:
		{
			CFolderDialog dlg;
			if( IDCANCEL != dlg.DoModal() )
			{
				::GetPlayerListMgr()->AddDirectory(dlg.m_szFolderPath);
			}
		}
		break;
	}
//	::DestroyMenu(hMenu);
}


void CPlayListDlg::OnBtnClickDel(Object* pBtnObj, POINT* pt)
{
	Menu* pMenu = UI_LoadMenu(_T("menu_playlist_delete"));
	if (NULL == pMenu)
		return;

	CRect rc;
	pBtnObj->GetWindowRect(&rc);
	::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rc, 2);
	UINT nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, this);
	SAFE_DELETE(pMenu);

	switch(nRet)
	{
	case 1204:
		{
			if(GetPlayerListMgr()->RemoveAllFile())
			{
				m_plistctrl->RemoveAllItem();
				m_plistctrl->UpdateObject();
				m_plistctrl->SetPlayingItem(NULL);
			}
		}
		break;
	}
}

void CPlayListDlg::OnBtnClickMode(Object* pBtnObj, POINT* pt)
{
	Menu* pMenu = UI_LoadMenu(_T("menu_playlist_mode"));
	if (NULL == pMenu)
		return;

	PLAY_MODE eMode = GetPlayerListMgr()->GetPlayMode();
	MenuItem* pMenuItem = pMenu->GetMenuItemByPos((int)eMode);
	if (NULL != pMenuItem)
	{
		pMenuItem->SetFlag(pMenuItem->GetFlag() | MF_CHECKED);
	}

	CRect rc;
	pBtnObj->GetWindowRect(&rc);
	::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rc, 2);
	UINT nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, this);
	SAFE_DELETE(pMenu);

	switch(nRet)
	{
	case 1701:
		{
			GetPlayerListMgr()->SetPlayMode(SINGLE_ONCE);
			break;
		}
		break;
	case 1702:
		{
			GetPlayerListMgr()->SetPlayMode(SINGLE_LOOP);
			break;
		}
		break;
	case 1703:
		{
			GetPlayerListMgr()->SetPlayMode(ALL_ONCE);
			break;
		}
		break;
	case 1704:
		{
			GetPlayerListMgr()->SetPlayMode(ALL_LOOP);
			break;
		}
		break;
	case 1705:
		{
			GetPlayerListMgr()->SetPlayMode(ALL_RAND);
			break;
		}
		break;
	}
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