#include "stdafx.h"
#include "lyricanalyze.h"

// http://www.doc88.com/p-686404873763.html
// 
// һ����ʶ��ǩ���ʽΪ��[��ʶ��:ֵ]��
// [ar:������]��artist�����ҡ��ݳ���
// [ti:������]��title��Ŀ,����,��Ŀ
// [al:ר����]��album��Ƭ����ר��
// [by:�༭��]������ʣ������ߡ��༭��Ա(һ��ָlrc��ʵ�������)
// [offset:ʱ�䲹��ֵ]���䵥λ�Ǻ��룬��ֵ��ʾ�ӳ٣���ֵ��ʾ��ǰ������������������ʾ�����죩�����ֲ��š�
// ��ʱ���ǩ����ʽΪ��[mm:ss]����[mm:ss.fff]��
// [mm:ss]��������:����
// [mm:ss.fff] �� [mm:ss:fff]��������:����.������
// ʱ���ǩλ��ĳ�и���еľ��ײ��֣�һ�и�ʿ��԰������ʱ���ǩ(�������е��ص��䲿��)��
//
// [:] ע�ͱ�ǩ����������ݲ�����


CLyricItemData::CLyricItemData()
{
    m_nTime = 0;
}
CLyricData::CLyricData()
{
    m_nOffset = 0;
}
CLyricData::~CLyricData()
{
   Clear();
}

inline const TCHAR*  GetNextSymbol(const TCHAR* pStart, const TCHAR* pEnd, const TCHAR c)
{
    const TCHAR* p = pStart;

    while (*p != c && p != pEnd)
        p++;

    return p;
}

inline const TCHAR*  GetNextLeftSymbol(const TCHAR* pStart, const TCHAR* pEnd)
{
    return GetNextSymbol(pStart, pEnd, _T('['));
}
inline const TCHAR*  GetNextRightSymbol(const TCHAR* pStart, const TCHAR* pEnd, bool& bError)
{
    bError = false;

    const TCHAR* p = pStart;
    while (p != pEnd)
    {
        if (*p == _T(']'))
        {
            return p;
        }
        else if (*p == _T('['))  // ����������]֮ǰ������һ��[��������һ��[��i֮������ݣ��Ӹô����²���
        {
            bError = true;
            return p;
        }
        p++;
    }
    return p;
}

void  CLyricData::Clear()
{
    _LyricLineMap::iterator iter = m_mapLyricLine.begin();
    for (; iter != m_mapLyricLine.end(); iter++)
    {
        CLyricItemData* p = iter->second;
        SAFE_DELETE(p);
    }
    m_mapLyricLine.clear();
    m_vecItemsWaitingForContent.clear();

    m_strTitle.clear();
    m_strArtist.clear();
    m_strAlbum.clear();
    m_strBy.clear();
    m_nOffset = 0;
}

HRESULT  CLyricData::Analyze(const TCHAR* szBuffer)
{
    this->Clear();
    if (NULL == szBuffer)
        return E_FAIL;

    int  nLength = _tcslen(szBuffer);
    bool bError = false;

    const TCHAR*  pEndBuffer = szBuffer + nLength;  // ����λ��
    const TCHAR*  p = szBuffer;  // ��ǰ����λ��
    const TCHAR*  pStart = p;    // ��ǩ��ʼ��λ�� ָ��[
    const TCHAR*  pNext = p;     // ��һ����ǩ��ʼ��λ��
    
    p = pStart = GetNextLeftSymbol(szBuffer, pEndBuffer);
    while (p < pEndBuffer)
    {
        p = GetNextRightSymbol(p+1, pEndBuffer, bError);
        if (bError)
        {
            p = pStart = GetNextLeftSymbol(p, pEndBuffer);
            continue;
        }
        
        pNext = GetNextLeftSymbol(p+1, pEndBuffer);
        if (pNext == p+1) // ��һ�����Ǳ�ǩ����ô�Ƚ��������ǩ
        {
            analyze_tag(pStart+1, p);
        }
        else  // ��һ��λ���������ֶ�
        {
            analyze_tag(pStart+1, p);
            analyze_content(p+1, pNext);
        }
        p = pStart = pNext;
    }

    m_vecItemsWaitingForContent.clear();
    return S_OK;
}

HRESULT  CLyricData::analyze_tag(const TCHAR* p, const TCHAR* pEnd)
{
#ifdef _DEBUGx
    String  str;
    str.insert(0, p, pEnd-p);
#endif

    int nLength = pEnd-p;

    // 1. ���� :
    const TCHAR*  pSep = GetNextSymbol(p, pEnd, _T(':'));
    if (pSep >= pEnd)
        return E_FAIL;

    LYRIC_TAG_TYPE eTagType = GetTagType(p, pSep);
    
    switch (eTagType)
    {
    case LYRIC_TAG_TITLE:
        {
            m_strTitle.clear();
            m_strTitle.insert(0, p+3, nLength-3); // ���� ti:
        }   
        break;

    case LYRIC_TAG_ARTIST:
        {
            m_strArtist.clear();
            m_strArtist.insert(0, p+3, nLength-3);  // ���� ar:
        }
        break;

    case LYRIC_TAG_ALBUM:
        {
            m_strAlbum.clear();
            m_strAlbum.insert(0, p+3, nLength-3); // ���� al:
        }
        break;

    case LYRIC_TAG_BY:
        {
            m_strBy.clear();
            m_strBy.insert(0, p+3, nLength-3); // ���� by:
        }
        break;

    case LYRIC_TAG_OFFSET:
        {
            String  str;
            str.insert(0, p+7, nLength-7);  // ����offset:
            m_nOffset = _ttoi(str.c_str());
        }
        break;

    case LYRIC_TAG_TIME:
        {
            String  strMinute;
            strMinute.insert(0, p, 2);
            String  strSecond;
            strSecond.insert(0, p+3, nLength-3);

            int nMinute = _wtoi(strMinute.c_str());
            double dSecond = _wtof(strSecond.c_str());

            int nTime = (int)(dSecond*1000) + nMinute*60000;
            
            _LyricLineMap::iterator iter = m_mapLyricLine.find(nTime);
            if (iter == m_mapLyricLine.end())
            {
                CLyricItemData*  pLyricItem = new CLyricItemData;
                pLyricItem->m_nTime = nTime;

                m_mapLyricLine[nTime] = pLyricItem;
                m_vecItemsWaitingForContent.push_back(pLyricItem);
            }
        }
    }

    return S_OK;
}
HRESULT  CLyricData::analyze_content(const TCHAR* p, const TCHAR* pEnd)
{
#ifdef _DEBUGx
    String  str;
    str.insert(0, p, pEnd-p);
#endif

    // ȥ��������\r\n
    if (*(pEnd-1) == _T('\n') && p < pEnd)
        pEnd--;
    if (*(pEnd-1) == _T('\r') && p < pEnd)
        pEnd--;

    int nCount = (int)m_vecItemsWaitingForContent.size();
    for (int i = 0; i < nCount; i++)
    {
        m_vecItemsWaitingForContent[i]->m_strLyric.clear();
        m_vecItemsWaitingForContent[i]->m_strLyric.insert(0, p, pEnd-p);
    }

    m_vecItemsWaitingForContent.clear();
    return S_OK;
}


LYRIC_TAG_TYPE  CLyricData::GetTagType(const TCHAR* p, const TCHAR* pEnd)
{
    int nLength = pEnd-p;
    if (0 == nLength)
        return LYRIC_TAG_COMMENT;

    if (6 == nLength)
    {
        String str;
        str.insert(0, p, 6);
        if (0 == _tcsicmp(str.c_str(), _T("offset")))
            return LYRIC_TAG_OFFSET;
        else
            return LYRIC_TAG_UNKNOWN;
    }
    if (2 != nLength)
    {
        return LYRIC_TAG_UNKNOWN;
    }

    TCHAR p1 = p[0];
    TCHAR p2 = p[1];

    if ( (p1 >= _T('0') && p1 <= _T('9')) && (p2 >= _T('0') && p2 <= _T('9')) )
        return LYRIC_TAG_TIME;

    if ( (p1 == _T('A') || p1 == _T('a')) && (p2 == _T('R') || p2 == _T('r')) )
        return LYRIC_TAG_ARTIST;
    
    if ( (p1 == _T('T') || p1 == _T('t')) && (p2 == _T('i') || p2 == _T('I')) )
        return LYRIC_TAG_TITLE;

    if ( (p1 == _T('A') || p1 == _T('a')) && (p2 == _T('l') || p2 == _T('L')) )
        return LYRIC_TAG_ALBUM;

    if ( (p1 == _T('A') || p1 == _T('a')) && (p2 == _T('R') || p2 == _T('r')) )
        return LYRIC_TAG_ARTIST;

    if ( (p1 == _T('B') || p1 == _T('b')) && (p2 == _T('y') || p2 == _T('Y')) )
        return LYRIC_TAG_BY;

    return LYRIC_TAG_UNKNOWN;
}