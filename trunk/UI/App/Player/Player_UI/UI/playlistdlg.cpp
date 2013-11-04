#include "StdAfx.h"
#include "App\Player\Player_UI\UI\playlistdlg.h"
#include "App\Player\Player_UI\Mgr\player.h"
#include "App\Player\Player_UI\Mgr\PlayerListMgr.h"
#include "App\Player\Player_UI\Mgr\MainMgr.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
CPlayListDlg::CPlayListDlg()
{
	m_pPlayerListMgr = NULL;
	m_plistctrl = NULL;
}

CPlayListDlg::~CPlayListDlg(void)
{
}

BOOL CPlayListDlg::PreCreateWindow(CREATESTRUCT* pcs)
{
    SetMsgHandled(FALSE);
	pcs->dwExStyle &= ~WS_EX_APPWINDOW;
	pcs->dwExStyle |= WS_EX_TOOLWINDOW;
	
    return TRUE;
}
 
int CPlayListDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

void CPlayListDlg::OnInitWindow( )
{
	m_plistctrl = (ITTPlayerPlaylistCtrl*)this->FindChildObject(_T("playlistctrl"));
    this->SetWindowResizeType(WRSB_ALL);
}

void CPlayListDlg::OnClose()
{
  	this->HideWindow();
  	GetPlayerListMgr()->OnPlayListDlgHide();
}

void CPlayListDlg::OnBtnClickAdd(IObject* pBtnObj, POINT* pt)
{
    UI::IMenu* pMenu = NULL;
    UI::UILoadMenu(GetUIApplication(), _T("menu_playlist_add"), &pMenu);
    if (NULL == pMenu)
        return;

	CRect rc;
	pBtnObj->GetWindowRect(&rc);
	::MapWindowPoints(GetHWND(), NULL, (LPPOINT)&rc, 2);

	UINT nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, this, GetHWND(), &rc);
	SAFE_DELETE_Ixxx(pMenu);

// 	HMENU hMenu = ::CreatePopupMenu();
// 	AppendMenu( hMenu, MF_STRING, 100, _T("�ļ�"));
// 	AppendMenu( hMenu, MF_STRING, 101, _T("�ļ���"));
// 	AppendMenu( hMenu, MF_STRING, 102, _T("��������"));
// 	AppendMenu( hMenu, MF_STRING, 103, _T("��������"));
// 	
// 	UINT nRet = ::TrackPopupMenu(hMenu, TPM_RETURNCMD, rc.left, rc.bottom, 0, m_hWnd, NULL );
	switch(nRet)
	{
	case 1101:
		{
			CFileDialog dlg(TRUE, _T("*.mp3"), 0,4|2, _T("*.mp3\0*.mp3\0*.wma\0*.wma\0*.wav\0*.wav\0*.mp3;*.wma;*.wav;\0*.wma;*.mp3;*.wav\0*.*\0*.*\0\0"));
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


void CPlayListDlg::OnBtnClickDel(IObject* pBtnObj, POINT* pt)
{
    UI::IMenu* pMenu = NULL;
    UI::UILoadMenu(GetUIApplication(), _T("menu_playlist_delete"), &pMenu);
    if (NULL == pMenu)
        return;

	CRect rc;
	pBtnObj->GetWindowRect(&rc);
	::MapWindowPoints(GetHWND(), NULL, (LPPOINT)&rc, 2);
	UINT nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, this, GetHWND(), &rc);
	SAFE_DELETE_Ixxx(pMenu);

	switch(nRet)
	{
	case 1201:
		{
			ITTPlayerPlaylistItem* pSelItem = m_plistctrl->GetCurSel();
			if (pSelItem)
			{
				GetPlayerListMgr()->RemovePlayListItem(pSelItem->GetPlayerListItemInfo());  // ͨ�� OnRemoveItem �ص�
			}
		}
		break;

    case 1203:  // ɾ�������ڵ��ļ�
        {
            GetPlayerListMgr()->RemoveNotExistFile();
        }
        break;

	case 1204:
		{
			if (GetPlayerListMgr()->RemoveAllItem())
			{
				m_plistctrl->RemoveAllItem(0);
				m_plistctrl->SetPlayingItem(NULL);
			}
		}
		break;
	}
}

void CPlayListDlg::OnBtnClickMode(IObject* pBtnObj, POINT* pt)
{
    UI::IMenu* pMenu = NULL;
    UI::UILoadMenu(GetUIApplication(), _T("menu_playlist_mode"), &pMenu);
    if (NULL == pMenu)
        return;

	PLAY_MODE eMode = GetPlayerListMgr()->GetPlayMode();
    UI::IListItemBase* pMenuItem = pMenu->GetItemByPos((int)eMode);
	if (pMenuItem)
	{
		pMenuItem->SetRadioChecked(true);
	}

	CRect rc;
	pBtnObj->GetWindowRect(&rc);
	::MapWindowPoints(GetHWND(), NULL, (LPPOINT)&rc, 2);
	UINT nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, this, GetHWND(), &rc);
	SAFE_DELETE_Ixxx(pMenu);

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


void CPlayListDlg::OnLCNDbclick(IMessage* pObjMsgFrom, POINT pt, UI::IListItemBase* pItem)
{
	if (NULL == pItem)
		return;

	ITTPlayerPlaylistItem* pPlayItem = static_cast<ITTPlayerPlaylistItem*>(pItem);
	if (NULL == pPlayItem)
		return;

	GetMainMgr()->Play((IPlayerListItemInfo *)pPlayItem->GetPlayerListItemInfo());
}

void CPlayListDlg::OnAddItem(IPlayerListItemInfo* pItemInfo, bool bUpdate)
{
	if (NULL == m_plistctrl)
		return;

	ITTPlayerPlaylistItem*  pItem = m_plistctrl->AddFileItem(pItemInfo, bUpdate);
    {
        // ��ȡ������Ϣ
        TCHAR szFormat[32] = _T("");
        String  strToolTip;

        strToolTip.append(_T("��   ��:  "));
        strToolTip.append(pItemInfo->GetTitle());
        strToolTip.append(_T("\n������:  "));
        strToolTip.append(pItemInfo->GetArtist());
        strToolTip.append(_T("\nר   ��:  "));
        strToolTip.append(pItemInfo->GetAlbum());
        strToolTip.append(_T("\n��   ʽ: "));
        int nSample = pItemInfo->GetSample();
        if (0 != nSample)
        {
            _stprintf(szFormat, _T(" %dkHZ"), nSample/1000);
            strToolTip.append(szFormat);
        }
        int nBitrate = pItemInfo->GetBitrate();
        if (0 != nBitrate)
        {
            _stprintf(szFormat, _T(" %dK"), nBitrate/1000);
            strToolTip.append(szFormat);
        }

        strToolTip.append(_T("\n��   ��:  "));

        int nDuration = pItemInfo->GetDuration();
        if (nDuration > 0)
        {
            nDuration = nDuration/1000;
            int n1 = nDuration%60;
            int n2 = nDuration/60;
            _stprintf(szFormat, _T("%02d:%02d"), n2, n1);
            strToolTip.append(szFormat);
        }

        strToolTip.append(_T("\n�ļ���:  "));
        strToolTip.append(pItemInfo->GetFilePath());

        pItem->SetToolTip(strToolTip.c_str());
    }
}

// �����겥���б��ˢ��
void CPlayListDlg::OnAddItemFinish(bool bUpdate)
{
	if (NULL == m_plistctrl)
		return;

	m_plistctrl->UpdateItemRect(m_plistctrl->GetFirstItem(), bUpdate);
}
void CPlayListDlg::OnRemoveItem(IPlayerListItemInfo* pInfo)
{
	if (NULL == m_plistctrl)
		return;

	m_plistctrl->RemoveFileItem(pInfo);
}
ITTPlayerPlaylistItem* CPlayListDlg::GetCtrlItemByPlayItem(IPlayerListItemInfo* p)
{
	if (NULL == p || NULL == m_plistctrl)
		return NULL;

    UI::IListItemBase*  pItem = m_plistctrl->GetFirstItem();
	while (pItem)
	{
		ITTPlayerPlaylistItem* pCtrlItem = (ITTPlayerPlaylistItem*)pItem;
		if (pCtrlItem->GetPlayerListItemInfo() == p)
		{
			return pCtrlItem;
		}
		pItem = pItem->GetNextItem();
	}
	return NULL;
}
void CPlayListDlg::OnMp3Start(IPlayerListItemInfo* pItemInfo)
{
	if (NULL == m_plistctrl)
		return;

	ITTPlayerPlaylistItem* pCtrlItem = this->GetCtrlItemByPlayItem(pItemInfo);
	if (NULL == pCtrlItem)
		return;

	m_plistctrl->SetPlayingItem(pCtrlItem);
}
