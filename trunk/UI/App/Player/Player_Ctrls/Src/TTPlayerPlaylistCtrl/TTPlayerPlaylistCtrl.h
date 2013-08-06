#pragma once
#include "APP\Player\Player_Ctrls\Inc\ittplayerplaylistctrl.h"

//////////////////////////////////////////////////////////////////////////
// 模仿千千静听播放列表的自绘控件

#define XML_TTPLAYER_PLAYLIST_BKCOLOR1   _T("bkcolor1")
#define XML_TTPLAYER_PLAYLIST_BKCOLOR2   _T("bkcolor2")

#define XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR          _T("indextextcolor")
#define XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR_SELECT   _T("indextextcolor.select")
#define XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR_PLAYING  _T("indextextcolor.playing")

#define XML_TTPLAYER_PLAYLIST_TEXT_COLOR                _T("textcolor")
#define XML_TTPLAYER_PLAYLIST_TEXT_COLOR_SELECT         _T("textcolor.select")
#define XML_TTPLAYER_PLAYLIST_TEXT_COLOR_PLAYING        _T("textcolor.playing")

#define XML_TTPLAYER_PLAYLIST_TIME_COLOR                _T("timetextcolor")
#define XML_TTPLAYER_PLAYLIST_TIME_COLOR_SELECT         _T("timetextcolor.select")
#define XML_TTPLAYER_PLAYLIST_TIME_COLOR_PLAYING        _T("timetextcolor.playing")

#define XML_TTPLAYER_PLAYLIST_SELECT_BORDER_COLOR       _T("selectcolor.border")
#define XML_TTPLAYER_PLAYLIST_SELECT_COLOR1             _T("selectcolor1")
#define XML_TTPLAYER_PLAYLIST_SELECT_COLOR2             _T("selectcolor2")

class  TTPlayerPlaylistItem : public UI::MessageProxy
{
public:
    TTPlayerPlaylistItem();
    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TTPlayerPlaylistItem, UI::IListItemBase)

    void  SetITTPlayerPlaylistItem(ITTPlayerPlaylistItem* p) { m_pITTPlayerPlaylistItem = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }
    ITTPlayerPlaylistItem*  GetITTPlayerPlaylistItem() { return m_pITTPlayerPlaylistItem; }

protected:
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  GetDesiredSize(SIZE* pSize);

public:
	IPlayerListItemInfo*   m_pItemInfo;

private:
    ITTPlayerPlaylistItem*  m_pITTPlayerPlaylistItem;
};

class  TTPlayerPlaylistCtrl : public UI::MessageProxy
{
public:
	TTPlayerPlaylistCtrl();
	~TTPlayerPlaylistCtrl();

    UI_DECLARE_OBJECT3(TTPlayerPlaylistCtrl, UI::OBJ_CONTROL|UI::CONTROL_LISTBOX, _T("TTPlayerCtrl"))
    
    UI_BEGIN_MSG_MAP
        UIMSG_WM_NOTIFY(UI_LCN_DRAWITEM, OnDrawItem)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TTPlayerPlaylistCtrl, UI::IListBox)

    void  SetITTPlayerPlaylistCtrl(ITTPlayerPlaylistCtrl* p) { m_pITTPlayerPlaylistCtrl= p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }

public:
	ITTPlayerPlaylistItem*  AddFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate=true);
	void  RemoveFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate=true);
	ITTPlayerPlaylistItem*  GetCurSel() { return (ITTPlayerPlaylistItem*)m_pITTPlayerPlaylistCtrl->GetFirstSelectItem(); }

	void  SetPlayingItem(TTPlayerPlaylistItem* pCtrlItem);
	TTPlayerPlaylistItem*  GetPlayingItem(){ return m_pPlayingItem; }

protected:
    void  ResetAttribute();
    void  SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload);
    LRESULT  OnDrawItem(WPARAM w, LPARAM l);

protected:
	TTPlayerPlaylistItem*  m_pPlayingItem;
	UI::Color*   m_pBkColor1;
	UI::Color*   m_pBkColor2;

	UI::Color*   m_pIndexTextColor;
	UI::Color*   m_pIndexTextColorSelect;
	UI::Color*   m_pIndexTextColorPlaying;

	UI::Color*   m_pTextColor;
	UI::Color*   m_pTextColorSelect;
	UI::Color*   m_pTextColorPlaying;

	UI::Color*   m_pTimeTextColor;
	UI::Color*   m_pTimeTextColorSelect;
	UI::Color*   m_pTimeTextColorPlaying;

	UI::Color*   m_pSelectBorderColor;
	UI::Color*   m_pSelectColor1;
	UI::Color*   m_pSelectColor2;

private:
    ITTPlayerPlaylistCtrl*  m_pITTPlayerPlaylistCtrl;
};

