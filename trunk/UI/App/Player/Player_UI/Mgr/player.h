#pragma once
#include "App\Player\Player_UI\Data\PlayerConfigData.h"

//
// �¼�����
//
//	1. �¼�����
//		
//	   �¼����ͷ�Ϊ�������Ա���ٱ���������PLAYER_EVENT_TYPE + PLAYER_EVENT_ID
//
//	2. ����/��Ӧ
//	
//	   IMgr::FireEvent ���� CFrameWork::FireEvent��CFrameWork���༭m_vecMgr�б�����ÿһ��
//	   Mgr��HandleEvent�ӿڡ�Mgr����ȥ�����Լ�����Ȥ���¼�
//
//	3. Ϊʲô������ Mgrȥע���Լ�����Ȥ���¼� �ķ�����ȴ������ȫ��������
//	   
//	   ���򵥡�
//		
//
enum PLAYER_EVENT_TYPE
{
	EVENT_TYPE_UI,
	EVENT_TYPE_PLAY
};
enum PLAYER_UI_EVENT_ID
{
	UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE,              // �����б��ڱ�������WPARAM: ���ھ���� LPARAM:
	UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED,     // �����б�����ʾ��  WPARAM�����ھ���� LPARAM:BOOL bVisible
	UI_EVENT_ID_ON_EQUALIZERDLG_CREATE,               // ���������ڱ�������  WPARAM: ���ھ��
	UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED,      // ������������ʾ��    WPARAM�����ھ���� LPARAM:BOOL bVisible
    UI_EVENT_ID_ON_LYRICDLG_CREATE,                   // ��ʴ��ڱ�������    WPARAM: ���ھ��
    UI_EVENT_ID_ON_LYRICDLG_VISIBLE_CHANGED,          // ��ʴ�����ʾ��      WPARAM�����ھ���� LPARAM:BOOL bVisible
    UI_EVENT_ID_ON_WNDTOPMOST_CHANGED,                // �����ö���ʾ��      WPARAM:            LPARAM:BOOL bTopMost
};
enum PLAYER_PALY_EVENT_ID
{
	PLAY_EVENT_ID_ON_START,                 // ��ʼ����һ��MP3��WPARAM: IPlayerListItemInfo*
	PLAY_EVENT_ID_ON_PAUSE,
	PLAY_EVENT_ID_ON_CONTINUE,
	PLAY_EVENT_ID_ON_STOP,                  // ֹͣ����
    PLAY_EVENT_ID_ON_FINISH,                // ���Ž���, WPARAM: bool �������Ƿ񲥷���һ��
	PLAY_EVENT_ID_REMOVEALL,                // �����б����
	PLAY_EVENT_ID_REMOVEITEM,               // ɾ��ĳһ�WPARAM: PlayItemInfo*
	PLAY_EVENT_ID_BALANCE_CHANGED,          // �޸���ƽ�����, WPARAM: -100 - 100
    PLAY_EVENT_ID_PROGRESS_IND,             // mp3����֪ͨ��WPARAM��double* pdSeconds;  LPARAM: double* pdPecents;
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

    CPlayerConfigData     m_configFile;       // �������ݱ���/��ȡ
    CConfigData           m_config;           // ��������
};