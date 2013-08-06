#include "stdafx.h"
#include "lyricdownload.h"
#include "App\Player\Player_UI\Mgr\Lyric\wininetwrap.h"
#include "3rd\markup\markup.h"

//////////////////////////////////////////////////////////////////////////
CLyricDownloadList::~CLyricDownloadList()
{
    vector<LyricDownloadItemInfo*>::iterator iter = m_vec.begin();
    for (; iter != m_vec.end(); iter++)
    {
        LyricDownloadItemInfo* p = *iter;
        SAFE_DELETE(p);
    }
    m_vec.clear();
} 

void  CLyricDownloadList::AddItem(const String&  strId, const String& strArtist, const String& strTitle)
{
    LyricDownloadItemInfo*  p = new LyricDownloadItemInfo;
    p->lId = _ttoi(strId.c_str());
    p->m_strArtist = strArtist;
    p->m_strTitle = strTitle;

    m_vec.push_back(p);
}   

LyricDownloadItemInfo*  CLyricDownloadList::GetItem(int nIndex)
{
    if (nIndex < 0)
        return NULL;

    if (nIndex >= (int)m_vec.size())
        return NULL;

    return m_vec[nIndex];
}

//////////////////////////////////////////////////////////////////////////

CLyricDownloadMgr::CLyricDownloadMgr()
{
    m_pActiveSvr = NULL;
}
CLyricDownloadMgr::~CLyricDownloadMgr()
{
    VecLyricSvr::iterator iter = m_vecSvr.begin();
    for (; iter != m_vecSvr.end(); iter++)
    {
        LyricServerInfo* p = *iter;
        SAFE_DELETE(p);
    }
    m_vecSvr.clear();
    m_pActiveSvr = NULL;
}

// 从指定的服务器上获取歌词服务器列表
// http://ttlrcct2.qianqian.com/  dll/lyricsvr.dll?svrlst
void  CLyricDownloadMgr::GetSvrList(const TCHAR* szServAddr, const TCHAR* szReqArg)
{
    if (NULL == szServAddr || NULL == szReqArg)
        return;

    IBuffer*  pBuffer = NULL;
    CreateBuffer(&pBuffer);

    if (-1 == GetHttpData(szServAddr, 80, szReqArg, pBuffer))
    {
        SAFE_RELEASE(pBuffer);
        return;
    }
    ParseSvrList((char*)pBuffer->Get());
    SAFE_RELEASE(pBuffer);
}

// 解析服务器列表
bool  CLyricDownloadMgr::ParseSvrList(const char* szContent)
{
    if (NULL == szContent)
        return false;

    CMarkup  markup;

    // 将utf8转成unicode
    int  nLength = (int)strlen(szContent);
    int  nSize = 3*nLength+1;
    wchar_t*  szUnicode = new wchar_t[nSize];
    memset(szUnicode, 0, sizeof(wchar_t)*nSize);
    CMarkup::UTF8To16((unsigned short*)szUnicode, szContent, nLength);
    bool  bRet = markup.SetDoc(szUnicode);
    SAFE_ARRAY_DELETE(szUnicode);

    if (false == bRet)
        return false;

    if (false == markup.FindElem())  return false;
    if (false == markup.IntoElem())  return false;
    
    while (1)
    {
        if (false == markup.FindElem(_T("server")))   break;
        
        LyricServerInfo*  pInfo = new LyricServerInfo;
        pInfo->m_strName = markup.GetAttrib(_T("name")); 
        pInfo->m_strUri  = markup.GetAttrib(_T("url"));

        // 解析uri
        TCHAR* p = const_cast<TCHAR*>(pInfo->m_strUri.c_str());
        int nPos1 = (int)pInfo->m_strUri.find_first_of(_T("http://"));

        int nPosFindNext = 0;
        if (-1 == nPos1)
        {
            nPos1 = 0;
        }
        else
        {
            nPos1 += (int)_tcslen(_T("http://"));
        }

        int nPos2 = (int)pInfo->m_strUri.find_first_of(_T("/"), nPos1);
        if (-1 == nPos2)
            nPos2 = (int)pInfo->m_strUri.length();

        pInfo->m_strUriAddr = pInfo->m_strUri.substr(nPos1, nPos2-nPos1);
        pInfo->m_strUriReqArg = pInfo->m_strUri.substr(nPos2+1, pInfo->m_strUri.length());

        m_vecSvr.push_back(pInfo);
    }

    if (m_vecSvr.size() > 0)
        m_pActiveSvr = m_vecSvr[0];

    return true;
}

// 搜索指定歌曲在服务器上的歌词列表
bool  CLyricDownloadMgr::GetSvrLyricList(const TCHAR* szArtist, const TCHAR* szTitle, IBuffer* pBuffer)
{
    if (NULL == szArtist || NULL == szTitle || NULL == pBuffer)
        return false;

    if (NULL == m_pActiveSvr)  // 下载歌词服务器列表失败
        return false;

    int  nLength = 0;
    int  nTitleLength = 0;
    int  nArtistLength = 0;

    TCHAR*  szSafeTitle = NULL;
    TCHAR*  szSafeArtist = NULL;
    TCHAR*  szTitleEncode = NULL;
    TCHAR*  szArtistEncode = NULL;

    // 查询关键字要去掉空格
    nLength = (int)_tcslen(szTitle);
    szSafeTitle = new TCHAR[nLength+1];
    memset(szSafeTitle, 0, (nLength+1)*sizeof(TCHAR));

    for (int i = 0, j = 0; i < nLength; i++)
    {
        if (szTitle[i] == _T(' ') ||
            szTitle[i] == _T(','))
            continue;

        szSafeTitle[j++] = szTitle[i];
    }

    nLength = (int)_tcslen(szArtist);
    szSafeArtist = new TCHAR[nLength+1];
    memset(szSafeArtist, 0, (nLength+1)*sizeof(TCHAR));
    for (int i = 0, j = 0; i < nLength; i++)
    {
        if (szArtist[i] == _T(' ') ||
            szArtist[i] == _T(','))
            continue;

        szSafeArtist[j++] = szArtist[i];
    }

    // 将查询字符转换成16进制编码的字符串
    nTitleLength = 4*(int)_tcslen(szSafeTitle) + 1;
    nArtistLength = 4*(int)_tcslen(szSafeArtist) + 1;

    szTitleEncode = new TCHAR[nTitleLength];
    szArtistEncode = new TCHAR[nArtistLength];
    memset(szTitleEncode, 0, nTitleLength*sizeof(TCHAR));
    memset(szArtistEncode, 0, nArtistLength*sizeof(TCHAR));

    UI::Util::UnicodeEncode(szSafeTitle, szTitleEncode);
    UI::Util::UnicodeEncode(szSafeArtist, szArtistEncode);
    SAFE_ARRAY_DELETE(szSafeArtist);
    SAFE_ARRAY_DELETE(szSafeTitle);

    // 向服务器请求查询歌词列表
    TCHAR* szReqArg= new TCHAR[MAX_PATH + nTitleLength + nArtistLength];
    _stprintf(szReqArg, _T("%s?sh?Artist=%s&Title=%s"), m_pActiveSvr->m_strUriReqArg.c_str(), szArtistEncode,  szTitleEncode);

    SAFE_ARRAY_DELETE(szTitleEncode);
    SAFE_ARRAY_DELETE(szArtistEncode);

    int nRet = GetHttpData(m_pActiveSvr->m_strUriAddr.c_str(), 80, szReqArg, pBuffer);
    SAFE_ARRAY_DELETE(szReqArg);

    if (-1 == nRet)
        return false;

    return true;
}

// 解析歌词列表
bool  CLyricDownloadMgr::ParseLyricList(const char* szContent, __out CLyricDownloadList* pList)
{
    if (NULL == szContent || NULL == pList)
        return false;

    CMarkup  markup;

    // 将utf8转成unicode
    int  nLength = (int)strlen(szContent);
    int  nSize = 3*nLength+1;
    wchar_t*  szUnicode = new wchar_t[nSize];
    memset(szUnicode, 0, sizeof(wchar_t)*nSize);
    CMarkup::UTF8To16((unsigned short*)szUnicode, szContent, nLength);
    bool  bRet = markup.SetDoc(szUnicode);
    SAFE_ARRAY_DELETE(szUnicode);

    if (false == bRet)
        return false;

    if (false == markup.FindElem())  return false;
    if (false == markup.IntoElem())  return false;

    while (1)
    {
        if (false == markup.FindElem(_T("lrc")))   break;
        pList->AddItem(markup.GetAttrib(_T("id")), markup.GetAttrib(_T("artist")), markup.GetAttrib(_T("title")) );
    }

    return true;
}

// 下载歌词需要两个参数：id + code，该code的算法从网上抄了一个
bool  CLyricDownloadMgr::Download(LyricDownloadItemInfo* pInfo, IBuffer* pBuffer)
{
    if (NULL == pInfo || NULL == pBuffer)
        return false;

    int  nCode = GetLyricDownloadCode(pInfo->lId, pInfo->m_strArtist, pInfo->m_strTitle);

    // 向服务器下载歌词
    TCHAR szReqArg[MAX_PATH*2] = _T("");
    _stprintf(szReqArg, _T("%s?dl?Id=%d&Code=%d"), m_pActiveSvr->m_strUriReqArg.c_str(), pInfo->lId, nCode);

    int nRet = GetHttpData(m_pActiveSvr->m_strUriAddr.c_str(), 80, szReqArg, pBuffer);
    if (-1 == nRet)
        return false;

    return true;
}


long long Conv(int i) 
{
    long long r = i % 4294967296;
    if (i >= 0 && r > 2147483648)
        r = r - 4294967296;

    if (i < 0 && r < 2147483648)
        r = r + 4294967296;
    return r;
}

// 该算法代码来自网络，对其含义及其内容不打算细看。
int  CLyricDownloadMgr::GetLyricDownloadCode(int nLyricId, const String& strArtist, const String& strTitle)
{
    String  strCode = strArtist;
    strCode.append(strTitle);

    int nUtf8CodeSize = ((int)strCode.length())*3 + 1;
    char* szUtf8Code = new char[strCode.length()*3 + 1];
    memset(szUtf8Code, 0, nUtf8CodeSize);
    UI::Util::U2UTF8(strCode.c_str(), szUtf8Code, nUtf8CodeSize);

    int  nLength = (int)strlen(szUtf8Code);
    int*  pCodeArr = new int[nLength];
    byte* p = (byte*)szUtf8Code;

    for (int i = 0; i < nLength; i++)
    {
        pCodeArr[i] = *p;
        p++;
    }
    SAFE_ARRAY_DELETE(szUtf8Code);

    int t1 = 0, t2 = 0, t3 = 0;
    t1 = (nLyricId & 0x0000FF00) >> 8;
    if ((nLyricId & 0x00FF0000) == 0) {
        t3 = 0x000000FF & ~t1;
    } else {
        t3 = 0x000000FF & ((nLyricId & 0x00FF0000) >> 16);
    }

    t3 = t3 | ((0x000000FF & nLyricId) << 8);
    t3 = t3 << 8;
    t3 = t3 | (0x000000FF & t1);
    t3 = t3 << 8;
    if ((nLyricId & 0xFF000000) == 0) {
        t3 = t3 | (0x000000FF & (~nLyricId));
    } else {
        t3 = t3 | (0x000000FF & (nLyricId >> 24));
    }

    int j = nLength - 1;
    while (j >= 0) {
        int c = pCodeArr[j];
        if (c >= 0x80) c = c - 0x100;

        t1 = (int)((c + t2) & 0x00000000FFFFFFFF);
        t2 = (int)((t2 << (j % 2 + 4)) & 0x00000000FFFFFFFF);
        t2 = (int)((t1 + t2) & 0x00000000FFFFFFFF);
        j -= 1;
    }
    j = 0;
    t1 = 0;
    while (j <= nLength - 1) {
        int c = pCodeArr[j];
        if (c >= 128) c = c - 256;
        int t4 = (int)((c + t1) & 0x00000000FFFFFFFF);
        t1 = (int)((t1 << (j % 2 + 3)) & 0x00000000FFFFFFFF);
        t1 = (int)((t1 + t4) & 0x00000000FFFFFFFF);
        j += 1;
    }

    int t5 = (int)Conv(t2 ^ t3);
    t5 = (int)Conv(t5 + (t1 | nLyricId));
    t5 = (int)Conv(t5 * (t1 | t3));
    t5 = (int)Conv(t5 * (t2 ^ nLyricId));

    long t6 = (long)t5;
    if (t6 > 2147483648)
        t5 = (int)(t6 - 4294967296);

    SAFE_ARRAY_DELETE(pCodeArr);
    return t5;
}

