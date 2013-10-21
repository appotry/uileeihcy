#pragma once
#include "App\Player\Player_UI\Data\PlayerConfigData.h"

//
// 事件机制
//
//	1. 事件类型
//		
//	   事件类型分为两级，以便减少遍历次数：PLAYER_EVENT_TYPE + PLAYER_EVENT_ID
//
//	2. 激发/响应
//	
//	   IMgr::FireEvent 调用 CFrameWork::FireEvent，CFrameWork将编辑m_vecMgr列表，调用每一个
//	   Mgr的HandleEvent接口。Mgr即可去处理自己感兴趣的事件
//
//	3. 为什么不采用 Mgr去注册自己感兴趣的事件 的方法，却采用了全部遍历？
//	   
//	   更简单。
//		
//
enum PLAYER_EVENT_TYPE
{
	EVENT_TYPE_UI,
	EVENT_TYPE_PLAY
};
enum PLAYER_UI_EVENT_ID
{
	UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE,              // 播放列表窗口被创建，WPARAM: 窗口句柄； LPARAM:
	UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED,     // 播放列表窗口显示，  WPARAM：窗口句柄； LPARAM:BOOL bVisible
	UI_EVENT_ID_ON_EQUALIZERDLG_CREATE,               // 均衡器窗口被创建，  WPARAM: 窗口句柄
	UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED,      // 均衡器窗口显示，    WPARAM：窗口句柄； LPARAM:BOOL bVisible
    UI_EVENT_ID_ON_LYRICDLG_CREATE,                   // 歌词窗口被创建，    WPARAM: 窗口句柄
    UI_EVENT_ID_ON_LYRICDLG_VISIBLE_CHANGED,          // 歌词窗口显示，      WPARAM：窗口句柄； LPARAM:BOOL bVisible
    UI_EVENT_ID_ON_WNDTOPMOST_CHANGED,                // 窗口置顶显示，      WPARAM:            LPARAM:BOOL bTopMost
};
enum PLAYER_PALY_EVENT_ID
{
	PLAY_EVENT_ID_ON_START,                 // 开始播放一首MP3，WPARAM: IPlayerListItemInfo*
	PLAY_EVENT_ID_ON_PAUSE,
	PLAY_EVENT_ID_ON_CONTINUE,
	PLAY_EVENT_ID_ON_STOP,                  // 停止播放
    PLAY_EVENT_ID_ON_FINISH,                // 播放结束, WPARAM: bool 接下来是否播放下一曲
	PLAY_EVENT_ID_REMOVEALL,                // 播放列表被清空
	PLAY_EVENT_ID_REMOVEITEM,               // 删除某一项，WPARAM: PlayItemInfo*
	PLAY_EVENT_ID_BALANCE_CHANGED,          // 修改了平衡参数, WPARAM: -100 - 100
    PLAY_EVENT_ID_PROGRESS_IND,             // mp3进度通知，WPARAM：double* pdSeconds;  LPARAM: double* pdPecents;
};



class  CFrameWork;
class  CMainMgr;
class  CPlayerListMgr;
class  CEqualizerMgr;
class  CLyricMgr;

CFrameWork*     GetFrameWork();
CMainMgr*       GetMainMgr();
CPlayerListMgr* GetPlayerListMgr();
CEqualizerMgr*  GetEqualizerMgr();
CLyricMgr*      GetLyricMgr();
CConfigData*    GetConfigData();

class IMgr
{
public:
	virtual  bool  Initialize() = 0;
	virtual  bool  Release() = 0;

public:
	virtual  void  HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam) = 0;

protected:
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

	CMainMgr*        GetMainMgr();
	CPlayerListMgr*  GetPlayerListMgr();
	CEqualizerMgr*   GetEqualizerMgr();
    CLyricMgr*       GetLyricMgr();
    CConfigData*     GetConfigData() { return &m_config; }

    bool  SaveConfig();

	void  Release();
	void  FireEvent(IMgr* pSource,int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

private:
	CMainMgr*         m_pMgrMain;
	CPlayerListMgr*   m_pMgrPlayerList;
	CEqualizerMgr*    m_pMgrEqualizer;
    CLyricMgr*        m_pMgrLyric;
	vector<IMgr*>     m_vecMgr;

    CPlayerConfigData     m_configFile;       // 配置数据保存/读取
    CConfigData           m_config;           // 配置数据
};