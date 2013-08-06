#ifndef LYRICANALYZE_H_36C5E114_0341_4325_ACB2_F32196038BA8
#define LYRICANALYZE_H_36C5E114_0341_4325_ACB2_F32196038BA8


enum LYRIC_TAG_TYPE
{
    LYRIC_TAG_UNKNOWN,
    LYRIC_TAG_ARTIST,
    LYRIC_TAG_TITLE,
    LYRIC_TAG_ALBUM,
    LYRIC_TAG_BY,
    LYRIC_TAG_OFFSET,
    LYRIC_TAG_TIME,
    LYRIC_TAG_COMMENT,
};

class CLyricItemData
{
public:
    CLyricItemData();

    int     m_nTime;  // ms
    String  m_strLyric;
};

typedef map<int, CLyricItemData*>  _LyricLineMap;

class CLyricData
{
public:
    CLyricData();
    ~CLyricData();

    HRESULT  Analyze(const TCHAR* szBuffer);

private:
    HRESULT  analyze_tag(const TCHAR* p, const TCHAR* pEnd);
    HRESULT  analyze_content(const TCHAR* p, const TCHAR* pEnd);

    LYRIC_TAG_TYPE  GetTagType(const TCHAR* p, const TCHAR* pEnd);
    void  Clear();

public:
    String  m_strTitle;
    String  m_strArtist;
    String  m_strAlbum;
    String  m_strBy;
    int  m_nOffset;

    _LyricLineMap  m_mapLyricLine;

private:
    vector<CLyricItemData*>  m_vecItemsWaitingForContent;
};

#endif // LYRICANALYZE_H_36C5E114_0341_4325_ACB2_F32196038BA8