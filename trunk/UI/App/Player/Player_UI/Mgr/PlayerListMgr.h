#pragma once
#include "App\Player\Player_UI\Data\PlayerListData.h"
#include "App\Player\Player_UI\UI\playlistdlg.h"
#include "App\Player\Player_UI\Mgr\player.h"

enum PLAY_MODE
{
	SINGLE_ONCE,   // ��������
	SINGLE_LOOP,   // ����ѭ��
	ALL_ONCE,      // ˳�򲥷�
	ALL_LOOP,      // ѭ������
	ALL_RAND,      // �������
};


class MusicFileTagAttribute : public ITagParseCallback
{
public:
	MusicFileTagAttribute();

	virtual HRESULT  OnAddAttribute(WCHAR* pName, byte* pValue, int nValueSize, int nDataType);
	virtual HRESULT  SetTitle(const WCHAR*);
	virtual HRESULT  SetArtist(const WCHAR*);
	virtual HRESULT  SetAlbum(const WCHAR*);
	virtual HRESULT  SetYear(const WCHAR*);
	virtual HRESULT  SetComment(const WCHAR*);
	virtual HRESULT  SetGenreString(const WCHAR*);
	virtual HRESULT  SetGenre(int);    // ����
	virtual HRESULT  SetTrack(int);    // ����
    virtual HRESULT  SetDuration(int nDuration);
	virtual HRESULT  SetFormat(int nMpegVersion, int nLayerType, int nChannelMode);
    virtual HRESULT  SetBitrate(int nBitrate);
    virtual HRESULT  SetSapmleRate(int nSample);

	const WCHAR*  GetTitle() { return m_strTitle.c_str(); }
	const WCHAR*  GetArtist() { return m_strArtist.c_str(); }
	const WCHAR*  GetAlbum() { return m_strAlbum.c_str(); }
	const WCHAR*  GetYear() { return m_strYear.c_str(); }
	const WCHAR*  GetComment() { return m_strComment.c_str(); }
	const WCHAR*  GetGenreString() { return m_strGenre.c_str(); }
	int   GetTrack() { return m_nTrack; }
    int   GetBitrate() { return m_nBitrate; }
    int   GetSample() { return m_nSamplingRate; }
    int   GetDuration() { return m_nDuration; }
    mp3_channel_mode  GetChannelMode() { return m_eChannelMode; }

private:
	String  m_strTitle;   // ����
	String  m_strArtist;  // ������
	String  m_strAlbum;   // ר��
	String  m_strYear;    // ���
	String  m_strComment; // ��ע
	String  m_strGenre;   // ��m_nGenre�Ľ��ͣ�WMA��û��m_nGenre������ֱ�ӵ��ַ�����
	int     m_nGenre;     // ����
	int     m_nTrack;     // ר���еı�ţ����������
    
    int     m_nDuration;  // ����ʱ��(��λms)

	// Frame��־
	mp3_mpeg_audio_version m_eMpegVersion;
	mp3_layer_type         m_eLayerType;    
	mp3_channel_mode       m_eChannelMode; 
	int                    m_nBitrate;      
	int                    m_nSamplingRate; 
	
	map<String, String>  m_mapAttr;
};


class PlayerListItemInfo : public IPlayerListItemInfo
{
public:
	virtual const TCHAR*  GetDisplayText();
	virtual const TCHAR*  GetFilePath(){ return m_strFilePath.c_str(); }
	virtual const TCHAR*  GetFileName(){ return m_strFileName.c_str(); }
	virtual const TCHAR*  GetFileTime(){ return m_strFileTime.c_str(); }

    virtual const TCHAR*  GetArtist()  { return m_tagAttr.GetArtist(); }
    virtual const TCHAR*  GetTitle()   { return m_tagAttr.GetTitle();  }
    virtual const TCHAR*  GetAlbum()   { return m_tagAttr.GetAlbum(); }
    virtual int  GetBitrate() { return m_tagAttr.GetBitrate(); }
    virtual int  GetSample()  { return m_tagAttr.GetSample(); }
    virtual int  GetDuration(){ return m_tagAttr.GetDuration(); }
    virtual mp3_channel_mode  GetChannelMode() { return m_tagAttr.GetChannelMode(); }

public:
	void  SetFilePath(const String& str);
	MusicFileTagAttribute*  GetTagAttr() { return &m_tagAttr; }

protected:
	String  m_strDisplayText; // ���б�����ʾ���ı�

	String  m_strFilePath;   // ����·��
	String  m_strFileName;   // �ļ���
	String  m_strFileTime;   // ����ʱ��

	MusicFileTagAttribute  m_tagAttr;

};


class CPlayerListMgr : public IMgr
{
public:
	CPlayerListMgr(void);
	~CPlayerListMgr(void);

public:
	bool  Initialize();
	bool  Release();
	
	CPlayListDlg*  GetPalyListDlg() { return m_pPlaylistDlg; }
	PlayerListItemInfo* AddFile(const String& strFile);
	bool  AddDirectory(const String& strDir);
	bool  RemoveAllItem();
	bool  RemovePlayListItem(IPlayerListItemInfo* pInfo);
    int   RemoveNotExistFile();

	HWND  ToggleShowPlayerListDlg();
	void  OnPlayListDlgHide();

	void  SetPlayMode(PLAY_MODE e);
	PLAY_MODE  GetPlayMode() { return m_ePlayMode; }

    PlayerListItemInfo*  OnLoadItem(const String& strFile, UI::IMapAttribute*);
	
	int   GetItemCount() { return (int)m_PlayerList.GetCount(); }
	IPlayerListItemInfo*  GetItemByPath(const String& strFilePath);
//	IPlayerListItemInfo*  GetItem(int i);
//	int   GetItemIndex(IPlayerListItemInfo*);

    IPlayerListItemInfo*  GetCurPlayingItem() { return m_pCurPlayingItem; }
	IPlayerListItemInfo*  GetNextPlayItem(bool& bPlay);
	IPlayerListItemInfo*  GetPrevItem();
	IPlayerListItemInfo*  GetNextItem();
    IPlayerListItemInfo*  GetFirstItem();

	virtual void  HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

private:
	CPlayerListData  m_data;           // ���ݱ���/��ȡ
	CPlayListDlg*  m_pPlaylistDlg;     // �����б���

//	vector<PlayerListItemInfo*>  m_vecPlayerList;  // �����б�
    typedef  UI::UIList<PlayerListItemInfo*>  _MyPlayList;
    typedef  UI::UIListItem<PlayerListItemInfo*>  _MyPlayListItem;
    _MyPlayList m_PlayerList;

	list<IPlayerListItemInfo*>   m_lstPlayHistory; // ���������ʷ�����ڲ�����һ��ʱ����
	IPlayerListItemInfo*  m_pCurPlayingItem;  // ��ǰ���ڲ�ŵ��ļ�
	PLAY_MODE  m_ePlayMode;        // ��ǰ����ģʽ
};

