#pragma once

class CConfig
{
public:
    CConfig();
    ~CConfig();

public:
    void  Load();
    void  Save();

	const TCHAR*  GetPicFolder();
    void  SetPicFolder(const TCHAR* szPath);

    void  SetTime(int nHour, int nMinute, int nSecond);
    void  SetBreakTime(int nBreakTime);
    int   GetHour();
    int   GetMinute();
    int   GetSecond();
    int   GetBreakTime();

	void  SetTransparent(int n);
	int   GetTransparent();
	void  SetHls_h(int h);
	int   GetHls_h();
	void  SetHls_s(int);
	int   GetHls_s();

    void  SetPlayMusic(bool b) { m_bPlayMusic = b; }
    bool  GetPlayMusic() { return m_bPlayMusic; }
private:
    bool  m_bDirty;

    bool  m_bPlayMusic;
    String  m_strMusicPath;

    int  m_nBreakTime;
    int  m_nHour;
    int  m_nMinute;
    int  m_nSecond;

	String  m_strPicFolder;

	int  m_nMainWndTransparent;
	int  m_hls_h;
	int  m_hls_s;
};