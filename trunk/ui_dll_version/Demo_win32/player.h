#pragma once

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

};
enum PLAYER_PALY_EVENT_ID
{
	PLAY_EVENT_ID_ON_START,                 // ��ʼ����һ��MP3��WPARAM: PlayItemInfo*
	PLAY_EVENT_ID_ON_PAUSE,
	PLAY_EVENT_ID_ON_CONTINUE,
	PLAY_EVENT_ID_ON_STOP
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

	CMainMgr*       GetMainMgr();
	CPlayerListMgr* GetPlayerListMgr();

	void            Release();

	void            FireEvent(IMgr* pSource,int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

private:
	CMainMgr*         m_pMgrMain;
	CPlayerListMgr*   m_pMgrPlayerList;
	vector<IMgr*>     m_vecMgr;
};