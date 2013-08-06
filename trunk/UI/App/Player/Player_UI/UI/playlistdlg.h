#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

class CPlayerListMgr;
class PlayerListItemInfo;

////////////////////////////////////////////////////////////////////////// 
// ÁÐ±í´°¿Ú

class CPlayListDlg : public UI::ICustomWindow
{
public:
	CPlayListDlg();
	~CPlayListDlg(void);

	UI_BEGIN_MSG_MAP_Ixxx(CPlayListDlg)
		UIMSG_WM_CREATE(OnCreate)
		UIMSG_WM_CLOSE(OnClose)
		UIMSG_BN_CLICKED_EX(_T("playlist_add"), OnBtnClickAdd )
		UIMSG_BN_CLICKED_EX(_T("playlist_del"), OnBtnClickDel )
		UIMSG_BN_CLICKED_EX(_T("playlist_mode"), OnBtnClickMode )
		UIMSG_LCN_DBCLICK(OnLCNDbclick)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)
 
public:
	BOOL  PreCreateWindow(CREATESTRUCT* pcs);
	void  OnInitWindow();

	int   OnCreate(LPCREATESTRUCT lpCreateStruct);
	void  OnClose();
	void  OnBtnClickAdd(IObject* pBtnObj, POINT* pt);
	void  OnBtnClickDel(IObject* pBtnObj, POINT* pt);
	void  OnBtnClickMode(IObject* pBtnObj, POINT* pt);
    void  OnLCNDbclick(IMessage* pObjMsgFrom, POINT pt, UI::IListItemBase* pItem);

public:
	void  OnAddItem(IPlayerListItemInfo* pItemInfo, bool bUpdate);
	void  OnAddItemFinish(bool bUpdate);
	void  OnRemoveItem(IPlayerListItemInfo* pInfo);
	void  OnMp3Start(IPlayerListItemInfo* pItemInfo);


    void  SetPlayListMgr(CPlayerListMgr* pPlayerListMgr) { m_pPlayerListMgr = pPlayerListMgr; }

protected:
	ITTPlayerPlaylistItem*  GetCtrlItemByPlayItem(IPlayerListItemInfo* p);

private:
	ITTPlayerPlaylistCtrl*   m_plistctrl;
	CPlayerListMgr*  m_pPlayerListMgr;

};

