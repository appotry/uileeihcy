#pragma once
interface IBuffer;

// ��ʷ�������Ϣ
struct LyricServerInfo
{
    String  m_strName;
    String  m_strUri;

    String  m_strUriAddr;
    String  m_strUriReqArg;
};

// �����������Ϣ
struct LyricDownloadItemInfo
{
    int  lId;
    String  m_strArtist;
    String  m_strTitle;
};

class CLyricDownloadList
{
public:
    ~CLyricDownloadList();

    void  AddItem(const String&  strId, const String& strArtist, const String& strTitle);
    int  GetCount() { return (int)m_vec.size(); }
    LyricDownloadItemInfo*  GetItem(int nIndex);

private:
    vector<LyricDownloadItemInfo*>  m_vec;
};


class CLyricDownloadMgr
{
public:
    CLyricDownloadMgr();
    ~CLyricDownloadMgr();

    void  GetSvrList(const TCHAR* szServAddr, const TCHAR* szReqArg);
    bool  GetSvrLyricList(const TCHAR* szArtist, const TCHAR* szTitle, IBuffer* pBuffer);

    bool  ParseLyricList(const char* szContent, __out CLyricDownloadList* pList);
    bool  Download(LyricDownloadItemInfo* pInfo, IBuffer* pBuffer);

protected:
    bool  ParseSvrList(const char* szContent);
    int  GetLyricDownloadCode(int nLyricId, const String& strArtist, const String& strTitle);

    typedef vector<LyricServerInfo*>  VecLyricSvr;
    vector<LyricServerInfo*>  m_vecSvr;  // �������б� 
    LyricServerInfo*  m_pActiveSvr;      // ��ǰѡ��ķ�����
};
