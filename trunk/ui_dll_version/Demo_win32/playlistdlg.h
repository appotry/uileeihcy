#pragma once
class CPlayerListMgr;
class PlayerListItemInfo;

//////////////////////////////////////////////////////////////////////////
// 模仿千千静听播放列表的自绘控件

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
	UI_DECLARE_OBJECT( TTPlayerPlaylistCtrl, OBJ_CONTROL )

public:
	virtual  void ResetAttribute();
	virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

public:
	void    AddFileItem(PlayerListItemInfo* pItemInfo, bool bUpdate=true);

	virtual  void OnDrawItem(HRDC hRDC, ListItemBase* p);
	virtual  SIZE OnMeasureItem( ListItemBase* p);
	virtual  void OnDeleteItem( ListItemBase* p );

	void    SetPlayingItem(TTPlayerPlaylistItem* pCtrlItem);
	TTPlayerPlaylistItem*  GetPlayingItem(){ return m_pPlayingItem; }

protected:
	void    BuildItemText(TTPlayerPlaylistItem* pCtrlItem, String& strOut);

protected:
	TTPlayerPlaylistItem*  m_pPlayingItem;
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
		UIMSG_LCN_DBCLICK(OnLCNDbclick)
	UI_END_MSG_MAP_CHAIN_PARENT( CustomWindow )
 
public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
	virtual void OnInitWindow( );

	int     OnCreate(LPCREATESTRUCT lpCreateStruct);
	void    OnClose();
	void    OnBtnClickAdd(Object* pBtnObj, POINT* pt);
	void    OnLCNDbclick(POINT pt, ListItemBase* pItem);

public:
	void    OnAddItem(PlayerListItemInfo* pItemInfo);

	void    OnMp3Start(PlayerListItemInfo* pItemInfo);

protected:
	TTPlayerPlaylistItem*  GetCtrlItemByPlayItem(PlayerListItemInfo* p);

private:
	TTPlayerPlaylistCtrl*   m_plistctrl;
	CPlayerListMgr*         m_pPlayerListMgr;

};

