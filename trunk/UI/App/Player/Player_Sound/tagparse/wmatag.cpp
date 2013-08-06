#include "stdafx.h"
#include "wmatag.h"


bool CWMATag::ParseFile(const TCHAR* szFilePath, ITagParseCallback* pCallback)
{
    if (NULL == szFilePath || NULL == pCallback)
        return false;

    CComPtr<IWMSyncReader>     pSyncReader = NULL;  //  使用IWMReader读取时，发时Release时死锁了，改用SyncReader
    CComQIPtr<IWMHeaderInfo>   pHeaderInfo;

    do 
    {
        WMCreateSyncReader(NULL, 0, &pSyncReader);
        if (NULL == pSyncReader)
            break;

        HRESULT hr = pSyncReader->Open(szFilePath);
        if (FAILED(hr))
            break;

        pHeaderInfo = pSyncReader;
        if (NULL == pHeaderInfo)
            break;

        WORD wStream = 0, wNameBytes = 0, wValueBytes = 0;
        WMT_ATTR_DATATYPE dataType = WMT_TYPE_STRING;

        WORD  wAttrCount = 0;
        pHeaderInfo->GetAttributeCount(wStream, &wAttrCount);
        for (WORD i = 0; i < wAttrCount; i++)
        {
            HRESULT hr = pHeaderInfo->GetAttributeByIndex(i, &wStream, NULL, &wNameBytes, &dataType, NULL, &wValueBytes);
            if (FAILED(hr))
                continue;

            if (wNameBytes <= 0 || wValueBytes <=0)
                continue;

            WCHAR* pwszName  = (WCHAR*)new WCHAR[wNameBytes];
            BYTE*  pValue = new BYTE[wValueBytes];

            hr = pHeaderInfo->GetAttributeByIndex(i, &wStream, pwszName, &wNameBytes, &dataType, pValue, &wValueBytes);

            this->ParseAttrib(pwszName, pValue, wValueBytes, dataType, pCallback);

            SAFE_ARRAY_DELETE(pwszName);
            SAFE_ARRAY_DELETE(pValue);
        }
    }
    while (0);

    if (pSyncReader)
        pSyncReader->Close();

    return false;
}


// http://msdn.microsoft.com/zh-cn/library/dd743066(v=vs.85).aspx
// 		hr = pHeaderInfo->GetAttributeByName(&wStream, g_wszWMAuthor, &dataType, NULL, &wBytes); 
// 		if (FAILED(hr))
// 			break;
// 
// 		if (wBytes > 0)
// 		{
// 			WCHAR* pwszTitle = (WCHAR*)new BYTE[wBytes];
// 			pHeaderInfo->GetAttributeByName(&wStream, g_wszWMAuthor, &dataType, (BYTE*)pwszTitle, &wBytes); 
// 			SAFE_ARRAY_DELETE(pwszTitle);
// 		}
bool CWMATag::ParseAttrib(WCHAR* pszName, BYTE* pBytes, int nByteSize, WMT_ATTR_DATATYPE dataType, ITagParseCallback* pCallback)
{
    if (NULL == pszName || NULL == pBytes || NULL == pCallback)
        return false;

    if (0 == wcscmp(pszName, g_wszWMTitle))        
    {
        pCallback->SetTitle((WCHAR*)pBytes);
    }
    else if (0 == wcscmp(pszName, g_wszWMAuthor))  // 还有一个：唱片集艺术家 "WM/AlbumArtist"
    {
        pCallback->SetArtist((WCHAR*)pBytes);
    }
    else if (0 == wcscmp(pszName, g_wszWMAlbumTitle))
    {
        pCallback->SetAlbum((WCHAR*)pBytes);
    }
    else if (0 == wcscmp(pszName, g_wszWMDescription))
    {
        pCallback->SetComment((WCHAR*)pBytes);
    }
    else if (0 == wcscmp(pszName, g_wszWMGenre))  // 还有一个 g_wszWMGenreID，这个是不是数值型的？
    {
        pCallback->SetGenreString((WCHAR*)pBytes);
    }
    else if (0 == wcscmp(pszName, g_wszWMGenreID)) // 这可能是一个字符型数据，例如：(13)
    {
        if (dataType == WMT_TYPE_DWORD)
        {
            pCallback->SetGenre(*((DWORD*)pBytes));
        }
        else
        {
            // ...
        }
    }
    else if (0 == wcscmp(pszName, g_wszWMYear))
    {
        UIASSERT(dataType == WMT_TYPE_STRING);
        if (dataType == WMT_TYPE_STRING)
        {
            pCallback->SetYear((WCHAR*)pBytes);
        }
    }
    else if (0 == wcscmp(pszName, g_wszWMTrackNumber))
    {
        if (dataType == WMT_TYPE_DWORD)
        {
            pCallback->SetTrack(*((DWORD*)pBytes));
        }
        else if (dataType == WMT_TYPE_STRING)
        {
            pCallback->SetTrack(_wtoi((wchar_t*)pBytes));
        }
        else
        {
            UIASSERT(0);
        }
    }
    else if (0 == wcscmp(pszName, g_wszWMBitrate))
    {
        if (dataType == WMT_TYPE_DWORD)
        {
            int nBitrate = (int)(*((DWORD*)pBytes));
            pCallback->SetBitrate(nBitrate);
        }
    }
    else if (0 == wcscmp(pszName, g_wszWMDuration))
    {
        if (dataType == WMT_TYPE_QWORD)
        {
            LARGE_INTEGER li = (LARGE_INTEGER)(*(LARGE_INTEGER*)pBytes);
            UIASSERT(li.HighPart==0);  // 不去考虑太大的文件
            DWORD  nDuration = li.LowPart;  // 注：这里不能用int
            pCallback->SetDuration(nDuration/10000);  // The Duration attribute contains the playing duration of the file, in 100-nanosecond units.
        }
    }
    return true;
}