#pragma once
class CPlayerListMgr;
class PlayerListItemInfo;

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

class TTPlayerPlaylistItem : public ListItemBase
{
public:
	TTPlayerPlaylistItem(ListCtrlBase* pCtrl):ListItemBase(pCtrl){};

public:
	PlayerListItemInfo*   m_pItemInfo;
};

class TTPlayerPlaylistCtrl : public ListCtrlBase
{
public:
	TTPlayerPlaylistCtrl();
	~TTPlayerPlaylistCtrl();
	UI_DECLARE_OBJECT( TTPlayerPlaylistCtrl, OBJ_CONTROL )

public:
	virtual  void ResetAttribute();
	virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

public:
	void     AddFileItem(PlayerListItemInfo* pItemInfo, bool bUpdate=true);
	void     RemoveFileItem(PlayerListItemInfo* pItemInfo, bool bUpdate=true);

	virtual  void OnDrawItem(HRDC hRDC, ListItemBase* p);
	virtual  SIZE OnMeasureItem( ListItemBase* p);
	virtual  void OnDeleteItem( ListItemBase* p );

	void    SetPlayingItem(TTPlayerPlaylistItem* pCtrlItem);
	TTPlayerPlaylistItem*  GetPlayingItem(){ return m_pPlayingItem; }

protected:
	void    BuildItemText(TTPlayerPlaylistItem* pCtrlItem, String& strOut);

protected:
	TTPlayerPlaylistItem*  m_pPlayingItem;
	UIColor*   m_pBkColor1;
	UIColor*   m_pBkColor2;
	
	UIColor*   m_pIndexTextColor;
	UIColor*   m_pIndexTextColorSelect;
	UIColor*   m_pIndexTextColorPlaying;

	UIColor*   m_pTextColor;
	UIColor*   m_pTextColorSelect;
	UIColor*   m_pTextColorPlaying;

	UIColor*   m_pTimeTextColor;
	UIColor*   m_pTimeTextColorSelect;
	UIColor*   m_pTimeTextColorPlaying;

	UIColor*   m_pSelectBorderColor;
	UIColor*   m_pSelectColor1;
	UIColor*   m_pSelectColor2;

};

////////////////////////////////////////////////////////////////////////// 
// 列表窗口

class CPlayListDlg : public UI::CustomWindow
{
public:
	CPlayListDlg(CPlayerListMgr* pPlayerListMgr);
	~CPlayListDlg(void);

	UI_BEGIN_MSG_MAP
		UIMSG_WM_CREATE( OnCreate )
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_BN_CLICKED_EX(_T("playlist_add"), OnBtnClickAdd )
		UIMSG_BN_CLICKED_EX(_T("playlist_del"), OnBtnClickDel )
		UIMSG_BN_CLICKED_EX(_T("playlist_mode"), OnBtnClickMode )
		UIMSG_LCN_DBCLICK(OnLCNDbclick)
	UI_END_MSG_MAP_CHAIN_PARENT( CustomWindow )
 
public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
	virtual void OnInitWindow( );

	int     OnCreate(LPCREATESTRUCT lpCreateStruct);
	void    OnClose();
	void    OnBtnClickAdd(Object* pBtnObj, POINT* pt);
	void    OnBtnClickDel(Object* pBtnObj, POINT* pt);
	void    OnBtnClickMode(Object* pBtnObj, POINT* pt);
	void    OnLCNDbclick(Message* pObjMsgFrom, POINT pt, ListItemBase* pItem);

public:
	void    OnAddItem(PlayerListItemInfo* pItemInfo);
	void    OnRemoveItem(PlayerListItemInfo* pInfo);
	void    OnMp3Start(PlayerListItemInfo* pItemInfo);

protected:
	TTPlayerPlaylistItem*  GetCtrlItemByPlayItem(PlayerListItemInfo* p);

private:
	TTPlayerPlaylistCtrl*   m_plistctrl;
	CPlayerListMgr*         m_pPlayerListMgr;

};

