#ifndef ITTPLAYERPLAYLISTCTRL_H_6447BAFA_18F5_458b_A546_9E48C8E7389E
#define ITTPLAYERPLAYLISTCTRL_H_6447BAFA_18F5_458b_A546_9E48C8E7389E

#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"

class TTPlayerPlaylistCtrl;
class TTPlayerPlaylistItem;
interface IPlayerListItemInfo;

interface PLAYER_CTRLS_API ITTPlayerPlaylistItem : public UI::IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(ITTPlayerPlaylistItem, TTPlayerPlaylistItem)

    IPlayerListItemInfo*  GetPlayerListItemInfo();
};

interface PLAYER_CTRLS_API ITTPlayerPlaylistCtrl : public UI::IListBox
{
    UI_DECLARE_Ixxx_INTERFACE(ITTPlayerPlaylistCtrl, TTPlayerPlaylistCtrl)
    
    ITTPlayerPlaylistItem*  AddFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate=true);
    void  RemoveFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate=true);
    ITTPlayerPlaylistItem*  GetCurSel();

    void  SetPlayingItem(ITTPlayerPlaylistItem* pCtrlItem);
	ITTPlayerPlaylistItem*  GetPlayingItem();
};


#endif // ITTPLAYERPLAYLISTCTRL_H_6447BAFA_18F5_458b_A546_9E48C8E7389E