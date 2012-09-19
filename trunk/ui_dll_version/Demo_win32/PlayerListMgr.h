#pragma once
#include "PlayerListData.h"
#include "playlistdlg.h"
#include "player.h"

class PlayerListItemInfo
{
public:
	void    SetFilePath(const String& str);

	const String& GetFilePath(){ return m_strFilePath; }
	const String& GetFileName(){ return m_strFileName; }
	const String& GetFileTime(){ return m_strFileTime; }

protected:
	String    m_strFilePath;   // ����·��
	String    m_strFileName;   // �ļ���
	String    m_strFileTime;   // ����ʱ��
};

enum PLAY_MODE
{
	SINGLE_ONCE,   // ��������
	SINGLE_LOOP,   // ����ѭ��
	ALL_ONCE,      // ˳�򲥷�
	ALL_LOOP,      // ѭ������
	ALL_RAND,      // �������
};

class CPlayerListMgr : public IMgr
{
public:
	CPlayerListMgr(void);
	~CPlayerListMgr(void);

public:
	bool   Initialize();
	bool   Release();

	void   AddFile(const String& strFile);
	bool   AddDirectory(const String& strDir);
	bool   RemoveAllItem();
	bool   RemovePlayListItem(PlayerListItemInfo* pInfo);

	HWND   ToggleShowPlayerListDlg(HWND hParent);
	void   OnPlayListDlgHide();

	void   SetPlayMode(PLAY_MODE e) { m_ePlayMode = e; }
	PLAY_MODE GetPlayMode() { return m_ePlayMode; }

	void   OnLoadItem(const String& strFile);
	
	int   GetItemCount() { return (int)m_vecPlayerList.size(); }
	PlayerListItemInfo*  GetItem(int i);
	int   GetItemIndex(PlayerListItemInfo*);


	PlayerListItemInfo*  GetNextPlayItem(bool& bPlay);
	PlayerListItemInfo*  GetPrevItem();
	PlayerListItemInfo*  GetNextItem();

	virtual  void  HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

private:
	CPlayerListData   m_data;           // ���ݱ���/��ȡ
	CPlayListDlg*     m_pPlaylistDlg;   // �����б���

	vector<PlayerListItemInfo*>  m_vecPlayerList;  // �����б�
	list<PlayerListItemInfo*>    m_lstPlayHistory; // ���������ʷ�����ڲ�����һ��ʱ����
	PlayerListItemInfo*   m_pCurPlayingItem;  // ��ǰ���ڲ�ŵ��ļ�
	PLAY_MODE             m_ePlayMode;        // ��ǰ����ģʽ
};
