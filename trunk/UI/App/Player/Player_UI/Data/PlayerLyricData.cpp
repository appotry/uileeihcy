#include "stdafx.h"
#include "PlayerLyricData.h"


// 保存歌词数据为lrc文件
bool  CPlayerLyricData::Save(const TCHAR* szArtist, const TCHAR* szTitle, byte* pByte, int nSize)
{
    if (NULL == szArtist || NULL == szTitle)
        return false;

    // TODO: 增加文件保存路径配置
    String  strPath;
    this->GetDefaultLyricDir(strPath);
    strPath.append(szArtist);
    strPath.append(_T(" - "));
    strPath.append(szTitle);
    strPath.append(_T(".lrc"));

    fstream  f;
    f.open(strPath.c_str(), ios_base::out);
    if (f.fail())
        return false;

    f.write((char*)pByte, nSize);
    f.close();
    return true;
}

// 从默认目录中尝试加载指定的lrc文件
bool  CPlayerLyricData::Load(const TCHAR* szMp3FileName, const TCHAR* szArtist, const TCHAR* szTitle, IBuffer* pBuffer)
{
    String  strDir;
    this->GetDefaultLyricDir(strDir);

    // 1. 查找同名lrc
    if (szMp3FileName)
    {
        String  strPath = strDir;
        strPath.append(szMp3FileName);

        int nPos = (int)strPath.find_last_of(_T('.'));
        if (String::npos != nPos)
        {
            strPath = strPath.substr(0, nPos);
            strPath.append(_T(".lrc"));

            if (PathFileExists(strPath.c_str()))
            {
                return _LoadLrcFile(strPath, pBuffer);
            }
        }
    }
    
    // 2. 根据标题和歌手查找

    if (NULL == szTitle || NULL == szArtist)
        return false;

    String  strPath = strDir;
    strPath.append(szArtist);
    strPath.append(_T(" - "));
    strPath.append(szTitle);
    strPath.append(_T(".lrc"));

    if (!PathFileExists(strPath.c_str()))
        return false;
    return _LoadLrcFile(strPath, pBuffer);
}

bool  CPlayerLyricData::_LoadLrcFile(const String& strPath, IBuffer* pBuffer)
{
    fstream f;
    f.open(strPath.c_str());
    if (f.fail())
        return false;

    f.seekg(0, ios_base::end);
    int nFileSize = f.tellg();
    f.seekg(0, ios_base::beg);

    pBuffer->Alloc(nFileSize);
    f.read((char*)pBuffer->Get(), nFileSize);
    f.close();

    return true;
}

// 获取默认的歌词保存路径
void  CPlayerLyricData::GetDefaultLyricDir(String& str)
{
    TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
    _tcscat(szPath, _T("TTPlayer\\lyric\\"));

    str = szPath;
}
