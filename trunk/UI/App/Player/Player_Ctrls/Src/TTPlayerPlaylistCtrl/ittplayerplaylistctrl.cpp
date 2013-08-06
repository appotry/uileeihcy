#include "stdafx.h"
#include "APP\Player\Player_Ctrls\Inc\ittplayerplaylistctrl.h"
#include "TTPlayerPlaylistCtrl.h"


UI_IMPLEMENT_Ixxx_INTERFACE2(ITTPlayerPlaylistCtrl, TTPlayerPlaylistCtrl, UI::IListBox)
UI_IMPLEMENT_Ixxx_INTERFACE2(ITTPlayerPlaylistItem, TTPlayerPlaylistItem, UI::IListItemBase)

IPlayerListItemInfo*  ITTPlayerPlaylistItem::GetPlayerListItemInfo()
{
    return m_pTTPlayerPlaylistItemImpl->m_pItemInfo;
}

void  ITTPlayerPlaylistCtrl::SetPlayingItem(ITTPlayerPlaylistItem* pCtrlItem)
{
    if (NULL == pCtrlItem)
        m_pTTPlayerPlaylistCtrlImpl->SetPlayingItem(NULL);
    else
        m_pTTPlayerPlaylistCtrlImpl->SetPlayingItem(pCtrlItem->GetImpl());
}
ITTPlayerPlaylistItem*  ITTPlayerPlaylistCtrl::GetPlayingItem()
{
    TTPlayerPlaylistItem*  p = m_pTTPlayerPlaylistCtrlImpl->GetPlayingItem();
    if (NULL == p)
        return NULL;

    return p->GetITTPlayerPlaylistItem();
}

ITTPlayerPlaylistItem*  ITTPlayerPlaylistCtrl::AddFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate) { return m_pTTPlayerPlaylistCtrlImpl->AddFileItem(pItemInfo, bUpdate); }
void  ITTPlayerPlaylistCtrl::RemoveFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate) { m_pTTPlayerPlaylistCtrlImpl->RemoveFileItem(pItemInfo, bUpdate); }
ITTPlayerPlaylistItem*  ITTPlayerPlaylistCtrl::GetCurSel() { return m_pTTPlayerPlaylistCtrlImpl->GetCurSel(); }
