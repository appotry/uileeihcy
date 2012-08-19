#pragma once

enum PLAYER_EVENT_TYPE
{
	EVENT_TYPE_UI,
	EVENT_TPPE_PLAYER
};
enum PLAYER_EVENT_ID
{
	UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE
};



class   CFrameWork;
class   CMainMgr;
class   CPlayerListMgr;

CFrameWork*     GetFrameWork();
CMainMgr*       GetMainMgr();
CPlayerListMgr* GetPlayerListMgr();

class IMgr
{
public:
	virtual  bool  Initialize() = 0;
	virtual  bool  Release() = 0;

public:
	virtual  void  DoEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam){};

	void     FireEvent(int nCmdType, int nEventId, WPARAM wParam = 0, LPARAM lParam = 0);
};


class CFrameWork
{
public:
	CFrameWork();
	~CFrameWork();

	static CFrameWork* GetInstance()
	{
		static CFrameWork  s;
		return &s;
	}

	CMainMgr*       GetMainMgr();
	CPlayerListMgr* GetPlayerListMgr();

	void            Release();

	void            FireEvent(IMgr* pSource,int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

private:
	CMainMgr*         m_pMgrMain;
	CPlayerListMgr*   m_pMgrPlayerList;
	vector<IMgr*>     m_vecMgr;
};