#pragma once
#include "App\Player\Player_UI\Data\PlayerListData.h"
#include "App\Player\Player_UI\UI\playlistdlg.h"
#include "App\Player\Player_UI\Mgr\player.h"

enum PLAY_MODE
{
	SINGLE_ONCE,   // 单曲播放
	SINGLE_LOOP,   // 单曲循环
	ALL_ONCE,      // 顺序播放
	ALL_LOOP,      // 循环播放
	ALL_RAND,      // 随机播放
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
	virtual HRESULT  SetGenre(int);    // 类型
	virtual HRESULT  SetTrack(int);    // 音轨
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
	String  m_strTitle;   // 标题
	String  m_strArtist;  // 艺术家
	String  m_strAlbum;   // 专辑
	String  m_strYear;    // 年份
	String  m_strComment; // 备注
	String  m_strGenre;   // 对m_nGenre的解释（WMA中没有m_nGenre，而是直接的字符串）
	int     m_nGenre;     // 流派
	int     m_nTrack;     // 专辑中的编号，用于排序等
    
    int     m_nDuration;  // 歌曲时长(单位ms)

	// Frame标志
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
	String  m_strDisplayText; // 在列表中显示的文本

	String  m_strFilePath;   // 歌曲路径
	String  m_strFileName;   // 文件名
	String  m_strFileTime;   // 歌曲时长

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
	CPlayerListData  m_data;           // 数据保存/读取
	CPlayListDlg*  m_pPlaylistDlg;     // 播放列表窗口

//	vector<PlayerListItemInfo*>  m_vecPlayerList;  // 播放列表
    typedef  UI::UIList<PlayerListItemInfo*>  _MyPlayList;
    typedef  UI::UIListItem<PlayerListItemInfo*>  _MyPlayListItem;
    _MyPlayList m_PlayerList;

	list<IPlayerListItemInfo*>   m_lstPlayHistory; // 随机播放历史，用于播放上一首时返回
	IPlayerListItemInfo*  m_pCurPlayingItem;  // 当前正在插放的文件
	PLAY_MODE  m_ePlayMode;        // 当前播放模式
};

